// Rhapsody.cpp
// Elka Rhapsody 610 (1974-80), from owners' descriptions of the instrument and
// its boards:
// - 61 keys, a top-octave master oscillator and divider chips, then flip-flop
//   dividers for the lower octaves: every key phase-locked to its octaves.
// - Four sounds, each with a CANCEL: VIOLONCELLO (an octave below the
//   strings), STRINGS, PIANO and CLAVICHORD.
// - The keyboard splits at 2 or 3 octaves (25 / 37 keys below), with a
//   VOLUME for each half; three DECAY sliders (violoncello, strings, piano and
//   clavichord) set how long notes ring on.
// - One voice board per key with fixed filters, voiced differently across the
//   keyboard; then two chorus stages on a bucket-brigade delay, one fast and
//   moderate ("life"), one slow (beating). The piano has its own output.
// Filters, envelope times, the chorus timings and the piano and clavichord
// tones are estimates (see README).

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kTopHzC = 4186.009;
const double kStrAttack = 0.06;                  // string keying (estimate)
const double kFastHz = 5.8, kFastMs = 0.35;      // chorus stage 1: fast, moderate (estimates)
const double kSlowHz = 0.45, kSlowMs = 1.9;      // chorus stage 2: slow, beating (estimates)
const double kBaseMs = 4.0;
}

RhapsodyEngine::RhapsodyEngine() : sr (44100.0), ph1 (0.0), ph2 (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    for (int i = 0; i < 12; ++i) topCount[i] = 0.09 * i;
    std::memset (down, 0, sizeof (down));
}

void RhapsodyEngine::prepare (double s)
{
    sr = s;
    for (int k = 0; k < kKeys; ++k) { strEnv[k].setRate (sr); celEnv[k].setRate (sr); pnoEnv[k].setRate (sr); }
    updateEnv();
    strHp.highpass (220.0, 0.7, sr); strPk.peak (2400.0, 1.1, 4.5, sr); strLp.lowpass (6500.0, 0.7, sr);
    celLp.lowpass (1600.0, 0.8, sr); celPk.peak (380.0, 1.3, 4.0, sr);
    pnoLp.lowpass (2600.0, 0.7, sr); pnoPk.peak (900.0, 1.0, 3.0, sr);
    clvHp.highpass (380.0, 0.7, sr); clvPk.peak (2100.0, 1.4, 6.0, sr);
    bbdLp.set (8000.0, sr);
    dcBlock.set (12.0, sr);
    silence();
}

void RhapsodyEngine::updateEnv()
{
    const double pd = map::rhPianoDecay (pv[kRhDecPno]);
    for (int k = 0; k < kKeys; ++k)
    {
        strEnv[k].setAttack (kStrAttack); strEnv[k].setDecay (0.01); strEnv[k].setSustain (1.0);
        strEnv[k].setRelease (map::rhRelease (pv[kRhDecStr]));
        celEnv[k].setAttack (kStrAttack * 1.3); celEnv[k].setDecay (0.01); celEnv[k].setSustain (1.0);
        celEnv[k].setRelease (map::rhRelease (pv[kRhDecCello]));
        // piano / clavichord: struck, dying away while held (lower keys ring longer), the slider sets how long
        const double keyScale = 1.6 - 1.1 * k / (double) (kKeys - 1);
        pnoEnv[k].setAttack (0.002); pnoEnv[k].setDecay (pd * keyScale); pnoEnv[k].setSustain (0.0);
        pnoEnv[k].setRelease (0.35 * pd * keyScale);
    }
}

void RhapsodyEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kRhDecCello || p == kRhDecStr || p == kRhDecPno) updateEnv();
}

void RhapsodyEngine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = true;
    strEnv[k].gate (true, false);
    celEnv[k].gate (true, false);
    pnoEnv[k].gate (true, true);
}

void RhapsodyEngine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = false;
    strEnv[k].gate (false); celEnv[k].gate (false); pnoEnv[k].gate (false);
}

void RhapsodyEngine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; strEnv[k].gate (false); celEnv[k].gate (false); pnoEnv[k].gate (false); }
}

void RhapsodyEngine::silence()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; strEnv[k].kill(); celEnv[k].kill(); pnoEnv[k].kill(); }
    bbd1.clear(); bbd2.clear();
    strHp.reset(); strPk.reset(); strLp.reset(); celLp.reset(); celPk.reset();
    pnoLp.reset(); pnoPk.reset(); clvHp.reset(); clvPk.reset();
    bbdLp.reset(); dcBlock.reset();
}

