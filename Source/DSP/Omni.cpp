// Omni.cpp
// ARP Omni (model 2480, 1975-77), from its owner's and service manuals:
// - 49 keys, C2 - C6, every key sounding from a top-octave generator divided
//   down (one tone source for strings and synthesizer).
// - STRINGS: VIOLINS 4' and VIOLAS 8' are polyphonic over the whole keyboard;
//   CELLOS 8' and BASS 16' are monophonic, low-note priority, on the bottom 20
//   keys (C2 - G3). String ATTACK and RELEASE; RELEASE also sets how long the
//   synthesizer keys ring on.
// - SYNTHESIZER: 4' and 8' polyphonic; BASS 8' and 16' monophonic on the bottom
//   20 keys (when either is on, the poly 4' / 8' leave those keys); all four
//   pitches are summed into the VCF, the bass ones through the bass keying and
//   BASS VOLUME. WAVEFORM
//   SAW or DYNAMIC PULSE, ENHANCEMENT (a hollow, square-like tone), one shared
//   24 dB ladder VCF (FREQUENCY, RESONANCE, ADSR, LFO) and VCA, the ADSR
//   retriggered by every key; LFO about 0.1 - 20 Hz. CHORUS PHASER sends the
//   synthesizer through the string phasers, slower and deeper.
// - The bass keying decays while a key is held and cuts off quickly when the
//   bottom keys are let go.
// - The ensemble: three short delay lines modulated by three LFOs (clipped
//   triangles), summed with the dry signal.
// - GENERAL: STRING / SYNTH MIX, BASS VOLUME, MASTER. The sustain pedal holds
//   every note.
// Filter corners, voicing, envelope ranges and ensemble timings are estimates
// (see README); the structure follows the manuals.

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kTopHzC = 4186.009;
const double kSynthAttack = 0.003;      // synthesizer key gate (estimate)
const double kBassDecay = 2.5;          // bass keying decay while held (estimate, from the manual's description)
const double kBassCut = 0.03;           // bass release when the keys are up (estimate)
const double kEnvOct = 6.0;             // full VCF ADSR sweep (estimate)
const double kLfoOct = 2.0;             // full VCF LFO sweep (estimate)
const double kEnsBaseMs = 0.42, kEnsDepthMs = 0.3;                  // string phasers, up to 0.8 ms (estimates)
const double kEnsHz[3] = { 0.63, 0.81, 1.07 };
const double kChoBaseMs = 0.45, kChoDepthMs = 0.35, kChoRate = 0.45;  // CHORUS PHASER: slower, deeper (estimates)

// clipped triangle, -1 .. +1
double clipTri (double ph) { const double t = 4.0 * std::fabs (ph - 0.5) - 1.0; return clampd (1.3 * t, -1.0, 1.0); }
}

OmniEngine::OmniEngine()
    : sr (44100.0), pedal (false), gateUp (0.0), bassNote (-1), bassLevel (0.0), bassHeld (false), bassDecK (0.0), bassCutK (0.0), bassAtkK (0.0),
      bassTarget (0.0), bassRestrike (false), synDown (0.0), lfoPh (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    for (int i = 0; i < 12; ++i) topCount[i] = 0.13 * i;
    std::memset (down, 0, sizeof (down));
    std::memset (held, 0, sizeof (held));
    for (int k = 0; k < kKeys; ++k) synGate[k] = 0.0;
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
    for (int i = 0; i < 3; ++i) { ensPh[i] = i / 3.0; choPh[i] = i / 3.0 + 0.1; }
}

