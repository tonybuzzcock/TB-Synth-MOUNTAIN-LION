// SK30.cpp
// Yamaha SK-30 Symphonic Ensemble (1980), from its owner's manual:
// - 61 keys. ORGAN, POLY-SYNTH and STRINGS share 7-note polyphony (the key
//   assigner's 7 channels); the SOLO SYNTHESIZER is single-note, high-note
//   priority, on the upper three octaves (NORMAL) or the lowest 19 keys
//   (MANUAL BASS).
// - All polyphonic tones come from one top-octave generator divided down
//   (YM622 TGC and dividers); MODE II adds a second generator offset by
//   DETUNE (+/- 59 cents) for the poly-synth and strings.
// - ORGAN: presets 1-3 or MANUAL with nine tone levers (16' ... 1'),
//   PERCUSSIVE 2nd / 3rd / 5th with DECAY TIME (0.8 s max), DECAY and SUSTAIN
//   switches sharing one lever (0.03 - 1.6 s), BRILLIANCE. Organ 1 clicks
//   like a Hammond.
// - POLY-SYNTH: presets 1-3 or MANUAL (WAVE saw / square, CUT OFF FREQ,
//   RESONANCE, EG DEPTH, ATTACK, DECAY); FEET 16' / 8' / 4'; SLOW ATTACK
//   (3 ms / 80 ms); SUSTAIN switch and lever; BRILLIANCE.
// - STRINGS: three presets, cello / viola / violin, each an octave higher.
// - VIBRATO for poly-synth and strings: DELAY 0 - 3.2 s, SPEED, DEPTH (30
//   cents max).
// - SOLO: VCO FEET 64' - 4', WAVE saw to square, PW 50 - 85 % (MANUAL / LFO /
//   EG), VCF CUT OFF, RESONANCE, MODULATION (LFO), EG DEPTH, VCA HOLD / GATE /
//   EG, ADSR (attack 3 ms - 3 s, decay and release 30 ms - 30 s), LFO 0.1 -
//   100 Hz, SOLO TUNE -500 .. +700 cents, PORTAMENTO, GLIDE DEPTH.
// - TREMOLO (internal, SLOW / FAST) and ENSEMBLE for organ and poly-synth;
//   ensemble wins when both are on.
// Filter types, preset voicings, envelope shapes not given in the manual,
// the ensemble and tremolo timings are estimates (see README).

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kTopHzC = 4186.009;
// tone-lever multiples of the 16' pitch: 16', 8', 5 1/3', 4', 2 2/3', 2', 1 3/5', 1 1/3', 1'
const double kLeverMul[9] = { 1.0, 2.0, 3.0, 4.0, 6.0, 8.0, 10.0, 12.0, 16.0 };
// organ presets (lever levels 0..1; estimates): 1 Hammond-like with percussion and click,
// 2 soft flutes, 3 full home organ
const float kOrgPreset[3][9] = {
    { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.75f, 0.9f, 0.0f, 0.55f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.6f, 1.0f, 0.3f, 0.8f, 0.35f, 0.6f, 0.0f, 0.25f, 0.4f } };
// poly-synth presets: square?, cutoff, resonance, EG depth, attack, decay (estimates):
// 1 brass, 2 clarinet, 3 bright synth brass
struct PolyVoice { bool square; float cut, res, egd, atk, dec; };
const PolyVoice kPolyPreset[3] = {
    { false, 0.36f, 0.15f, 0.65f, 0.45f, 0.6f },
    { true, 0.42f, 0.25f, 0.2f, 0.3f, 0.5f },
    { false, 0.42f, 0.35f, 0.5f, 0.25f, 0.42f } };
const double kStrAttack = 0.09;            // string keyer (estimate)
const double kVibCents = 30.0;             // full vibrato depth (spec)
const double kTremSlow = 0.8, kTremFast = 6.7, kTremAccel = 1.2;   // (estimates)
}

