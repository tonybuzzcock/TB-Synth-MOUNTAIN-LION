// Moog.cpp
// Minimoog Model D, from the owner's manual and the field service manual:
// - three oscillators, RANGE LO / 32' / 16' / 8' / 4' / 2', six waveforms each
//   (osc 3 has reverse sawtooth in place of triangle / sawtooth), osc 2 and 3
//   FREQUENCY knobs; OSC. 3 CONTROL off frees osc 3 from the keyboard
// - MIXER: three oscillator volumes and NOISE (white / pink), each with its
//   switch; driving the mixer hard saturates the filter input (the Model D's
//   famous mixer overdrive)
// - the 24 dB/oct transistor ladder (spec: 10 Hz - 20 kHz), EMPHASIS that
//   starts to self-oscillate between 7 and 8 (service manual), AMOUNT OF
//   CONTOUR 0 - 4 octaves (spec), KEYBOARD CONTROL 1 and 2 (1/3 and 2/3
//   tracking, both = full)
// - FILTER and LOUDNESS CONTOURS: attack 1 ms - 10 s, decay 4 ms - 35 s,
//   sustain (spec); with DECAY on the release follows the decay time
// - low-note priority, single triggering (legato notes don't restart the
//   contours), GLIDE 1 ms - 10 s (spec)
// - MODULATION MIX between osc 3 and noise, amount from the modulation wheel,
//   to the oscillators (OSC. MODULATION) and / or the filter (FILTER MODULATION)
// - pitch wheel: a fifth up and down (manual)
// The ladder runs at twice the sample rate.

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kRangeSemis[6] = { -108.0, -24.0, -12.0, 0.0, 12.0, 24.0 };   // LO (estimate: 9 octaves down), 32' .. 2'
const double kFastRelease = 0.008;       // release with DECAY off (estimate)
const double kOscModSemis = 12.0;        // full wheel, osc modulation depth (estimate)
const double kFiltModOct = 2.0;          // full wheel, filter modulation depth (estimate)
const double kEmphK = 5.3;               // ladder feedback at 10; reaches 4 (self-oscillation) at 7.5
const double kTrackRef = 48.0;           // key where keyboard control leaves the cutoff unchanged (estimate)
const double kBendSemis = 7.0;           // pitch wheel: a fifth (manual)

double moogWave (int wave, bool osc3, double ph, double dt)
{
    switch (wave)
    {
        case 0: return triBL (ph, dt);
        case 1: return osc3 ? -sawBL (ph, dt) : 0.5 * (triBL (ph, dt) + sawBL (ph, dt));
        case 2: return sawBL (ph, dt);
        case 3: return pulseBL (ph, dt, 0.5);
        case 4: return pulseBL (ph, dt, 0.30) * 1.1;     // wide rectangle (estimate 30 %)
        default: return pulseBL (ph, dt, 0.12) * 1.4;    // narrow rectangle (estimate 12 %)
    }
}
}

MoogEngine::MoogEngine()
    : sr (44100.0), heldCount (0), target (60.0), pitch (60.0), glideK (1.0), bend (0.0), wheel (0.0), wheelS (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    std::memset (held, 0, sizeof (held));
    ph[0] = 0.0; ph[1] = 0.33; ph[2] = 0.71;
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
    noise.seed (1234u);
}

void MoogEngine::prepare (double s)
{
    sr = s;
    fenv.setRate (sr);
    lenv.setRate (sr);
    dcBlock.set (8.0, sr);
    cutS.setTime (0.003, sr);
    volS.setTime (0.01, sr);
    cutS.v = std::log (map::mgCutoff (pv[kMgCutoff])) / std::log (2.0);
    volS.v = pv[kMgVol];
    updateEnvTimes();
    silence();
}

void MoogEngine::updateEnvTimes()
{
    fenv.setAttack (map::mgAttack (pv[kMgFAtk]));
    fenv.setDecay (map::mgDecay (pv[kMgFDec]));
    fenv.setSustain (pv[kMgFSus]);
    lenv.setAttack (map::mgAttack (pv[kMgLAtk]));
    lenv.setDecay (map::mgDecay (pv[kMgLDec]));
    lenv.setSustain (pv[kMgLSus]);
    const bool decay = isOn (pv[kMgDecay]);
    fenv.setRelease (decay ? map::mgDecay (pv[kMgFDec]) : kFastRelease);
    lenv.setRelease (decay ? map::mgDecay (pv[kMgLDec]) : kFastRelease);
    const double gt = map::mgGlide (pv[kMgGlide]);
    glideK = 1.0 - std::exp (-1.0 / (gt * sr * 0.3));
}

void MoogEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kMgFAtk || p == kMgFDec || p == kMgFSus || p == kMgLAtk || p == kMgLDec || p == kMgLSus
        || p == kMgDecay || p == kMgGlide)
        updateEnvTimes();
}

int MoogEngine::lowestHeld() const
{
    for (int k = 0; k < 128; ++k) if (held[k]) return k;
    return -1;
}

void MoogEngine::noteOn (int note, int)
{
    if (note < 0 || note > 127) return;
    const bool first = heldCount == 0;
    if (! held[note]) { held[note] = true; ++heldCount; }
    target = lowestHeld();
    if (first)
    {
        if (! isOn (pv[kMgGlideOn])) pitch = target;
        fenv.gate (true);
        lenv.gate (true);
    }
}

void MoogEngine::noteOff (int note)
{
    if (note < 0 || note > 127 || ! held[note]) return;
    held[note] = false;
    --heldCount;
    if (heldCount > 0) target = lowestHeld();
    else { fenv.gate (false); lenv.gate (false); }
}

