// OBX.cpp
// Oberheim OB-X: eight voices, each with two oscillators (sawtooth and pulse
// switchable together or separately, a shared PULSE WIDTH, osc 2 SYNC to
// osc 1, X-MOD: osc 2 modulating osc 1's frequency), NOISE, a 2-pole (12 dB)
// state-variable low-pass in the SEM tradition with RESONANCE and envelope
// FILTER MODULATION, keyboard tracking, and two ADSRs (filter, loudness).
// One modulation LFO (sine / square / sample & hold) with DEPTH, switched to
// osc 1, osc 2, the filter and the pulse width; the modulation wheel adds to
// DEPTH. Voices are assigned in rotation like the original's; each has a small
// fixed tuning offset (component tolerance) so chords beat gently.

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kModOscSemis = 12.0;     // full DEPTH to an oscillator (estimate)
const double kModFiltOct = 3.0;       // full DEPTH to the filter (estimate)
const double kModPw = 0.4;            // full DEPTH to the pulse width (estimate)
const double kXmodSemis = 18.0;       // X-MOD depth (estimate)
const double kBendSemis = 2.0;        // pitch lever (estimate)
const double kVoiceSpreadCents = 4.0; // per-voice tolerance (estimate)
}

OBXEngine::OBXEngine()
    : sr (44100.0), clock (0), nextVoice (0), lastPitch (60.0), lfoPh (0.0), lfoHold (0.0), bend (0.0), wheel (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    noise.seed (777u);
    Noise spread;
    spread.seed (4242u);
    for (int i = 0; i < kVoices; ++i)
    {
        v[i].note = -1; v[i].gate = false; v[i].age = 0;
        v[i].ph1 = spread.uniform(); v[i].ph2 = spread.uniform();
        v[i].pitch = 60.0;
        v[i].detune = spread.white() * kVoiceSpreadCents / 100.0;
    }
}

void OBXEngine::prepare (double s)
{
    sr = s;
    for (int i = 0; i < kVoices; ++i) { v[i].fenv.setRate (sr); v[i].lenv.setRate (sr); }
    dcBlock.set (10.0, sr);
    updateEnvTimes();
    silence();
}

void OBXEngine::updateEnvTimes()
{
    for (int i = 0; i < kVoices; ++i)
    {
        v[i].fenv.setAttack (map::oxAttack (pv[kOxFA]));
        v[i].fenv.setDecay (map::oxDecay (pv[kOxFD]));
        v[i].fenv.setSustain (pv[kOxFS]);
        v[i].fenv.setRelease (map::oxDecay (pv[kOxFR]));
        v[i].lenv.setAttack (map::oxAttack (pv[kOxLA]));
        v[i].lenv.setDecay (map::oxDecay (pv[kOxLD]));
        v[i].lenv.setSustain (pv[kOxLS]);
        v[i].lenv.setRelease (map::oxDecay (pv[kOxLR]));
    }
}

void OBXEngine::setParam (int p, float val)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = val;
    if (p >= kOxFA && p <= kOxLR) updateEnvTimes();
}

void OBXEngine::noteOn (int note, int)
{
    ++clock;
    int idx = -1;
    for (int i = 0; i < kVoices; ++i) if (v[i].note == note) { idx = i; break; }     // same key again
    if (idx < 0)
    {
        // next free voice in rotation, else the oldest
        for (int k = 0; k < kVoices && idx < 0; ++k)
        {
            const int i = (nextVoice + k) % kVoices;
            if (! v[i].gate && ! v[i].lenv.active()) idx = i;
        }
        for (int k = 0; k < kVoices && idx < 0; ++k)
        {
            const int i = (nextVoice + k) % kVoices;
            if (! v[i].gate) idx = i;
        }
        if (idx < 0)
        {
            unsigned oldest = 0xffffffffu;
            for (int i = 0; i < kVoices; ++i) if (v[i].age < oldest) { oldest = v[i].age; idx = i; }
        }
        nextVoice = (idx + 1) % kVoices;
    }
    Voice& vo = v[idx];
    if (map::oxPorta (pv[kOxPorta]) <= 0.0) vo.pitch = note;
    else vo.pitch = lastPitch;
    vo.note = note;
    vo.gate = true;
    vo.age = clock;
    vo.fenv.gate (true);
    vo.lenv.gate (true);
    lastPitch = note;
}

void OBXEngine::noteOff (int note)
{
    for (int i = 0; i < kVoices; ++i)
        if (v[i].note == note && v[i].gate)
        {
            v[i].gate = false;
            v[i].fenv.gate (false);
            v[i].lenv.gate (false);
        }
}

void OBXEngine::allNotesOff()
{
    for (int i = 0; i < kVoices; ++i) { v[i].gate = false; v[i].fenv.gate (false); v[i].lenv.gate (false); }
}

void OBXEngine::silence()
{
    for (int i = 0; i < kVoices; ++i)
    {
        v[i].gate = false; v[i].note = -1;
        v[i].fenv.kill(); v[i].lenv.kill();
        v[i].svf.reset();
    }
    dcBlock.reset();
}