SK30Engine::SK30Engine()
    : sr (44100.0), pedal (false), orgSlotsHeld (0), clock (0), vibPh (0.0), vibAmt (0.0), vibRise (0.0),
      soNote (-1), soPitch (60.0), soTarget (60.0), soGlide (0.0), soPh (0.0), soLfoPh (0.0), soGate (false), soDrone (0.0),
      tremRate (kTremSlow), tremPh (0.0), ensPh1 (0.0), ensPh2 (0.0), lastOrgSel (-1)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    for (int i = 0; i < 12; ++i) { topOrg[i] = 0.07 * i; topPoly[i] = 0.11 * i; topPoly2[i] = 0.19 * i; }
    std::memset (down, 0, sizeof (down));
    std::memset (held, 0, sizeof (held));
    for (int v = 0; v < kVoices; ++v) { slotNote[v] = -1; slotKey[v] = false; slotAge[v] = 0; click[v] = 0.0; }
    noise.seed (2480);
}

void SK30Engine::prepare (double s)
{
    sr = s;
    for (int v = 0; v < kVoices; ++v) { orgEnv[v].setRate (sr); percEnv[v].setRate (sr); polyEnv[v].setRate (sr); strEnv[v].setRate (sr); fEnv[v].setRate (sr); }
    soEnv.setRate (sr);
    updateEnv();
    celloLp.lowpass (1400.0, 0.8, sr); celloPk.peak (300.0, 1.3, 4.0, sr);
    violaLp.lowpass (3500.0, 0.8, sr); violaPk.peak (900.0, 1.2, 3.5, sr);
    violinHp.highpass (300.0, 0.7, sr); violinPk.peak (2600.0, 1.1, 5.0, sr); violinLp.lowpass (8000.0, 0.7, sr);
    clickBp.bandpass (2500.0, 0.8, sr);
    updateBrill();
    orgLp.set (9000.0, sr); polyLp.set (9000.0, sr); strLp.set (9000.0, sr);
    dcBlock.set (12.0, sr);
    silence();
}

void SK30Engine::updateBrill()
{
    orgShelf.highShelf (2500.0, (pv[kSkOrgBril] - 0.5) * 18.0, sr);
}

void SK30Engine::updateEnv()
{
    const bool dec = isOn (pv[kSkOrgDec]), sus = isOn (pv[kSkOrgSus]);
    const double t = map::skDS (pv[kSkOrgDS]);
    const int sel = stepOf (pv[kSkOrgSel], 5);
    for (int v = 0; v < kVoices; ++v)
    {
        orgEnv[v].setAttack (sel == 1 ? 0.0008 : 0.003);
        orgEnv[v].setDecay (dec ? t : 0.01);
        orgEnv[v].setSustain (dec ? 0.0 : 1.0);
        orgEnv[v].setRelease (sus || dec ? t : 0.012);
        percEnv[v].setAttack (0.0005);
        percEnv[v].setDecay (map::skPercDec (pv[kSkPercDec]));
        percEnv[v].setSustain (0.0);
        percEnv[v].setRelease (map::skPercDec (pv[kSkPercDec]));
        const double rel = isOn (pv[kSkPSus]) ? map::skDS (pv[kSkPSusT]) : 0.03;
        polyEnv[v].setAttack (isOn (pv[kSkSlow]) ? 0.08 : 0.003);
        polyEnv[v].setDecay (0.01);
        polyEnv[v].setSustain (1.0);
        polyEnv[v].setRelease (rel);
        strEnv[v].setAttack (isOn (pv[kSkSlow]) ? 0.25 : kStrAttack);
        strEnv[v].setDecay (0.01);
        strEnv[v].setSustain (1.0);
        strEnv[v].setRelease (rel < 0.15 ? 0.15 : rel);
        const int ps = stepOf (pv[kSkPolySel], 5);
        const double a = ps >= 1 && ps <= 3 ? kPolyPreset[ps - 1].atk : pv[kSkEgA];
        const double d = ps >= 1 && ps <= 3 ? kPolyPreset[ps - 1].dec : pv[kSkEgD];
        fEnv[v].setAttack (map::skEgA (a));
        fEnv[v].setDecay (map::skEgD (d));
        fEnv[v].setSustain (0.0);
        fEnv[v].setRelease (map::skEgD (d));
    }
    soEnv.setAttack (map::skSoA (pv[kSkSoA]));
    soEnv.setDecay (map::skSoDR (pv[kSkSoD]));
    soEnv.setSustain (pv[kSkSoS]);
    soEnv.setRelease (map::skSoDR (pv[kSkSoR]));
}

void SK30Engine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kSkOrgBril) updateBrill();
    else if (kParams[p].inst == kSK30) updateEnv();
}

