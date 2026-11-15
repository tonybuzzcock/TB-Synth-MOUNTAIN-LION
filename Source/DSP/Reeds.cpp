// Reeds.cpp
// The two reed instruments.
//
// Wurlitzer 200A: 64 keys (A1 - C7), a felt hammer strikes a tuned steel reed;
// the reed bar is a charged electrostatic pickup, so the signal follows the
// change of capacitance between reed and comb, C ~ 1 / (gap - x): soft notes
// are nearly sinusoidal and hard ones bark (strong even harmonics), the
// Wurlitzer sound. The pickup works into a high resistance, so lows are rolled
// off (a first-order high-pass). Then the preamp, the 200A's VIBRATO (really a
// tremolo: an oscillator modulating the preamp's gain; only depth is on the
// panel), the power amp and the two small speakers. Each reed has its first
// two bending modes (cantilever ratios 1 : 6.27), harder blows bring in more of
// the second; dampers stop the reed when the key (and sustain pedal) is up.
//
// Hohner Cembalet: 61 keys (C2 - C7), a plectrum lifts the reed and lets it go
// at its release point, so the attack hardly depends on how hard the key is
// played; a bright, harpsichord-like start with a third mode (1 : 6.27 :
// 17.55), a capacitive pickup beside the reed, and a VOLUME control.
//
// Decay times, pickup curves and voicing are estimates (see README).

#include "Engines.h"

