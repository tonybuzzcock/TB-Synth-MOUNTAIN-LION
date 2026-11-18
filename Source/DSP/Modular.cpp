// Modular.cpp
// Moog Modular, after the 3P / IIIP systems ("I Feel Love" was made on one),
// as a fixed patch so it plays without cables:
//   three 921 oscillators (RANGE 32' - 2', SINE / TRIANGLE / SAWTOOTH /
//   PULSE, pulse width, oscillators 2 and 3 tuned against 1)
//   -> mixer (three oscillators and noise)
//   -> 904A 24 dB ladder low-pass (FREQUENCY, REGENERATION), swept by a 911
//      envelope (CONTOUR) and the keyboard
//   -> 902 VCA under a second 911 envelope.
// An LFO can move the oscillators (vibrato) and the filter. GLIDE as on the
// 950 keyboard's portamento. MONO plays like the real one (lowest-note
// priority, one voice); POLY gives every key its own patch so the string pads
// that were built up track by track on the record play as chords. DRIFT
// lets each oscillator wander a little, as the real oscillators did.
// Ranges and the envelope and filter law are estimates (see README).

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kRangeSemis[5] = { -24.0, -12.0, 0.0, 12.0, 24.0 };   // 32' .. 2' (8' = written pitch)
const double kRegenK = 4.2;           // regeneration at full: just into self-oscillation (estimate)
const double kContourOct = 6.0;       // full CONTOUR sweep (estimate)
const double kVibSemis = 1.0;         // full LFO to pitch (estimate)
const double kLfoFltOct = 3.0;        // full LFO to filter (estimate)
const double kDriftCents = 18.0;      // full DRIFT: how far an oscillator wanders (estimate)
}

ModularEngine::ModularEngine()
    : sr (44100.0), clock (0), lfoPh (0.0), bend (0.0), wheel (0.0), monoLast (-1)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    std::memset (held, 0, sizeof (held));
    for (int i = 0; i < kVoices; ++i)
    {
        Voice& v = vo[i];
        v.note = -1; v.gate = false; v.age = 0; v.pitch = v.target = 60.0;
        for (int o = 0; o < 3; ++o)
        {
            // each oscillator its own slight offset and slow wander (fixed pseudo-random values)
            const int seed = i * 3 + o;
            v.ph[o] = 0.31 * o + 0.07 * i;
            v.doff[o] = std::sin (seed * 12.9898) * 0.5 + std::sin (seed * 4.1414) * 0.5;
            v.dph[o] = wrap01 (seed * 0.618);
            v.drate[o] = 0.05 + 0.15 * wrap01 (seed * 0.377);
        }
        for (int s = 0; s < 4; ++s) v.lad[s] = 0.0;
    }
    noise.seed (1977);
}

void ModularEngine::prepare (double s)
{
    sr = s;
    for (int i = 0; i < kVoices; ++i) { vo[i].fenv.setRate (sr); vo[i].lenv.setRate (sr); }
    updateEnv();
    dcBlock.set (12.0, sr);
    silence();
}

void ModularEngine::updateEnv()
{
    for (int i = 0; i < kVoices; ++i)
    {
        Voice& v = vo[i];
        v.fenv.setAttack (map::mdTime (pv[kMdFA])); v.fenv.setDecay (map::mdTime (pv[kMdFD]));
        v.fenv.setSustain (pv[kMdFS]); v.fenv.setRelease (map::mdTime (pv[kMdFR]));
        v.lenv.setAttack (map::mdTime (pv[kMdA])); v.lenv.setDecay (map::mdTime (pv[kMdD]));
        v.lenv.setSustain (pv[kMdS]); v.lenv.setRelease (map::mdTime (pv[kMdR]));
    }
}

void ModularEngine::setParam (int p, float val)
{
    if (p < 0 || p >= kNumParams) return;
    const bool wasPoly = isOn (pv[kMdPoly]);
    pv[p] = val;
    if (p == kMdFA || p == kMdFD || p == kMdFS || p == kMdFR || p == kMdA || p == kMdD || p == kMdS || p == kMdR) updateEnv();
    if (p == kMdPoly && wasPoly != isOn (pv[kMdPoly])) allNotesOff();
}

