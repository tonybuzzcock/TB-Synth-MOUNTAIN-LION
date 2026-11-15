// Multiman.cpp
// Crumar Multiman (1975), from its schematic diagram (DWG 1-12, 7/4/75):
// - a top-octave tone generator divided down for all 61 keys (F to F); the
//   whole generator is swept by the VIBRATO oscillator (SPEED, DEPTH, ON)
// - per key, the gates and key contacts (DWG 4): a string keyer taking the 8'
//   and 4' divider outputs, a piano keyer that strikes and decays, and brass
//   key contacts. Every bus is split into a LOWER and an UPPER half (P1 / P2,
//   8A / 8B, D1 / D2), hence two VOLUME controls and two ON tabs per section.
//   SUSTAIN LENGTH sets how long strings, piano and clavichord ring on after
//   the key is let go.
// - BRASS (DWG 5): the keyed brass bus through a four-stage transistor ladder
//   (TR1-TR10, 47 nF per stage, exponential converter TR25) with EMPH.
//   (resonance) and CONTOUR (cutoff), and an attack / decay envelope that every
//   key contact retriggers (the 22 nF caps on the D lines).
// - PIANO / CLAVICHORD (DWG 6): the struck bus through the piano filter, the
//   clavichord filter, or both.
// - STRINGS (DWG 7): CELLO and VIOLIN filters on the string bus, then the
//   delay-line ensemble (DWG 10).
// - BASS PEDALS (DWG 11): a monophonic 13-note pedal board; here the lowest 13
//   keys (F1 - F2) play it when BASS is on.
// Filter corners, envelope ranges, vibrato and ensemble timings are estimates
// (see README); the structure follows the schematic.

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kTopHzC = 4186.009;
const double kStringAttack = 0.06;     // the string keyer's 2.2 uF charging (estimate)
const double kBrassEnvOct = 4.0;       // envelope sweep of the brass ladder (estimate)
const double kVibSemis = 0.6;          // full VIBRATO DEPTH (estimate)
const double kEnsBaseMs = 5.5, kEnsSlowMs = 2.0, kEnsFastMs = 0.3, kEnsSlowHz = 0.7, kEnsFastHz = 5.8;   // (estimates)
double pianoDecay (int note) { return 1.6 * std::pow (0.5, (note - 29) / 24.0); }   // struck decay while held (estimate)
}

MultimanEngine::MultimanEngine()
    : sr (44100.0), gateUp (0.0), gateDown (0.0), bassNote (-1), bassPh (0.0), bassLevel (0.0), bassTarget (0.0), bassK (0.0),
      lfoSlow (0.0), lfoFast (0.0), vibPh (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    for (int i = 0; i < 12; ++i) topCount[i] = 0.11 * i;
    std::memset (down, 0, sizeof (down));
    for (int k = 0; k < kKeys; ++k) brassGate[k] = 0.0;
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
}

void MultimanEngine::prepare (double s)
{
    sr = s;
    for (int k = 0; k < kKeys; ++k) { strEnv[k].setRate (sr); pnoEnv[k].setRate (sr); }
    brassEnv.setRate (sr);
    updateEnv();
    gateUp = 1.0 - std::exp (-1.0 / (0.004 * sr));
    gateDown = 1.0 - std::exp (-1.0 / (0.03 * sr));
    bassK = 1.0 - std::exp (-1.0 / (0.01 * sr));
    bassLp1.set (700.0, sr); bassLp2.set (900.0, sr);
    celloLp.lowpass (1100.0, 0.8, sr);  celloPk.peak (320.0, 1.3, 4.0, sr);
    violinHp.highpass (300.0, 0.7, sr); violinPk.peak (2400.0, 1.2, 5.0, sr); violinLp.lowpass (7500.0, 0.7, sr);
    pianoLp.lowpass (2600.0, 0.9, sr);
    clavHp.highpass (700.0, 0.8, sr);   clavPk.peak (2200.0, 1.5, 6.0, sr);
    ensLp1.set (8000.0, sr); ensLp2.set (8000.0, sr);
    dcBlock.set (12.0, sr);
    masterS.setTime (0.01, sr); masterS.v = pv[kCrMaster];
    silence();
}