void OmniEngine::prepare (double s)
{
    sr = s;
    for (int k = 0; k < kKeys; ++k) strEnv[k].setRate (sr);
    adsr.setRate (sr);
    updateEnv();
    gateUp = 1.0 - std::exp (-1.0 / (kSynthAttack * sr));
    bassDecK = 1.0 - std::exp (-1.0 / (kBassDecay * sr));
    bassCutK = 1.0 - std::exp (-1.0 / (kBassCut * sr));
    bassAtkK = 1.0 - std::exp (-1.0 / (0.004 * sr));
    violinHp.highpass (350.0, 0.7, sr); violinPk.peak (2600.0, 1.1, 5.0, sr); violinLp.lowpass (8000.0, 0.7, sr);
    violaLp.lowpass (3200.0, 0.8, sr);  violaPk.peak (900.0, 1.2, 3.5, sr);
    celloLp.lowpass (1400.0, 0.8, sr);  celloPk.peak (300.0, 1.3, 4.0, sr);
    sbassLp.lowpass (600.0, 0.8, sr);
    synBassLp.lowpass (2200.0, 0.7, sr);
    ensLp.set (9000.0, sr);
    dcBlock.set (12.0, sr);
    masterS.setTime (0.01, sr); masterS.v = pv[kOmMaster];
    silence();
}

void OmniEngine::updateEnv()
{
    const double atk = map::omStrAttack (pv[kOmSAtk]), rel = map::omStrRelease (pv[kOmSRel]);
    for (int k = 0; k < kKeys; ++k)
    {
        strEnv[k].setAttack (atk);
        strEnv[k].setDecay (0.01);
        strEnv[k].setSustain (1.0);
        strEnv[k].setRelease (rel);
    }
    synDown = 1.0 - std::exp (-1.0 / (0.25 * rel * sr));
    adsr.setAttack (map::omAttack (pv[kOmA]));
    adsr.setDecay (map::omDecay (pv[kOmD]));
    adsr.setSustain (pv[kOmS]);
    adsr.setRelease (map::omDecay (pv[kOmR]));
}

void OmniEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kOmSAtk || p == kOmSRel || p == kOmA || p == kOmD || p == kOmS || p == kOmR) updateEnv();
}

int OmniEngine::lowestBassKey() const
{
    for (int k = 0; k < kBassKeys; ++k) if (held[k]) return k;
    return -1;
}

bool OmniEngine::anyHeld() const
{
    for (int k = 0; k < kKeys; ++k) if (held[k]) return true;
    return false;
}

void OmniEngine::updateBass (bool newKey)
{
    const int b = lowestBassKey();
    if (b >= 0)
    {
        const int note = kLowKey + b;
        if (newKey || note != bassNote) bassRestrike = true;
        bassNote = note;
        bassHeld = true;
    }
    else bassHeld = false;
}

void OmniEngine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = true;
    held[k] = true;
    strEnv[k].gate (true, false);
    adsr.gate (true, true);              // every key retriggers the ADSR
    if (k < kBassKeys) updateBass (true);
}

void OmniEngine::releaseKey (int k)
{
    held[k] = false;
    strEnv[k].gate (false);
    if (! anyHeld()) adsr.gate (false);
    if (k < kBassKeys) updateBass (false);
}

void OmniEngine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys || ! down[k]) return;
    down[k] = false;
    if (! pedal) releaseKey (k);
}

void OmniEngine::sustainPedal (bool d)
{
    pedal = d;
    if (! d) for (int k = 0; k < kKeys; ++k) if (held[k] && ! down[k]) releaseKey (k);
}

void OmniEngine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; held[k] = false; strEnv[k].gate (false); }
    adsr.gate (false);
    bassHeld = false;
}

void OmniEngine::silence()
{
    allNotesOff();
    pedal = false;
    for (int k = 0; k < kKeys; ++k) { strEnv[k].kill(); synGate[k] = 0.0; }
    adsr.kill();
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
    bassLevel = 0.0; bassNote = -1; bassRestrike = false;
    ens.clear(); cho.clear();
    violinHp.reset(); violinPk.reset(); violinLp.reset(); violaLp.reset(); violaPk.reset();
    celloLp.reset(); celloPk.reset(); sbassLp.reset(); synBassLp.reset();
    ensLp.reset(); dcBlock.reset();
}

