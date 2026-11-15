// Clav.cpp
// Hohner Clavinet D6: 60 keys (F1 - E6). Each key's tangent strikes its
// string against an anvil and holds it there, so the string rings between the
// tangent and the bridge for as long as the key is down; letting go, the
// yarn winding damps it. The strings are modelled as waveguides (a delay loop
// with a gently losing low-pass), the strike as a narrow, velocity-bright
// shape at the tangent. Two electromagnetic pickups under the strings at
// different distances from the end:
//   C-A  individual, the "rhythm" (neck) pickup
//   C-B  individual, the bridge pickup (punchier, more treble)
//   D-A  both, in phase
//   D-B  both, out of phase (thin: they partly cancel)
// Then the four tone switches of the preamp, each switching in its own filter
// so they stack (BRILLIANT and TREBLE high-pass, MEDIUM and SOFT low-pass);
// with none selected the D6 is silent, and so is this. The MUTE slider
// presses felt against the strings: shorter, darker notes.
// Pickup positions, filter corners, decays and the mute's effect are estimates
// (no component values were found for the tone switches; see README).

#include "Engines.h"

#include <algorithm>

namespace tbsyn
{

namespace
{
const double kNeckPos = 0.17, kBridgePos = 0.07;     // pickup distances, fraction of the string (estimates)
double perPass (double tau, double len, double sr) { return std::exp (-len / (tau * sr)); }
}

ClavEngine::ClavEngine() : sr (44100.0), clock (0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    noise.seed (31337u);
    for (int i = 0; i < kVoices; ++i)
    {
        std::memset (&s[i], 0, sizeof (Str));
        s[i].note = -1;
    }
}

void ClavEngine::prepare (double r)
{
    sr = r;
    brill.set (2000.0, sr);
    treb.set (650.0, sr);
    med.set (2200.0, sr);
    soft.set (600.0, sr);
    dcBlock.set (15.0, sr);
    volS.setTime (0.01, sr); volS.v = pv[kClVol];
    silence();
}

void ClavEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if (p == kClMute) for (int i = 0; i < kVoices; ++i) if (s[i].active) setLoop (s[i]);
}

void ClavEngine::setLoop (Str& st)
{
    const double mute = pv[kClMute];
    const double tauHeld = 3.2 * std::pow (0.5, (st.note - 29) / 20.0) * (1.0 - 0.93 * mute);
    st.gainHeld = perPass (tauHeld, st.len, sr);
    st.gainReleased = perPass (0.03, st.len, sr);
    st.gain = st.keyDown ? st.gainHeld : st.gainReleased;
}

void ClavEngine::noteOn (int note, int vel)
{
    if (note < kLowKey || note > kHighKey) return;
    ++clock;
    int idx = -1;
    for (int i = 0; i < kVoices; ++i) if (s[i].active && s[i].note == note) { idx = i; break; }
    for (int i = 0; i < kVoices && idx < 0; ++i) if (! s[i].active) idx = i;
    if (idx < 0)
    {
        unsigned oldest = 0xffffffffu;
        for (int i = 0; i < kVoices; ++i) if (s[i].age < oldest) { oldest = s[i].age; idx = i; }
    }
    Str& st = s[idx];
    const double vn = clampd (vel / 127.0, 0.05, 1.0);
    const double mute = pv[kClMute];
    st.note = note; st.keyDown = true; st.active = true; st.age = clock; st.idle = 0; st.envOut = 1.0;
    // loop low-pass: brighter for harder strikes, darker with the mute
    st.bright = clampd (0.08 + 0.25 * (1.0 - vn) + 0.5 * mute + 0.004 * (88 - note), 0.02, 0.85);
    const double c = st.bright;
    st.len = clampd (sr / noteHz (note) - c / (1.0 - c), 4.0, (double) kLen - 8.0);
    setLoop (st);
    st.lpState = 0.0;

    // the strike: a narrow shape near the tangent end, smoothed more for soft notes
    const int L = (int) st.len;
    const double peak = 0.12 * L + 1.0;
    const double amp = 0.25 + 0.75 * vn;
    const double sm = 0.15 + 0.7 * (1.0 - vn);
    double lp = 0.0, mean = 0.0;
    std::memset (st.buf, 0, sizeof (st.buf));
    st.w = 0;
    float* tmp = st.buf;     // the loop's history; the newest L samples are the string
    for (int j = 0; j < L; ++j)
    {
        const double tri = j < peak ? j / peak : (L - j) / (L - peak);
        lp += ((tri + 0.08 * noise.white()) - lp) * (1.0 - sm);
        tmp[j] = (float) (lp * amp);
        mean += lp * amp;
    }
    mean /= L;
    for (int j = 0; j < L; ++j) tmp[j] -= (float) mean;
    st.w = L;
}