void MultimanEngine::updateEnv()
{
    const double rel = map::crSustain (pv[kCrSust]);
    for (int k = 0; k < kKeys; ++k)
    {
        strEnv[k].setAttack (kStringAttack);
        strEnv[k].setDecay (0.01);
        strEnv[k].setSustain (1.0);
        strEnv[k].setRelease (rel);
        pnoEnv[k].setAttack (0.0005);
        pnoEnv[k].setDecay (pianoDecay (kLowKey + k));
        pnoEnv[k].setSustain (0.0);
        pnoEnv[k].setRelease (rel * 0.6);
    }
    brassEnv.setAttack (map::crAttack (pv[kCrAtk]));
    brassEnv.setDecay (map::crDecay (pv[kCrDec]));
    brassEnv.setSustain (0.0);
    brassEnv.setRelease (map::crDecay (pv[kCrDec]));
}

void MultimanEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kCrSust || p == kCrAtk || p == kCrDec) updateEnv();
}

int MultimanEngine::lowestBassKey() const
{
    for (int k = 0; k < kBassKeys; ++k) if (down[k]) return k;
    return -1;
}

void MultimanEngine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    down[k] = true;
    strEnv[k].gate (true, false);
    pnoEnv[k].gate (true, true);
    brassEnv.gate (true, true);           // every key contact retriggers the brass contour
    if (k < kBassKeys)
    {
        const int b = lowestBassKey();
        if (b >= 0) { bassNote = kLowKey + b - 12; bassTarget = 1.0; }
    }
}

void MultimanEngine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys || ! down[k]) return;
    down[k] = false;
    strEnv[k].gate (false);
    pnoEnv[k].gate (false);
    bool any = false;
    for (int j = 0; j < kKeys; ++j) if (down[j]) any = true;
    if (! any) brassEnv.gate (false);
    if (k < kBassKeys)
    {
        const int b = lowestBassKey();
        if (b >= 0) bassNote = kLowKey + b - 12;
        else bassTarget = 0.0;
    }
}

void MultimanEngine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; strEnv[k].gate (false); pnoEnv[k].gate (false); }
    brassEnv.gate (false);
    bassTarget = 0.0;
}

void MultimanEngine::silence()
{
    allNotesOff();
    for (int k = 0; k < kKeys; ++k) { strEnv[k].kill(); pnoEnv[k].kill(); brassGate[k] = 0.0; }
    brassEnv.kill();
    for (int i = 0; i < 4; ++i) lad[i] = 0.0;
    bassLevel = 0.0; bassNote = -1;
    ens.clear();
    celloLp.reset(); celloPk.reset(); violinHp.reset(); violinPk.reset(); violinLp.reset();
    pianoLp.reset(); clavHp.reset(); clavPk.reset(); ensLp1.reset(); ensLp2.reset();
    bassLp1.reset(); bassLp2.reset(); dcBlock.reset();
}