void OmniEngine::render (float* out, int n)
{
    const bool violin = isOn (pv[kOmViolin]), viola = isOn (pv[kOmViola]);
    const bool cello = isOn (pv[kOmCello]), sbass = isOn (pv[kOmBass]);
    const bool sy4 = isOn (pv[kOmSy4]), sy8 = isOn (pv[kOmSy8]);
    const bool syB8 = isOn (pv[kOmSyB8]), syB16 = isOn (pv[kOmSyB16]);
    const bool pulse = pv[kOmWave] >= 0.5f, enh = isOn (pv[kOmEnh]), chorus = isOn (pv[kOmChorus]);
    const bool anyPolySyn = sy4 || sy8;
    const bool synBass = syB8 || syB16;
    const int polyLow = synBass ? kBassKeys : 0;              // synth bass takes the bottom keys from the poly synth
    const bool anyBass = cello || sbass || synBass;
    const double cut = std::log (map::omVcf (pv[kOmVcfF])) / std::log (2.0);
    const double kRes = 4.2 * pv[kOmVcfR];
    const double envOct = kEnvOct * pv[kOmVcfEnv], lfoOct = kLfoOct * pv[kOmVcfLfo];
    const double lfoHz = map::omLfo (pv[kOmLfo]);
    const double mix = pv[kOmMix];
    const double strGain = std::sqrt (1.0 - mix), synGain = std::sqrt (mix);   // equal-power MIX
    const double bassVol = pv[kOmBassV] * pv[kOmBassV];
    const double fs2 = 2.0 * sr;
    const double msToS = sr / 1000.0;
    double baseInc[12];
    for (int c = 0; c < 12; ++c) baseInc[c] = kTopHzC * semisToRatio (c) / sr;

    for (int i = 0; i < n; ++i)
    {
        for (int c = 0; c < 12; ++c)
        {
            topCount[c] += baseInc[c];
            if (topCount[c] >= 1024.0) topCount[c] -= 1024.0;
        }
        const double env = adsr.tick();
        lfoPh = wrap01 (lfoPh + lfoHz / sr);
        const double lfo = 1.0 - 4.0 * std::fabs (lfoPh - 0.5);       // triangle
        const double pw = pulse ? 0.5 - 0.4 * env : 0.5;              // DYNAMIC PULSE: width follows the ADSR (estimate)

        // the keyed buses
        double vlnBus = 0.0, vlaBus = 0.0, synBus = 0.0;
        for (int k = 0; k < kKeys; ++k)
        {
            const bool g = held[k];
            synGate[k] += ((g ? 1.0 : 0.0) - synGate[k]) * (g ? gateUp : synDown);
            const bool sAct = (violin || viola) && strEnv[k].active();
            const bool yAct = anyPolySyn && k >= polyLow && synGate[k] > 1e-4;
            if (! strEnv[k].active() && synGate[k] <= 1e-4) synGate[k] = 0.0;
            if (! sAct && ! yAct) { if (strEnv[k].active()) strEnv[k].tick(); continue; }
            const int note = kLowKey + k;
            const int pc = note % 12;
            const int div = (108 + pc - note) / 12;
            const double scale = 1.0 / (double) (1 << div);
            const double dt8 = baseInc[pc] * scale;
            const double ph8 = wrap01 (topCount[pc] * scale);
            const double ph4 = wrap01 (topCount[pc] * scale * 2.0);
            const double se = strEnv[k].tick();
            if (sAct)
            {
                if (violin) vlnBus += se * sawBL (ph4, dt8 * 2.0);
                if (viola) vlaBus += se * sawBL (ph8, dt8);
            }
            if (yAct)
            {
                double y = 0.0;
                if (sy8) y += pulse ? pulseBL (ph8, dt8, pw) : sawBL (ph8, dt8);
                if (sy4) y += pulse ? pulseBL (ph4, dt8 * 2.0, pw) : sawBL (ph4, dt8 * 2.0);
                if (enh)
                {
                    if (sy8) y += 0.8 * pulseBL (ph8, dt8, 0.5);
                    if (sy4) y += 0.8 * pulseBL (ph4, dt8 * 2.0, 0.5);
                }
                synBus += synGate[k] * y;
            }
        }

        // the bass keying: monophonic, lowest of the bottom 20 keys
        double cbBus = 0.0, sbBus = 0.0;
        {
            if (bassRestrike) { bassTarget = 1.0; bassRestrike = false; }
            if (bassHeld) { bassTarget += (0.0 - bassTarget) * bassDecK; bassLevel += (bassTarget - bassLevel) * bassAtkK; }
            else bassLevel += (0.0 - bassLevel) * bassCutK;
            if (anyBass && bassLevel > 1e-5 && bassNote >= 0)
            {
                const int pc = bassNote % 12;
                const int div = (108 + pc - bassNote) / 12;
                const double scale = 1.0 / (double) (1 << div);
                const double dt8 = baseInc[pc] * scale;
                const double ph8 = wrap01 (topCount[pc] * scale);
                const double ph16 = wrap01 (topCount[pc] * scale * 0.5);
                if (cello) cbBus += sawBL (ph8, dt8);
                if (sbass) sbBus += 1.2 * sawBL (ph16, dt8 * 0.5);
                double y = 0.0;
                if (syB8) y += pulse ? pulseBL (ph8, dt8, pw) : sawBL (ph8, dt8);
                if (syB16) y += pulse ? pulseBL (ph16, dt8 * 0.5, pw) : sawBL (ph16, dt8 * 0.5);
                if (enh)
                {
                    if (syB8) y += 0.8 * pulseBL (ph8, dt8, 0.5);
                    if (syB16) y += 0.8 * pulseBL (ph16, dt8 * 0.5, 0.5);
                }
                synBus += y * bassLevel * bassVol / 0.49;   // the service manual: all four pitches are summed into the VCF
                cbBus *= bassLevel; sbBus *= bassLevel;
            }
        }

        // strings: voicing, then the phasers
        double str = 0.0;
        if (violin) str += violinLp.tick (violinPk.tick (violinHp.tick (vlnBus))) * 0.7;
        if (viola) str += violaPk.tick (violaLp.tick (vlaBus)) * 0.6;
        const double strBass = (celloPk.tick (celloLp.tick (cbBus)) * 1.2 + sbassLp.tick (sbBus) * 1.6 ) * bassVol / 0.49;

        // synthesizer: the shared VCF and VCA
        double syn = 0.0;
        {
            const double x = softClip (synBus * 0.3) / 0.3;
            const double fc = clampd (std::pow (2.0, cut + envOct * env + lfoOct * lfo), 20.0, 0.45 * fs2);
            const double w = std::tan (kPi * fc / fs2);
            const double gg = w / (1.0 + w), beta = 1.0 - gg, G = gg * gg * gg * gg;
            double y4 = 0.0;
            for (int os = 0; os < 2; ++os)
            {
                const double S = gg * gg * gg * beta * lad[0] + gg * gg * beta * lad[1] + gg * beta * lad[2] + beta * lad[3];
                double in = softClip ((x - kRes * S) / (1.0 + kRes * G));
                for (int st = 0; st < 4; ++st) { const double v = (in - lad[st]) * gg; const double y = v + lad[st]; lad[st] = y + v; in = y; }
                y4 += in;
            }
            syn = y4 * 0.5 * (1.0 + 0.5 * kRes) * env;
        }

        // ensemble: strings always, synthesizer when CHORUS PHASER is on
        ens.push (str);
        double strOut = str;
        for (int t = 0; t < 3; ++t)
        {
            ensPh[t] = wrap01 (ensPh[t] + kEnsHz[t] / sr);
            strOut += ens.read ((kEnsBaseMs + kEnsDepthMs * clipTri (ensPh[t])) * msToS);
        }
        strOut = ensLp.lp (strOut * 0.25);
        double synOut = syn;
        cho.push (syn);
        if (chorus)
        {
            synOut = syn;
            for (int t = 0; t < 3; ++t)
            {
                choPh[t] = wrap01 (choPh[t] + kChoRate * kEnsHz[t] / sr);
                synOut += cho.read ((kChoBaseMs + kChoDepthMs * clipTri (choPh[t])) * msToS);
            }
            synOut *= 0.4;
        }

        const double m = masterS.tick (pv[kOmMaster]);
        double y = (strOut * 0.13 * strGain + synOut * 0.29 * synGain + strBass * 0.105) * (m / 0.7) * (m / 0.7);
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