int SK30Engine::soloKey() const
{
    const bool bass = pv[kSkSoMode] >= 0.5f;
    const int lo = bass ? 0 : 24, hi = bass ? 18 : kKeys - 1;     // NORMAL: upper three octaves; BASS: lowest 19 keys
    for (int k = hi; k >= lo; --k) if (held[k]) return k;         // high-note priority
    return -1;
}

void SK30Engine::updateSolo (bool newKey)
{
    const int k = soloKey();
    if (k >= 0)
    {
        const int note = kLowKey + k;
        if (newKey || note != soNote)
        {
            const bool legato = soGate;
            soNote = note;
            soTarget = note;
            if (! legato || map::skPorta (pv[kSkSoPorta]) <= 0.0) soPitch = note;
            soGlide = map::skGlide (pv[kSkSoGlide]);
            soEnv.gate (true, true);
        }
        soGate = true;
    }
    else if (soGate)
    {
        soGate = false;
        soEnv.gate (false);
    }
}

int SK30Engine::findSlot (int note)
{
    for (int v = 0; v < kVoices; ++v) if (slotNote[v] == note) return v;
    int best = -1; unsigned int bestAge = 0xffffffffu;
    for (int v = 0; v < kVoices; ++v)                         // a silent channel
        if (! orgEnv[v].active() && ! polyEnv[v].active() && ! strEnv[v].active() && slotAge[v] < bestAge) { best = v; bestAge = slotAge[v]; }
    if (best >= 0) return best;
    bestAge = 0xffffffffu;
    for (int v = 0; v < kVoices; ++v)                         // the oldest released channel
        if (! slotKey[v] && slotAge[v] < bestAge) { best = v; bestAge = slotAge[v]; }
    if (best >= 0) return best;
    bestAge = 0xffffffffu;
    for (int v = 0; v < kVoices; ++v) if (slotAge[v] < bestAge) { best = v; bestAge = slotAge[v]; }   // the oldest note
    return best;
}

void SK30Engine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys) return;
    bool any = false;
    for (int j = 0; j < kKeys; ++j) if (held[j]) any = true;
    if (! any) { vibAmt = 0.0; vibRise = 0.0; }               // vibrato delay restarts with each phrase
    down[k] = true;
    held[k] = true;
    const int v = findSlot (note);
    if (slotNote[v] != note) { slotNote[v] = note; }
    slotKey[v] = true;
    slotAge[v] = ++clock;
    orgEnv[v].gate (true, true);
    percEnv[v].gate (true, true);
    polyEnv[v].gate (true, false);
    strEnv[v].gate (true, false);
    fEnv[v].gate (true, true);
    if (stepOf (pv[kSkOrgSel], 5) == 1) click[v] = 1.0;
    updateSolo (true);
}

void SK30Engine::releaseKey (int k)
{
    held[k] = false;
    const int note = kLowKey + k;
    for (int v = 0; v < kVoices; ++v)
        if (slotNote[v] == note && slotKey[v])
        {
            slotKey[v] = false;
            orgEnv[v].gate (false); percEnv[v].gate (false); polyEnv[v].gate (false); strEnv[v].gate (false); fEnv[v].gate (false);
        }
    updateSolo (false);
}

void SK30Engine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys || ! down[k]) return;
    down[k] = false;
    if (! pedal) releaseKey (k);
}

void SK30Engine::sustainPedal (bool d)
{
    pedal = d;
    if (! d) for (int k = 0; k < kKeys; ++k) if (held[k] && ! down[k]) releaseKey (k);
}

void SK30Engine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { down[k] = false; held[k] = false; }
    for (int v = 0; v < kVoices; ++v)
    {
        slotKey[v] = false;
        orgEnv[v].gate (false); percEnv[v].gate (false); polyEnv[v].gate (false); strEnv[v].gate (false); fEnv[v].gate (false);
    }
    updateSolo (false);
}

void SK30Engine::silence()
{
    allNotesOff();
    pedal = false;
    for (int v = 0; v < kVoices; ++v)
    {
        orgEnv[v].kill(); percEnv[v].kill(); polyEnv[v].kill(); strEnv[v].kill(); fEnv[v].kill();
        filt[v].reset(); slotNote[v] = -1; click[v] = 0.0;
    }
    soEnv.kill(); soFilt.reset(); soNote = -1; soGate = false; soDrone = 0.0;
    orgDel.clear(); polyDel.clear(); strDel.clear();
    celloLp.reset(); celloPk.reset(); violaLp.reset(); violaPk.reset(); violinHp.reset(); violinPk.reset(); violinLp.reset();
    clickBp.reset(); orgShelf.reset(); orgLp.reset(); polyLp.reset(); strLp.reset(); dcBlock.reset();
}