namespace tbsyn
{

namespace
{
const double kMode2 = 6.267, kMode3 = 17.55;     // cantilever beam overtone ratios
const double kTremHz = 5.6;                      // 200A vibrato rate (estimate)
const double kTremDepth = 0.75;                  // gain dip at full VIBRATO (estimate)

double perSample (double tau, double sr) { return tau <= 0.0 ? 0.0 : std::exp (-1.0 / (tau * sr)); }

int pickVoice (Reed* r, int count, int note, unsigned& clock)
{
    ++clock;
    for (int i = 0; i < count; ++i) if (r[i].active && r[i].note == note) return i;   // same reed again
    for (int i = 0; i < count; ++i) if (! r[i].active) return i;
    int idx = 0;
    unsigned oldest = 0xffffffffu;
    for (int i = 0; i < count; ++i) if (! r[i].keyDown && r[i].age < oldest) { oldest = r[i].age; idx = i; }
    if (oldest == 0xffffffffu)
        for (int i = 0; i < count; ++i) if (r[i].age < oldest) { oldest = r[i].age; idx = i; }
    return idx;
}

void clearReeds (Reed* r, int count)
{
    for (int i = 0; i < count; ++i)
    {
        std::memset (&r[i], 0, sizeof (Reed));
        r[i].note = -1;
    }
}
}

//==============================================================================
WurliEngine::WurliEngine() : sr (44100.0), clock (0), pedal (false), tremPh (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    clearReeds (r, kVoices);
}

void WurliEngine::prepare (double s)
{
    sr = s;
    pre.highpass (300.0, 0.6, sr);       // pickup into the preamp's high input resistance
    spkHp.highpass (110.0, 0.8, sr);
    spkPk.peak (1500.0, 0.9, 3.5, sr);
    spkLp.lowpass (6000.0, 0.7, sr);
    dcBlock.set (12.0, sr);
    volS.setTime (0.01, sr); volS.v = pv[kWuVol];
    vibS.setTime (0.02, sr); vibS.v = pv[kWuVib];
    silence();
}

void WurliEngine::setParam (int p, float v) { if (p >= 0 && p < kNumParams) pv[p] = v; }

void WurliEngine::noteOn (int note, int vel)
{
    if (note < kLowKey || note > kHighKey) return;
    const int i = pickVoice (r, kVoices, note, clock);
    Reed& d = r[i];
    const double vn = clampd (vel / 127.0, 0.0, 1.0);
    const double f = noteHz (note);
    const double hit = 0.10 + 0.62 * std::pow (vn, 1.4);         // reed swing / gap
    const double tau1 = 4.5 * std::pow (0.5, (note - 33) / 16.0);
    const bool again = d.active && d.note == note;
    d.note = note; d.keyDown = true; d.damped = false; d.active = true; d.age = clock;
    d.dph1 = f / sr; d.dph2 = f * kMode2 / sr; d.dph3 = 0.0;
    if (! again) { d.ph1 = 0.0; d.ph2 = 0.0; d.lastC = 0.0; d.a1 = 0.0; d.a2 = 0.0; }
    d.a1 = hit;
    d.a2 = hit * (0.04 + 0.16 * vn);
    d.a3 = 0.0;
    d.k1 = perSample (tau1, sr);
    d.k2 = perSample (tau1 / 12.0, sr);
    d.k3 = 0.0;
    d.kDamp1 = perSample (note < 60 ? 0.11 : 0.06, sr);
    d.kDamp2 = perSample (0.02, sr);
    d.attack = 0.0;
    d.attackK = 1.0 - std::exp (-1.0 / (0.0006 * sr));
}

void WurliEngine::noteOff (int note)
{
    for (int i = 0; i < kVoices; ++i)
        if (r[i].active && r[i].note == note && r[i].keyDown)
        {
            r[i].keyDown = false;
            if (! pedal) r[i].damped = true;
        }
}

void WurliEngine::sustainPedal (bool d)
{
    pedal = d;
    if (! d) for (int i = 0; i < kVoices; ++i) if (r[i].active && ! r[i].keyDown) r[i].damped = true;
}

void WurliEngine::allNotesOff()
{
    pedal = false;
    for (int i = 0; i < kVoices; ++i) { r[i].keyDown = false; r[i].damped = true; }
}

void WurliEngine::silence()
{
    clearReeds (r, kVoices);
    pedal = false;
    pre.reset(); spkHp.reset(); spkPk.reset(); spkLp.reset(); dcBlock.reset();
}

void WurliEngine::render (float* out, int n)
{
    const double dTrem = kTremHz / sr;
    for (int i = 0; i < n; ++i)
    {
        double x = 0.0;
        for (int k = 0; k < kVoices; ++k)
        {
            Reed& d = r[k];
            if (! d.active) continue;
            d.attack += (1.0 - d.attack) * d.attackK;
            const double u = d.attack * (d.a1 * std::sin (kTwoPi * d.ph1) + d.a2 * std::sin (kTwoPi * d.ph2));
            d.ph1 = wrap01 (d.ph1 + d.dph1);
            d.ph2 = wrap01 (d.ph2 + d.dph2);
            d.a1 *= d.damped ? d.kDamp1 : d.k1;
            d.a2 *= d.damped ? d.kDamp2 : d.k2;
            // electrostatic pickup: capacitance against the gap
            const double uc = clampd (u, -0.9, 0.88);
            x += uc / (1.0 - uc);
            if (d.a1 < 2e-5) d.active = false;
        }
        // preamp (gentle transistor rounding), then the vibrato on its gain
        double y = pre.tick (x);
        y = softClip (y * 1.4) / 1.4;
        tremPh = wrap01 (tremPh + dTrem);
        const double depth = vibS.tick (pv[kWuVib]);
        y *= 1.0 - depth * kTremDepth * (0.5 + 0.5 * std::sin (kTwoPi * tremPh));
        // power amp and speakers
        const double vol = volS.tick (pv[kWuVol]);
        y *= (vol / 0.7) * (vol / 0.7) * 0.4;
        y = 0.9 * softClip (y / 0.9);
        y = spkLp.tick (spkPk.tick (spkHp.tick (y)));
        out[i] = (float) dcBlock.hp (y);
    }
}

//==============================================================================
CembaletEngine::CembaletEngine() : sr (44100.0), clock (0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    clearReeds (r, kVoices);
}

void CembaletEngine::prepare (double s)
{
    sr = s;
    hp.highpass (180.0, 0.6, sr);
    lp.lowpass (7500.0, 0.7, sr);
    body.peak (2200.0, 1.0, 3.0, sr);
    dcBlock.set (12.0, sr);
    volS.setTime (0.01, sr); volS.v = pv[kCbVol];
    silence();
}

void CembaletEngine::setParam (int p, float v) { if (p >= 0 && p < kNumParams) pv[p] = v; }

void CembaletEngine::noteOn (int note, int vel)
{
    if (note < kLowKey || note > kHighKey) return;
    const int i = pickVoice (r, kVoices, note, clock);
    Reed& d = r[i];
    const double vn = clampd (vel / 127.0, 0.0, 1.0);
    const double f = noteHz (note);
    const double pluck = 0.32 + 0.12 * vn;      // the plectrum lets go at about the same point every time
    const double tau1 = 2.4 * std::pow (0.5, (note - 36) / 14.0);
    d.note = note; d.keyDown = true; d.damped = false; d.active = true; d.age = clock;
    d.ph1 = 0.25; d.ph2 = 0.25; d.ph3 = 0.25;           // released from a bent position
    d.dph1 = f / sr; d.dph2 = f * kMode2 / sr; d.dph3 = f * kMode3 / sr;
    d.a1 = pluck; d.a2 = pluck * 0.22; d.a3 = (f * kMode3 < 0.45 * sr) ? pluck * 0.07 : 0.0;
    d.k1 = perSample (tau1, sr);
    d.k2 = perSample (tau1 / 6.0, sr);
    d.k3 = perSample (tau1 / 15.0, sr);
    d.kDamp1 = perSample (0.05, sr);
    d.kDamp2 = perSample (0.015, sr);
    d.attack = 0.0;
    d.attackK = 1.0 - std::exp (-1.0 / (0.00015 * sr));
    d.lastC = 0.0;
}

void CembaletEngine::noteOff (int note)
{
    for (int i = 0; i < kVoices; ++i)
        if (r[i].active && r[i].note == note && r[i].keyDown) { r[i].keyDown = false; r[i].damped = true; }
}

void CembaletEngine::allNotesOff()
{
    for (int i = 0; i < kVoices; ++i) { r[i].keyDown = false; r[i].damped = true; }
}

void CembaletEngine::silence()
{
    clearReeds (r, kVoices);
    hp.reset(); lp.reset(); body.reset(); dcBlock.reset();
}

void CembaletEngine::render (float* out, int n)
{
    for (int i = 0; i < n; ++i)
    {
        double x = 0.0;
        for (int k = 0; k < kVoices; ++k)
        {
            Reed& d = r[k];
            if (! d.active) continue;
            d.attack += (1.0 - d.attack) * d.attackK;
            const double u = d.attack * (d.a1 * std::sin (kTwoPi * d.ph1) + d.a2 * std::sin (kTwoPi * d.ph2)
                                         + d.a3 * std::sin (kTwoPi * d.ph3));
            d.ph1 = wrap01 (d.ph1 + d.dph1);
            d.ph2 = wrap01 (d.ph2 + d.dph2);
            d.ph3 = wrap01 (d.ph3 + d.dph3);
            d.a1 *= d.damped ? d.kDamp1 : d.k1;
            d.a2 *= d.damped ? d.kDamp2 : d.k2;
            d.a3 *= d.damped ? d.kDamp2 : d.k3;
            // pickup plate beside the reed: capacitance rises either side of rest, more on one
            x += u + 0.6 * u * std::fabs (u);
            if (d.a1 < 2e-5) d.active = false;
        }
        const double vol = volS.tick (pv[kCbVol]);
        double y = body.tick (lp.tick (hp.tick (x)));
        y *= (vol / 0.7) * (vol / 0.7) * 0.45;
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