void MoogEngine::allNotesOff()
{
    std::memset (held, 0, sizeof (held));
    heldCount = 0;
    fenv.gate (false);
    lenv.gate (false);
}

void MoogEngine::silence()
{
    allNotesOff();
    fenv.kill(); lenv.kill();
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
    dcBlock.reset();
}

void MoogEngine::render (float* out, int n)
{
    const double tune = map::mgTuneSemis (pv[kMgTune]);
    const int rng[3] = { paramStep (kMgO1Rng, pv[kMgO1Rng]), paramStep (kMgO2Rng, pv[kMgO2Rng]), paramStep (kMgO3Rng, pv[kMgO3Rng]) };
    const int wav[3] = { paramStep (kMgO1Wave, pv[kMgO1Wave]), paramStep (kMgO2Wave, pv[kMgO2Wave]), paramStep (kMgO3Wave, pv[kMgO3Wave]) };
    const double fr2 = map::mgOscFreqSemis (pv[kMgO2Freq]), fr3 = map::mgOscFreqSemis (pv[kMgO3Freq]);
    // mixer: audio-taper volume pots
    const double g[3] = { isOn (pv[kMgO1On]) ? pv[kMgO1Vol] * pv[kMgO1Vol] : 0.0,
                          isOn (pv[kMgO2On]) ? pv[kMgO2Vol] * pv[kMgO2Vol] : 0.0,
                          isOn (pv[kMgO3On]) ? pv[kMgO3Vol] * pv[kMgO3Vol] : 0.0 };
    const double gn = isOn (pv[kMgNzOn]) ? pv[kMgNzVol] * pv[kMgNzVol] : 0.0;
    const bool pink = paramStep (kMgNzCol, pv[kMgNzCol]) == 1;
    const bool o3kb = isOn (pv[kMgO3Ctrl]);
    const bool oscMod = isOn (pv[kMgOscMod]), filtMod = isOn (pv[kMgFltMod]);
    const double track = (isOn (pv[kMgKbd1]) ? 1.0 / 3.0 : 0.0) + (isOn (pv[kMgKbd2]) ? 2.0 / 3.0 : 0.0);
    const double cutKnob = std::log (map::mgCutoff (pv[kMgCutoff])) / std::log (2.0);
    const double contour = map::mgContour (pv[kMgContAm]);
    const double k = kEmphK * pv[kMgEmph];
    const double mix = pv[kMgModMix];
    const bool glideOn = isOn (pv[kMgGlideOn]);
    const double fs2 = 2.0 * sr;
    const double wheelK = 1.0 - std::exp (-1.0 / (0.01 * sr));

    for (int i = 0; i < n; ++i)
    {
        pitch = glideOn ? pitch + (target - pitch) * glideK : target;
        wheelS += (wheel - wheelS) * wheelK;

        // osc 3 first: it is also a modulation source
        const double base = pitch + tune + bend * kBendSemis;
        double o[3];
        double mod = 0.0;
        {
            const double s3 = (o3kb ? base : 48.0 + tune) + kRangeSemis[rng[2]] + fr3;
            const double f3 = clampd (noteHz (s3), 0.01, 0.45 * sr);
            const double dt = f3 / sr;
            o[2] = moogWave (wav[2], true, ph[2], dt);
            ph[2] = wrap01 (ph[2] + dt);
        }
        const double nz = pink ? noise.pink() * 2.2 : noise.white() * 0.6;
        mod = ((1.0 - mix) * o[2] + mix * nz) * wheelS;
        for (int j = 0; j < 2; ++j)
        {
            double s = base + kRangeSemis[rng[j]] + (j == 1 ? fr2 : 0.0);
            if (oscMod) s += mod * kOscModSemis;
            const double f = clampd (noteHz (s), 0.01, 0.45 * sr);
            const double dt = f / sr;
            o[j] = moogWave (wav[j], false, ph[j], dt);
            ph[j] = wrap01 (ph[j] + dt);
        }

        // mixer -> filter input, saturating when driven hard
        const double sum = g[0] * o[0] + g[1] * o[1] + g[2] * o[2] + gn * nz;
        const double x = softClip (sum * 0.55) / 0.55;

        // cutoff in octaves: knob + contour + keyboard + modulation
        const double fe = fenv.tick();
        double oct = cutKnob + contour * fe + track * (pitch - kTrackRef) / 12.0;
        if (filtMod) oct += mod * kFiltModOct;
        const double fc = clampd (std::pow (2.0, cutS.tick (oct)), 10.0, 0.45 * fs2);
        const double w = std::tan (kPi * fc / fs2);
        const double gg = w / (1.0 + w), beta = 1.0 - gg;
        const double G = gg * gg * gg * gg;
        double y4 = 0.0;
        for (int os = 0; os < 2; ++os)
        {
            const double S = gg * gg * gg * beta * lad[0] + gg * gg * beta * lad[1] + gg * beta * lad[2] + beta * lad[3];
            double u = (x - k * S) / (1.0 + k * G);
            u = softClip (u);
            double in = u;
            for (int st = 0; st < 4; ++st)
            {
                const double v = (in - lad[st]) * gg;
                const double y = v + lad[st];
                lad[st] = y + v;
                in = y;
            }
            y4 += in;
        }
        y4 *= 0.5;

        // loudness contour and output amplifier (overdrives above about 6)
        const double le = lenv.tick();
        const double vol = volS.tick (pv[kMgVol]);
        const double a = y4 * le * (vol / 0.6) * 0.8;
        const double yo = 0.7 * softClip (a / 0.7);
        out[i] = (float) dcBlock.hp (yo);
    }
}

} // namespace tbsyn