void MultimanEngine::render (float* out, int n)
{
    const double brLo = isOn (pv[kCrBrOnLo]) ? pv[kCrBrLo] * pv[kCrBrLo] : 0.0;
    const double brHi = isOn (pv[kCrBrOnHi]) ? pv[kCrBrHi] * pv[kCrBrHi] : 0.0;
    const double pnLo = pv[kCrPnLo] * pv[kCrPnLo], pnHi = pv[kCrPnHi] * pv[kCrPnHi];
    const bool piano = isOn (pv[kCrPiano]), clav = isOn (pv[kCrClav]);
    const double ceLo = pv[kCrCeLo] * pv[kCrCeLo], ceHi = pv[kCrCeHi] * pv[kCrCeHi];
    const double viLo = pv[kCrViLo] * pv[kCrViLo], viHi = pv[kCrViHi] * pv[kCrViHi];
    const bool cello = isOn (pv[kCrCello]), violin = isOn (pv[kCrViolin]);
    const bool vibOn = isOn (pv[kCrVibOn]);
    const double vibHz = map::crVibSpeed (pv[kCrVibSp]);
    const double vibDepth = vibOn ? kVibSemis * pv[kCrVibDp] : 0.0;
    const bool bassOn = isOn (pv[kCrBassOn]);
    const double bassVol = pv[kCrBassV] * pv[kCrBassV];
    const double cut = std::log (map::crContour (pv[kCrCont])) / std::log (2.0);
    const double kRes = 5.0 * pv[kCrEmph];
    const bool anyBrass = brLo > 0.0 || brHi > 0.0;
    const double fs2 = 2.0 * sr;
    const double msToS = sr / 1000.0;
    const double dSlow = kTwoPi * kEnsSlowHz / sr, dFast = kTwoPi * kEnsFastHz / sr;
    double baseInc[12];
    for (int c = 0; c < 12; ++c) baseInc[c] = kTopHzC * semisToRatio (c) / sr;

    for (int i = 0; i < n; ++i)
    {
        // the tone generator, swept by the vibrato
        vibPh = wrap01 (vibPh + vibHz / sr);
        const double ratio = vibDepth > 0.0 ? semisToRatio (vibDepth * std::sin (kTwoPi * vibPh)) : 1.0;
        for (int c = 0; c < 12; ++c)
        {
            topCount[c] += baseInc[c] * ratio;
            if (topCount[c] >= 1024.0) topCount[c] -= 1024.0;
        }

        // the key buses, lower and upper halves
        double strBus[2] = { 0.0, 0.0 }, pnoBus[2] = { 0.0, 0.0 }, brBus[2] = { 0.0, 0.0 };
        for (int k = 0; k < kKeys; ++k)
        {
            const bool sAct = strEnv[k].active(), pAct = pnoEnv[k].active();
            brassGate[k] += ((down[k] ? 1.0 : 0.0) - brassGate[k]) * (down[k] ? gateUp : gateDown);
            const bool bAct = anyBrass && brassGate[k] > 1e-4;
            if (! sAct && ! pAct && ! bAct) { if (! down[k]) brassGate[k] = 0.0; continue; }
            const int note = kLowKey + k;
            const int pc = note % 12;
            const int div = (108 + pc - note) / 12;
            const double scale = 1.0 / (double) (1 << div);
            const double dt8 = baseInc[pc] * ratio * scale;
            const double ph8 = wrap01 (topCount[pc] * scale);
            const double ph4 = wrap01 (topCount[pc] * scale * 2.0);
            const double sq8 = pulseBL (ph8, dt8, 0.5), sq4 = pulseBL (ph4, dt8 * 2.0, 0.5);
            const int h = note >= kSplit ? 1 : 0;
            if (sAct) strBus[h] += strEnv[k].tick() * (sq8 + 0.6 * sq4);
            if (pAct) pnoBus[h] += pnoEnv[k].tick() * (sq8 + 0.5 * sq4);
            if (bAct) brBus[h] += brassGate[k] * sawBL (ph8, dt8);
        }

        // strings: cello and violin filters, then the ensemble
        double str = 0.0;
        const double sLo = strBus[0], sHi = strBus[1];
        if (cello) str += celloPk.tick (celloLp.tick (sLo * ceLo + sHi * ceHi)) * 0.9;
        if (violin) str += violinLp.tick (violinPk.tick (violinHp.tick (sLo * viLo + sHi * viHi))) * 0.8;
        ens.push (str);
        lfoSlow += dSlow; if (lfoSlow > kTwoPi) lfoSlow -= kTwoPi;
        lfoFast += dFast; if (lfoFast > kTwoPi) lfoFast -= kTwoPi;
        double strOut = 0.0;
        for (int t = 0; t < 3; ++t)
        {
            const double off = t * kTwoPi / 3.0;
            strOut += ens.read ((kEnsBaseMs + kEnsSlowMs * std::sin (lfoSlow + off) + kEnsFastMs * std::sin (lfoFast + off)) * msToS);
        }
        strOut = ensLp2.lp (ensLp1.lp (strOut / 3.0));

        // piano and clavichord
        const double pb = pnoBus[0] * pnLo + pnoBus[1] * pnHi;
        double pno = 0.0;
        if (piano) pno += pianoLp.tick (pb);
        if (clav) pno += clavPk.tick (clavHp.tick (pb)) * 1.2;

        // brass: the ladder, its contour retriggered by every key
        double brass = 0.0;
        {
            const double fe = brassEnv.tick();
            if (anyBrass || fe > 0.0)
            {
                const double x = softClip ((brBus[0] * brLo + brBus[1] * brHi) * 0.4) / 0.4;
                const double fc = clampd (std::pow (2.0, cut + kBrassEnvOct * fe), 20.0, 0.45 * fs2);
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
                brass = y4 * 0.5 * 1.6;
            }
        }

        // bass pedals
        double bass = 0.0;
        {
            const double tgt = (bassOn && bassNote >= 0) ? bassTarget : 0.0;
            bassLevel += (tgt - bassLevel) * bassK;
            if (bassLevel > 1e-5 && bassNote >= 0)
            {
                const double dt = noteHz (bassNote) / sr;
                bassPh = wrap01 (bassPh + dt);
                bass = bassLp2.lp (bassLp1.lp (pulseBL (bassPh, dt, 0.5))) * bassLevel * bassVol * 1.6;
            }
        }

        const double m = masterS.tick (pv[kCrMaster]);
        double y = (strOut * 0.15 + pno * 0.12 + brass * 0.16 + bass * 0.6) * (m / 0.7) * (m / 0.7);
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