void RhapsodyEngine::render (float* out, int n)
{
    double topInc[12];
    for (int c = 0; c < 12; ++c) topInc[c] = kTopHzC * semisToRatio (c) / sr;
    const bool ce = isOn (pv[kRhCello]), st = isOn (pv[kRhStrings]), pn = isOn (pv[kRhPiano]), cl = isOn (pv[kRhClav]);
    const int split = stepOf (pv[kRhSplit], 3);
    const int splitKeys = split == 1 ? 25 : (split == 2 ? 37 : 0);    // keys below the split
    const double sLo = pv[kRhVolStrLo] * pv[kRhVolStrLo], sHi = pv[kRhVolStrHi] * pv[kRhVolStrHi];
    const double pLo = pv[kRhVolPnLo] * pv[kRhVolPnLo], pHi = pv[kRhVolPnHi] * pv[kRhVolPnHi];
    const double m = pv[kRhMaster] / 0.7;
    const double gain = m * m;
    const double msToS = sr / 1000.0;

    for (int i = 0; i < n; ++i)
    {
        for (int c = 0; c < 12; ++c)
        {
            topCount[c] += topInc[c];
            if (topCount[c] >= 1024.0) topCount[c] -= 1024.0;
        }
        double strBus = 0.0, celBus = 0.0, pnoBus = 0.0, clvBus = 0.0;
        for (int k = 0; k < kKeys; ++k)
        {
            const bool sA = strEnv[k].active(), cA = celEnv[k].active(), pA = pnoEnv[k].active();
            if (! sA && ! cA && ! pA) continue;
            const double se = strEnv[k].tick(), cee = celEnv[k].tick(), pe = pnoEnv[k].tick();
            const bool lower = k < splitKeys;
            const int note = kLowKey + k;
            const int pc = note % 12;
            const int div = (108 + pc - note) / 12;
            const double scale = 1.0 / (double) (1 << div);
            const double dt8 = topInc[pc] * scale;
            const double ph8 = wrap01 (topCount[pc] * scale);
            // a touch of the per-board voicing: the top of the keyboard a little darker
            const double tilt = 1.0 - 0.25 * k / (double) (kKeys - 1);
            if (st && sA) strBus += se * (lower ? sLo : sHi) * tilt * sawBL (ph8, dt8);
            if (ce && cA) celBus += cee * (lower ? sLo : sHi) * sawBL (wrap01 (topCount[pc] * scale * 0.5), dt8 * 0.5);
            if ((pn || cl) && pA)
            {
                const double ph4 = wrap01 (topCount[pc] * scale * 2.0);
                const double g = pe * (lower ? pLo : pHi);
                if (pn) pnoBus += g * (pulseBL (ph8, dt8, 0.5) + 0.45 * sawBL (ph4, dt8 * 2.0));
                if (cl) clvBus += g * (pulseBL (ph8, dt8, 0.22) + 0.35 * pulseBL (ph4, dt8 * 2.0, 0.22));
            }
        }

        // voicing, then the two chorus stages for the strings and violoncello
        double x = 0.0;
        if (st) x += strLp.tick (strPk.tick (strHp.tick (strBus))) * 0.85;
        if (ce) x += celPk.tick (celLp.tick (celBus)) * 0.8;
        ph1 += kTwoPi * kFastHz / sr; if (ph1 > kTwoPi) ph1 -= kTwoPi;
        ph2 += kTwoPi * kSlowHz / sr; if (ph2 > kTwoPi) ph2 -= kTwoPi;
        bbd1.push (x);
        const double y1 = 0.5 * x + 0.5 * bbd1.read ((kBaseMs + kFastMs * std::sin (ph1)) * msToS);
        bbd2.push (y1);
        const double y2 = 0.5 * y1 + 0.5 * bbd2.read ((kBaseMs + 1.5 + kSlowMs * std::sin (ph2)) * msToS);
        const double strOut = bbdLp.lp (y2);

        double pno = 0.0;
        if (pn) pno += pnoPk.tick (pnoLp.tick (pnoBus)) * 0.7;
        if (cl) pno += clvPk.tick (clvHp.tick (clvBus)) * 0.3;

        double y = (strOut * 0.3 + pno * 0.5) * gain;
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