int ModularEngine::lowestHeld() const
{
    for (int n = 0; n < 128; ++n) if (held[n]) return n;
    return -1;
}

void ModularEngine::noteOn (int note, int)
{
    if (note < 0 || note > 127) return;
    held[note] = true;
    if (! isOn (pv[kMdPoly]))
    {
        // MONO: one voice, the lowest key held sounds; each new key retriggers
        const int low = lowestHeld();
        Voice& v = vo[0];
        const bool wasOn = v.gate;
        v.note = low;
        v.target = low;
        if (! wasOn && map::mdGlide (pv[kMdGlide]) <= 0.0) v.pitch = low;
        v.gate = true;
        v.fenv.gate (true, true);
        v.lenv.gate (true, true);
        monoLast = low;
        return;
    }
    // POLY: the same key again, else a free voice, else the oldest
    int best = -1;
    for (int i = 0; i < kVoices; ++i) if (vo[i].note == note) best = i;
    if (best < 0)
    {
        unsigned oldest = 0xffffffffu;
        for (int i = 0; i < kVoices; ++i)
            if (! vo[i].lenv.active() && vo[i].age < oldest) { best = i; oldest = vo[i].age; }
    }
    if (best < 0)
    {
        unsigned oldest = 0xffffffffu;
        for (int i = 0; i < kVoices; ++i)
            if (! vo[i].gate && vo[i].age < oldest) { best = i; oldest = vo[i].age; }
    }
    if (best < 0)
    {
        unsigned oldest = 0xffffffffu;
        for (int i = 0; i < kVoices; ++i) if (vo[i].age < oldest) { best = i; oldest = vo[i].age; }
    }
    Voice& v = vo[best];
    const bool fresh = ! v.lenv.active();
    v.note = note;
    v.target = note;
    (void) fresh;
    v.pitch = map::mdGlide (pv[kMdGlide]) > 0.0 && monoLast >= 0 ? monoLast : note;   // glide from the last note played
    v.gate = true;
    v.age = ++clock;
    v.fenv.gate (true, true);
    v.lenv.gate (true, true);
    monoLast = note;
}

void ModularEngine::noteOff (int note)
{
    if (note < 0 || note > 127 || ! held[note]) return;
    held[note] = false;
    if (! isOn (pv[kMdPoly]))
    {
        Voice& v = vo[0];
        const int low = lowestHeld();
        if (low >= 0) { v.note = low; v.target = low; }      // fall back to the next key held, no retrigger
        else { v.gate = false; v.fenv.gate (false); v.lenv.gate (false); }
        return;
    }
    for (int i = 0; i < kVoices; ++i)
        if (vo[i].note == note && vo[i].gate) { vo[i].gate = false; vo[i].fenv.gate (false); vo[i].lenv.gate (false); }
}

void ModularEngine::allNotesOff()
{
    std::memset (held, 0, sizeof (held));
    for (int i = 0; i < kVoices; ++i) { vo[i].gate = false; vo[i].fenv.gate (false); vo[i].lenv.gate (false); }
}

void ModularEngine::silence()
{
    allNotesOff();
    for (int i = 0; i < kVoices; ++i)
    {
        vo[i].fenv.kill(); vo[i].lenv.kill(); vo[i].note = -1;
        for (int s = 0; s < 4; ++s) vo[i].lad[s] = 0.0;
    }
    monoLast = -1;
    dcBlock.reset();
}

double ModularEngine::wave (int w, double ph, double dt, double pw) const
{
    switch (w)
    {
        case 0: return std::sin (kTwoPi * ph);
        case 1: return 1.0 - 4.0 * std::fabs (ph - 0.5);
        case 2: return sawBL (ph, dt);
        default: return pulseBL (ph, dt, pw);
    }
}