void ClavEngine::noteOff (int note)
{
    for (int i = 0; i < kVoices; ++i)
        if (s[i].active && s[i].note == note && s[i].keyDown) s[i].keyDown = false;
}

void ClavEngine::allNotesOff()
{
    for (int i = 0; i < kVoices; ++i) if (s[i].active) s[i].keyDown = false;
}

void ClavEngine::silence()
{
    for (int i = 0; i < kVoices; ++i) { s[i].active = false; s[i].keyDown = false; s[i].note = -1; }
    brill.reset(); treb.reset(); med.reset(); soft.reset(); dcBlock.reset();
}

void ClavEngine::render (float* out, int n)
{
    const bool b = isOn (pv[kClBrill]), t = isOn (pv[kClTreb]), m = isOn (pv[kClMed]), so = isOn (pv[kClSoft]);
    const bool both = paramStep (kClCD, pv[kClCD]) == 1;
    const bool optB = paramStep (kClAB, pv[kClAB]) == 1;
    const bool anyTone = b || t || m || so;
    const int mask = kLen - 1;
    const double gainK = 1.0 - std::exp (-1.0 / (0.002 * sr));

    for (int i = 0; i < n; ++i)
    {
        double x = 0.0;
        for (int k = 0; k < kVoices; ++k)
        {
            Str& st = s[k];
            if (! st.active) continue;
            // read the string one loop ago (linear interpolation)
            const double pos = (double) st.w - st.len;
            const int i0 = (int) std::floor (pos);
            const double f = pos - i0;
            const double y = st.buf[i0 & mask] * (1.0 - f) + st.buf[(i0 + 1) & mask] * f;
            st.lpState = (1.0 - st.bright) * y + st.bright * st.lpState;
            // the yarn takes a couple of milliseconds to settle on the string
            st.gain += ((st.keyDown ? st.gainHeld : st.gainReleased) - st.gain) * gainK;
            st.buf[st.w & mask] = (float) (st.lpState * st.gain);
            // the pickups: the string's motion seen at two points along it
            const int dN = (int) (2.0 * kNeckPos * st.len), dB = (int) (2.0 * kBridgePos * st.len);
            const double cur = st.buf[st.w & mask];
            const double neck = cur - st.buf[(st.w - dN) & mask];
            const double bridge = (cur - st.buf[(st.w - dB) & mask]) * 1.8;
            st.w = (st.w + 1) & mask;
            double p;
            if (! both) p = optB ? bridge : neck;
            else p = optB ? neck - bridge : neck + bridge;
            x += p;
            st.envOut = std::max (std::fabs (y), st.envOut * 0.9995);
            if (! st.keyDown && st.envOut < 1e-5) st.active = false;
        }
        // preamp tone switches (stacking filters); none = no sound, as on the D6
        double y = 0.0;
        if (anyTone)
        {
            y = x;
            if (b) y = brill.hp (y) * 3.0;
            if (t) y = treb.hp (y) * 1.6;
            if (m) y = med.lp (y) * 1.2;
            if (so) y = soft.lp (y) * 1.6;
        }
        const double vol = volS.tick (pv[kClVol]);
        y *= (vol / 0.7) * (vol / 0.7) * 0.32;
        y = 0.95 * softClip (y / 0.95);
        out[i] = (float) dcBlock.hp (y);
    }
}

} // namespace tbsyn
