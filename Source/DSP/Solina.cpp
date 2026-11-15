// Solina.cpp
// Solina (Eminent) String Ensemble, the string section of the Eminent 310
// organ made into an instrument: a top-octave generator divided down for all
// 49 keys (every C is phase-locked to every other C, as in the real divider
// chain), each key with its own CRESCENDO (attack) and SUSTAIN (release),
// six voice tabs (VIOLIN, VIOLA, TRUMPET, HORN, CELLO, CONTRABASS) as fixed
// formant filters on shared buses, and the ensemble: three bucket-brigade
// delays swept by a slow and a fast sine at 120 degree spacing, summed. That
// chorus is the sound; it can't be switched off on the original either.
// Voicing filters, the delay times and LFO rates are estimates (see README).

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kTopHzC = 4186.009;              // C8: the top octave the dividers start from
const double kBbdBaseMs = 6.0, kBbdSlowMs = 2.2, kBbdFastMs = 0.28;   // ensemble (estimates)
const double kSlowHz = 0.62, kFastHz = 6.1;                          // ensemble LFOs (estimates)
}

SolinaEngine::SolinaEngine() : sr (44100.0), lfoSlow (0.0), lfoFast (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    for (int i = 0; i < 12; ++i) topCount[i] = 0.08 * i;
    std::memset (down, 0, sizeof (down));
}

void SolinaEngine::prepare (double s)
{
    sr = s;
    for (int k = 0; k < kKeys; ++k) env[k].setRate (sr);
    updateEnv();
    violinHp.highpass (280.0, 0.7, sr);   violinPk.peak (2600.0, 1.2, 5.0, sr);  violinLp.lowpass (7000.0, 0.7, sr);
    violaLp.lowpass (2300.0, 0.8, sr);    violaPk.peak (950.0, 1.4, 4.0, sr);
    trumpetBp.bandpass (1300.0, 1.1, sr); trumpetHp.highpass (500.0, 0.7, sr);
    hornLp.lowpass (850.0, 0.9, sr);
    celloLp.lowpass (1500.0, 0.8, sr);    celloPk.peak (420.0, 1.3, 4.0, sr);
    bassLp.lowpass (600.0, 0.9, sr);      bassPk.peak (120.0, 1.2, 3.0, sr);
    bbdLp1.set (7500.0, sr); bbdLp2.set (7500.0, sr);
    dcBlock.set (12.0, sr);
    silence();
}

void SolinaEngine::updateEnv()
{
    for (int k = 0; k < kKeys; ++k)
    {
        env[k].setAttack (map::soCresc (pv[kSoCresc]));
        env[k].setDecay (0.01);
        env[k].setSustain (1.0);
        env[k].setRelease (map::soSustain (pv[kSoSust]));
    }
}

void SolinaEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kSoCresc || p == kSoSust) updateEnv();
}

void SolinaEngine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = true;
    env[k].gate (true, false);
}

void SolinaEngine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = false;
    env[k].gate (false);
}

void SolinaEngine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; env[k].gate (false); }
}

void SolinaEngine::silence()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; env[k].kill(); }
    bbd.clear();
    violinHp.reset(); violinPk.reset(); violinLp.reset(); violaLp.reset(); violaPk.reset();
    trumpetBp.reset(); trumpetHp.reset(); hornLp.reset(); celloLp.reset(); celloPk.reset(); bassLp.reset(); bassPk.reset();
    bbdLp1.reset(); bbdLp2.reset();
    dcBlock.reset();
}

void SolinaEngine::render (float* out, int n)
{
    const double tune = semisToRatio (map::soTuneCents (pv[kSoTune]) / 100.0);
    double topInc[12];
    for (int c = 0; c < 12; ++c) topInc[c] = kTopHzC * semisToRatio (c) * tune / sr;
    const bool vn = isOn (pv[kSoViolin]), va = isOn (pv[kSoViola]), tr = isOn (pv[kSoTrump]);
    const bool hn = isOn (pv[kSoHorn]), ce = isOn (pv[kSoCello]), cb = isOn (pv[kSoBass]);
    const double vol = pv[kSoVol];
    const double gain = (vol / 0.7) * (vol / 0.7) * 0.11;
    const double dSlow = kTwoPi * kSlowHz / sr, dFast = kTwoPi * kFastHz / sr;
    const double msToS = sr / 1000.0;

    for (int i = 0; i < n; ++i)
    {
        for (int c = 0; c < 12; ++c)
        {
            topCount[c] += topInc[c];
            if (topCount[c] >= 1024.0) topCount[c] -= 1024.0;
        }
        // the key buses: sawtooth 8', sawtooth 16' and square 8'
        double saw8 = 0.0, saw16 = 0.0, sq8 = 0.0;
        for (int k = 0; k < kKeys; ++k)
        {
            if (! env[k].active()) continue;
            const double e = env[k].tick();
            const int note = kLowKey + k;
            const int pc = note % 12;
            const int div = (108 + pc - note) / 12;          // octaves below the top-octave note
            const double scale = 1.0 / (double) (1 << div);
            const double ph8 = wrap01 (topCount[pc] * scale);
            const double dt8 = topInc[pc] * scale;
            const double ph16 = wrap01 (topCount[pc] * scale * 0.5);
            saw8 += e * sawBL (ph8, dt8);
            saw16 += e * sawBL (ph16, dt8 * 0.5);
            sq8 += e * pulseBL (ph8, dt8, 0.5);
        }
        double x = 0.0;
        if (vn) x += violinLp.tick (violinPk.tick (violinHp.tick (saw8))) * 0.9;
        if (va) x += violaPk.tick (violaLp.tick (saw8 + 0.5 * saw16)) * 0.8;
        if (tr) x += trumpetHp.tick (trumpetBp.tick (sq8)) * 1.6;
        if (hn) x += hornLp.tick (sq8) * 0.9;
        if (ce) x += celloPk.tick (celloLp.tick (saw8 + saw16)) * 0.7;
        if (cb) x += bassPk.tick (bassLp.tick (saw16)) * 1.1;

        // the ensemble: three swept delays, 120 degrees apart, summed
        bbd.push (x);
        lfoSlow += dSlow; if (lfoSlow > kTwoPi) lfoSlow -= kTwoPi;
        lfoFast += dFast; if (lfoFast > kTwoPi) lfoFast -= kTwoPi;
        double y = 0.0;
        for (int t = 0; t < 3; ++t)
        {
            const double off = t * kTwoPi / 3.0;
            const double ms = kBbdBaseMs + kBbdSlowMs * std::sin (lfoSlow + off) + kBbdFastMs * std::sin (lfoFast + off);
            y += bbd.read (ms * msToS);
        }
        y = bbdLp2.lp (bbdLp1.lp (y / 3.0));
        out[i] = (float) dcBlock.hp (y * gain);
    }
}

} // namespace tbsyn