// one bus through ENSEMBLE (wins) or TREMOLO, or dry
double SK30Engine::effect (Delay<4096>& d, double x, bool ens, bool trem)
{
    d.push (x);
    const double ms = sr / 1000.0;
    if (ens)
    {
        double y = x * 0.5;
        for (int t = 0; t < 3; ++t)
        {
            const double off = t * kTwoPi / 3.0;
            y += 0.33 * d.read ((6.0 + 1.8 * std::sin (ensPh1 + off) + 0.25 * std::sin (ensPh2 + off)) * ms);
        }
        return y;
    }
    if (trem)
    {
        const double s = std::sin (tremPh);
        return d.read ((1.2 + 0.6 * s) * ms) * (0.8 + 0.2 * std::cos (tremPh));
    }
    return x;
}

void SK30Engine::render (float* out, int n)
{
    const int orgSel = stepOf (pv[kSkOrgSel], 5);
    const int strSel = stepOf (pv[kSkStrSel], 4);
    const int polySel = stepOf (pv[kSkPolySel], 5);
    if (orgSel != lastOrgSel) { lastOrgSel = orgSel; updateEnv(); }
    float lev[9];
    const float* src = orgSel >= 1 && orgSel <= 3 ? kOrgPreset[orgSel - 1] : 0;
    const int leverParam[9] = { kSkO16, kSkO8, kSkO513, kSkO4, kSkO223, kSkO2, kSkO135, kSkO113, kSkO1 };
    for (int i = 0; i < 9; ++i) lev[i] = orgSel == 0 ? 0.0f : (src ? src[i] : pv[leverParam[i]]);
    const bool orgOn = orgSel != 0;
    const double p2 = orgSel == 1 ? 0.0 : pv[kSkPerc2], p3 = orgSel == 1 ? 0.7 : pv[kSkPerc3], p5 = orgSel == 1 ? 0.0 : pv[kSkPerc5];
    const bool percOn = orgOn && (p2 > 0.0 || p3 > 0.0 || p5 > 0.0);
    const bool polyOn = polySel != 0, strOn = strSel != 0;
    const PolyVoice man = { pv[kSkWave] >= 0.5f, pv[kSkCut], pv[kSkRes], pv[kSkEgDp], pv[kSkEgA], pv[kSkEgD] };
    const PolyVoice& pvc = polySel >= 1 && polySel <= 3 ? kPolyPreset[polySel - 1] : man;
    const int feet = stepOf (pv[kSkFeet], 3);                 // 0 16', 1 8', 2 4'
    const double feetMul = feet == 0 ? 0.5 : (feet == 2 ? 2.0 : 1.0);
    const bool mode2 = pv[kSkMode] >= 0.5f;
    const double det = semisToRatio (map::skDetune (pv[kSkDetune]) / 100.0);
    const double cutOct = std::log (map::skCut (pvc.cut)) / std::log (2.0) + (pv[kSkPBril] - 0.5) * 3.0;
    const double kRes = 2.0 - 1.9 * pvc.res;
    const double egOct = 6.0 * pvc.egd;
    const double strMul = strSel == 1 ? 0.5 : (strSel == 3 ? 2.0 : 1.0);   // cello / viola / violin
    const double vibHz = map::skVibSpeed (pv[kSkVibSp]), vibDepth = kVibCents * pv[kSkVibDp] / 100.0;
    const double vibDelay = map::skVibDelay (pv[kSkVibDl]);
    const double vibStep = vibDelay > 0.01 ? 1.0 / (vibDelay * sr) : 1.0;
    // solo
    const int sf = stepOf (pv[kSkSoFeet], 6);
    const double soFeetSemis[6] = { -36.0, -24.0, -12.0, 0.0, 7.0, 12.0 };
    const double soOffset = soFeetSemis[sf] + map::skSoTune (pv[kSkSoTune]) / 100.0;
    const double soWave = pv[kSkSoWave];
    const double soPwBase = map::skSoPW (pv[kSkSoPW]);
    const int pwm = stepOf (pv[kSkSoPwm], 3), vca = stepOf (pv[kSkSoVca], 3);
    const double soCutOct = std::log (map::skSoCut (pv[kSkSoCut])) / std::log (2.0);
    const double soK = 2.0 - 1.97 * pv[kSkSoRes];
    const double soEgOct = 6.0 * pv[kSkSoEgDp], soModOct = 3.0 * pv[kSkSoMod];
    const double soLfoHz = map::skSoLfo (pv[kSkSoLfo]);
    const double porta = map::skPorta (pv[kSkSoPorta]);
    const double portaK = porta > 0.0 ? 1.0 - std::exp (-1.0 / (porta * sr)) : 1.0;
    const double glideK = 1.0 - std::exp (-1.0 / (0.12 * sr));
    const bool orgEns = isOn (pv[kSkOrgEns]), orgTrem = isOn (pv[kSkOrgTrem]);
    const bool polyEns = isOn (pv[kSkPEns]), polyTrem = isOn (pv[kSkPTrem]);
    const double tremTarget = pv[kSkTrFast] >= 0.5f ? kTremFast : kTremSlow;
    const double tremK = 1.0 - std::exp (-1.0 / (kTremAccel * sr));
    const double gOrg = pv[kSkOrgV] * pv[kSkOrgV], gPoly = pv[kSkPolyV] * pv[kSkPolyV];
    const double gStr = pv[kSkStrV] * pv[kSkStrV], gSolo = pv[kSkSoloV] * pv[kSkSoloV];
    const double fs = sr;
    const SineTable& sine = sineTable();
    double baseInc[12];
    for (int c = 0; c < 12; ++c) baseInc[c] = kTopHzC * semisToRatio (c) / sr;

    for (int i = 0; i < n; ++i)
    {
        // vibrato (poly-synth and strings only)
        bool anyHeld = false;
        for (int v = 0; v < kVoices; ++v) if (slotKey[v]) { anyHeld = true; break; }
        if (anyHeld) { vibRise += vibStep; if (vibRise > 1.0) vibRise = 1.0; }
        vibAmt = vibRise;
        vibPh = wrap01 (vibPh + vibHz / sr);
        const double vr = vibDepth > 0.0 ? semisToRatio (vibDepth * vibAmt * std::sin (kTwoPi * vibPh)) : 1.0;
        for (int c = 0; c < 12; ++c)
        {
            topOrg[c] += baseInc[c];               if (topOrg[c] >= 1024.0) topOrg[c] -= 1024.0;
            topPoly[c] += baseInc[c] * vr;         if (topPoly[c] >= 1024.0) topPoly[c] -= 1024.0;
            topPoly2[c] += baseInc[c] * vr * det;  if (topPoly2[c] >= 1024.0) topPoly2[c] -= 1024.0;
        }

        double orgBus = 0.0, clickBus = 0.0, polyBus = 0.0, strBus = 0.0;
        for (int v = 0; v < kVoices; ++v)
        {
            const int note = slotNote[v];
            if (note < 0) continue;
            const bool oA = orgEnv[v].active(), pA = polyEnv[v].active(), sA = strEnv[v].active();
            if (! oA && ! pA && ! sA) continue;
            const int pc = note % 12;
            const int div = (108 + pc - note) / 12;
            const double scale = 1.0 / (double) (1 << div);
            const double oe = orgEnv[v].tick(), pe = percEnv[v].tick(), ye = polyEnv[v].tick(), se = strEnv[v].tick(), fe = fEnv[v].tick();
            if (orgOn && oA)
            {
                const double ph16 = wrap01 (topOrg[pc] * scale * 0.5);
                double y = 0.0;
                for (int l = 0; l < 9; ++l) if (lev[l] > 0.0f) y += lev[l] * sine (wrap01 (ph16 * kLeverMul[l]));
                if (percOn) y += pe * (p2 * 1.6 * sine (wrap01 (ph16 * 4.0)) + p3 * 1.6 * sine (wrap01 (ph16 * 6.0)) + p5 * 1.6 * sine (wrap01 (ph16 * 10.0)));
                orgBus += oe * y;
                if (click[v] > 1e-4) { clickBus += click[v]; click[v] *= 0.992; }
            }
            if (polyOn && pA)
            {
                const double dt = baseInc[pc] * scale * feetMul;
                const double ph = wrap01 (topPoly[pc] * scale * feetMul);
                double x = pvc.square ? pulseBL (ph, dt, 0.5) : sawBL (ph, dt);
                if (mode2)
                {
                    const double ph2 = wrap01 (topPoly2[pc] * scale * feetMul);
                    x = 0.6 * x + 0.6 * (pvc.square ? pulseBL (ph2, dt * det, 0.5) : sawBL (ph2, dt * det));
                }
                const double keyOct = 0.5 * (note - 60) / 12.0;
                const double fc = clampd (std::pow (2.0, cutOct + egOct * fe + keyOct), 20.0, 0.45 * fs);
                filt[v].setGK (std::tan (kPi * fc / fs), kRes);
                filt[v].tick (x);
                polyBus += ye * filt[v].low * (pvc.square ? 0.6 : 1.0);
            }
            if (strOn && sA)
            {
                const double dt = baseInc[pc] * scale * strMul;
                double x = sawBL (wrap01 (topPoly[pc] * scale * strMul), dt);
                if (mode2) x = 0.6 * x + 0.6 * sawBL (wrap01 (topPoly2[pc] * scale * strMul), dt * det);
                strBus += se * x;
            }
        }

        // strings voicing
        double str = 0.0;
        if (strOn)
        {
            if (strSel == 1) str = celloPk.tick (celloLp.tick (strBus)) * 1.1;
            else if (strSel == 2) str = violaPk.tick (violaLp.tick (strBus)) * 0.9;
            else str = violinLp.tick (violinPk.tick (violinHp.tick (strBus))) * 0.75;
        }

        // the solo synthesizer
        double solo = 0.0;
        {
            soLfoPh = wrap01 (soLfoPh + soLfoHz / sr);
            const double lfo = 1.0 - 4.0 * std::fabs (soLfoPh - 0.5);
            const double env = soEnv.tick();
            soPitch += (soTarget - soPitch) * portaK;
            soGlide += (0.0 - soGlide) * glideK;
            double amp = 0.0;
            if (vca == 0) { if (soNote >= 0) soDrone = 1.0; amp = soDrone; }      // HOLD: a drone on the last note
            else if (vca == 1) amp = soGate ? 1.0 : 0.0;
            else amp = env;
            if (soNote >= 0 && (amp > 1e-5 || soGate))
            {
                const double hz = noteHz (soPitch + soOffset + soGlide);
                const double dt = clampd (hz / sr, 1e-6, 0.45);
                soPh = wrap01 (soPh + dt);
                double pw = soPwBase;
                if (pwm == 1) pw = clampd (soPwBase + 0.15 * lfo, 0.5, 0.92);
                else if (pwm == 2) pw = clampd (soPwBase + 0.3 * env, 0.5, 0.92);
                const double x = (1.0 - soWave) * sawBL (soPh, dt) + soWave * pulseBL (soPh, dt, 1.0 - pw);
                const double keyOct = 0.5 * (soPitch - 60.0) / 12.0;
                const double fc = clampd (std::pow (2.0, soCutOct + soEgOct * env + soModOct * lfo + keyOct), 20.0, 0.45 * fs);
                soFilt.setGK (std::tan (kPi * fc / fs), soK);
                soFilt.tick (x);
                solo = softClip (soFilt.low * (1.0 + 0.3 * pv[kSkSoRes])) * amp;
            }
        }

        // tremolo / ensemble
        tremRate += (tremTarget - tremRate) * tremK;
        tremPh += kTwoPi * tremRate / sr; if (tremPh > kTwoPi) tremPh -= kTwoPi;
        ensPh1 += kTwoPi * 0.62 / sr; if (ensPh1 > kTwoPi) ensPh1 -= kTwoPi;
        ensPh2 += kTwoPi * 5.9 / sr;  if (ensPh2 > kTwoPi) ensPh2 -= kTwoPi;
        double org = orgShelf.tick (orgBus) + clickBp.tick (clickBus * noise.white()) * 0.4;
        org = orgLp.lp (effect (orgDel, org, orgEns, orgTrem));
        const double poly = polyLp.lp (effect (polyDel, polyBus, polyEns, polyTrem));
        const double strOut = strLp.lp (effect (strDel, str, true, false));           // the strings' own ensemble

        double y = org * 0.075 * gOrg + poly * 0.28 * gPoly + strOut * 0.17 * gStr + solo * 0.6 * gSolo;
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