void OBXEngine::render (float* out, int n)
{
    const double lfoHz = map::oxLfo (pv[kOxLfoRt]);
    const int lfoWave = paramStep (kOxLfoWv, pv[kOxLfoWv]);
    const double depth = clampd (pv[kOxModDp] + wheel, 0.0, 1.0);
    const bool m1 = isOn (pv[kOxModO1]), m2 = isOn (pv[kOxModO2]), mf = isOn (pv[kOxModFl]), mp = isOn (pv[kOxModPw]);
    const double f1 = map::oxOscSemis (pv[kOxO1Frq]);
    const double f2 = map::oxOscSemis (pv[kOxO2Frq]) + map::oxDetuneCents (pv[kOxO2Det]) / 100.0;
    const double pw0 = map::oxPulseWidth (pv[kOxPW]);
    const bool s1 = isOn (pv[kOxO1Saw]), p1 = isOn (pv[kOxO1Pls]), s2 = isOn (pv[kOxO2Saw]), p2 = isOn (pv[kOxO2Pls]);
    const bool sync = isOn (pv[kOxSync]), xmod = isOn (pv[kOxXmod]);
    const double cut = std::log (map::oxCutoff (pv[kOxCut])) / std::log (2.0);
    const double q = 0.55 * std::pow (36.0, (double) pv[kOxRes]);     // 0.55 .. 20
    const double envOct = map::oxEnvOct (pv[kOxEnvAm]);
    const double nzl = pv[kOxNoise] * pv[kOxNoise];
    const bool kbd = isOn (pv[kOxKbd]);
    const double pt = map::oxPorta (pv[kOxPorta]);
    const double porK = pt > 0.0 ? 1.0 - std::exp (-1.0 / (pt * sr * 0.3)) : 1.0;
    const double vol = pv[kOxVol];
    const double gain = (vol / 0.6) * (vol / 0.6) * 0.2;

    for (int i = 0; i < n; ++i)
    {
        // the LFO
        const double lph = lfoPh + lfoHz / sr;
        if (lph >= 1.0 && lfoWave == 2) lfoHold = noise.white();
        lfoPh = wrap01 (lph);
        double lfo = lfoWave == 0 ? std::sin (kTwoPi * lfoPh) : (lfoWave == 1 ? (lfoPh < 0.5 ? 1.0 : -1.0) : lfoHold);
        lfo *= depth;
        const double nz = noise.white();

        double sum = 0.0;
        for (int k = 0; k < kVoices; ++k)
        {
            Voice& vo = v[k];
            if (! vo.lenv.active()) continue;
            vo.pitch += (vo.note - vo.pitch) * porK;
            const double base = vo.pitch + vo.detune + bend * kBendSemis;

            // osc 2 first (X-MOD source)
            double sm2 = base + f2 + (m2 ? lfo * kModOscSemis : 0.0);
            const double dt2 = clampd (noteHz (sm2), 1.0, 0.45 * sr) / sr;
            double sm1 = base + f1 + (m1 ? lfo * kModOscSemis : 0.0);
            const double o2raw = sawBL (vo.ph2, dt2);
            if (xmod) sm1 += o2raw * kXmodSemis * 0.5;
            const double dt1 = clampd (noteHz (sm1), 1.0, 0.45 * sr) / sr;
            const double pw = clampd (pw0 + (mp ? lfo * kModPw : 0.0), 0.04, 0.96);

            double o1 = 0.0, o2 = 0.0;
            if (s1) o1 += sawBL (vo.ph1, dt1);
            if (p1) o1 += pulseBL (vo.ph1, dt1, pw);
            if (s2) o2 += o2raw;
            if (p2) o2 += pulseBL (vo.ph2, dt2, pw);

            vo.ph1 += dt1;
            vo.ph2 += dt2;
            if (vo.ph1 >= 1.0)
            {
                vo.ph1 -= 1.0;
                if (sync) vo.ph2 = vo.ph1 * dt2 / dt1;     // osc 2 restarts with osc 1
            }
            if (vo.ph2 >= 1.0) vo.ph2 -= std::floor (vo.ph2);

            const double x = 0.5 * (o1 + o2) + nzl * nz;

            const double fe = vo.fenv.tick();
            double oct = cut + envOct * fe + (kbd ? (vo.pitch - 60.0) / 12.0 : 0.0) + (mf ? lfo * kModFiltOct : 0.0);
            const double fc = std::pow (2.0, clampd (oct, 3.0, 14.6));
            vo.svf.set (fc, q, sr);
            vo.svf.tick (softClip (x * 0.6) / 0.6);
            const double y = vo.svf.low;
            // the 2-pole's band state rounds off when the resonance rings hard
            vo.svf.ic1 = 2.4 * softClip (vo.svf.ic1 / 2.4);
            sum += y * vo.lenv.tick();
        }
        // the voice summing and output amplifiers round off when pushed
        out[i] = (float) dcBlock.hp (0.95 * softClip (sum * gain / 0.95));
    }
}

} // namespace tbsyn