void ModularEngine::render (float* out, int n)
{
    const bool poly = isOn (pv[kMdPoly]);
    const int nv = poly ? kVoices : 1;
    const int rng[3] = { stepOf (pv[kMdO1Rng], 5), stepOf (pv[kMdO2Rng], 5), stepOf (pv[kMdO3Rng], 5) };
    const int wav[3] = { stepOf (pv[kMdO1Wave], 4), stepOf (pv[kMdO2Wave], 4), stepOf (pv[kMdO3Wave], 4) };
    const double tun[3] = { 0.0, map::mdTune (pv[kMdO2Tune]), map::mdTune (pv[kMdO3Tune]) };
    const double lvl[3] = { pv[kMdO1Lvl] * pv[kMdO1Lvl], pv[kMdO2Lvl] * pv[kMdO2Lvl], pv[kMdO3Lvl] * pv[kMdO3Lvl] };
    const double gn = pv[kMdNoise] * pv[kMdNoise];
    const double pw = map::mdPW (pv[kMdPW]);
    const double cutOct = std::log (map::mdCutoff (pv[kMdCut])) / std::log (2.0);
    const double k = kRegenK * pv[kMdRegen];
    const double contour = kContourOct * pv[kMdContour];
    const double track = pv[kMdKbd];
    const double lfoHz = map::mdLfo (pv[kMdLfo]);
    const double vib = kVibSemis * (pv[kMdVib] + wheel * 0.5);
    const double lfoFlt = kLfoFltOct * pv[kMdLfoFlt];
    const double glide = map::mdGlide (pv[kMdGlide]);
    const double glideK = glide > 0.0 ? 1.0 - std::exp (-1.0 / (glide * sr)) : 1.0;
    const double driftAmt = kDriftCents / 100.0 * pv[kMdDrift];
    const double vol = pv[kMdVol] / 0.7;
    const double gain = vol * vol * (poly ? 0.32 : 0.55);
    const double fs2 = 2.0 * sr;

    for (int i = 0; i < n; ++i)
    {
        lfoPh = wrap01 (lfoPh + lfoHz / sr);
        const double lfo = std::sin (kTwoPi * lfoPh);
        const double nz = noise.white() * 0.5;
        double sum = 0.0;
        for (int j = 0; j < nv; ++j)
        {
            Voice& v = vo[j];
            if (! v.lenv.active() || v.note < 0) continue;
            v.pitch += (v.target - v.pitch) * glideK;
            const double base = v.pitch + bend * 2.0 + vib * lfo;
            double x = 0.0;
            for (int o = 0; o < 3; ++o)
            {
                if (lvl[o] <= 0.0) continue;
                // drift: a fixed offset and a slow wander per oscillator
                v.dph[o] = wrap01 (v.dph[o] + v.drate[o] / sr);
                const double drift = driftAmt * (0.6 * v.doff[o] + 0.4 * std::sin (kTwoPi * v.dph[o]));
                const double semis = base + kRangeSemis[rng[o]] + tun[o] + drift;
                const double dt = clampd (noteHz (semis) / sr, 1e-6, 0.45);
                x += lvl[o] * wave (wav[o], v.ph[o], dt, pw);
                v.ph[o] = wrap01 (v.ph[o] + dt);
            }
            x += gn * nz;
            x = softClip (x * 0.55) / 0.55;
            const double fe = v.fenv.tick();
            const double oct = cutOct + contour * fe + track * (v.pitch - 60.0) / 12.0 + lfoFlt * lfo;
            const double fc = clampd (std::pow (2.0, oct), 10.0, 0.45 * fs2);
            const double w = std::tan (kPi * fc / fs2);
            const double gg = w / (1.0 + w), beta = 1.0 - gg;
            const double G = gg * gg * gg * gg;
            double y4 = 0.0;
            for (int os = 0; os < 2; ++os)
            {
                const double S = gg * gg * gg * beta * v.lad[0] + gg * gg * beta * v.lad[1] + gg * beta * v.lad[2] + beta * v.lad[3];
                double in = softClip ((x - k * S) / (1.0 + k * G));
                for (int st = 0; st < 4; ++st) { const double u = (in - v.lad[st]) * gg; const double y = u + v.lad[st]; v.lad[st] = y + u; in = y; }
                y4 += in;
            }
            sum += y4 * 0.5 * (1.0 + 0.4 * k / kRegenK) * v.lenv.tick();
        }
        double y = sum * gain;
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
