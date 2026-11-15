// Common.h
// Small building blocks shared by the instrument models: band-limited
// oscillator corrections, analog-style envelopes, topology-preserving filters,
// delay lines, noise. Plain C++98, header only, no allocation.

#ifndef TBSYNTH_COMMON_H
#define TBSYNTH_COMMON_H

#include <cmath>
#include <cstring>

namespace tbsyn
{

const double kPi = 3.14159265358979323846;
const double kTwoPi = 6.28318530717958647692;

inline double clampd (double x, double lo, double hi) { return x < lo ? lo : (x > hi ? hi : x); }
inline double noteHz (double note) { return 440.0 * std::pow (2.0, (note - 69.0) / 12.0); }
inline double semisToRatio (double s) { return std::pow (2.0, s / 12.0); }

// Rational tanh: close to tanh, cheap, monotonic, bounded to +/- 1
inline double softClip (double x)
{
    if (x > 3.0) return 1.0;
    if (x < -3.0) return -1.0;
    const double x2 = x * x;
    return x * (27.0 + x2) / (27.0 + 9.0 * x2);
}

// White noise from a 32-bit LCG; pink noise by Paul Kellet's economy filter
struct Noise
{
    unsigned int s;
    double b0, b1, b2;
    Noise() : s (22222u), b0 (0), b1 (0), b2 (0) {}
    void seed (unsigned int v) { s = v ? v : 1u; b0 = b1 = b2 = 0.0; }
    double white() { s = s * 1664525u + 1013904223u; return ((double) (s >> 8) / 8388608.0) - 1.0; }
    double uniform() { s = s * 1664525u + 1013904223u; return (double) (s >> 8) / 16777216.0; }
    double pink()
    {
        const double w = white();
        b0 = 0.99765 * b0 + w * 0.0990460;
        b1 = 0.96300 * b1 + w * 0.2965164;
        b2 = 0.57000 * b2 + w * 1.0526913;
        return (b0 + b1 + b2 + w * 0.1848) * 0.25;
    }
};

// polyBLEP: removes most aliasing from a step at phase 0 (t = phase 0..1, dt = increment)
inline double polyBlep (double t, double dt)
{
    if (dt <= 0.0) return 0.0;
    if (t < dt) { t /= dt; return t + t - t * t - 1.0; }
    if (t > 1.0 - dt) { t = (t - 1.0) / dt; return t * t + t + t + 1.0; }
    return 0.0;
}
// integrated polyBLEP (polyBLAMP) for slope changes (triangle corners)
inline double polyBlamp (double t, double dt)
{
    if (dt <= 0.0) return 0.0;
    if (t < dt) { t = t / dt - 1.0; return -1.0 / 3.0 * t * t * t; }
    if (t > 1.0 - dt) { t = (t - 1.0) / dt + 1.0; return 1.0 / 3.0 * t * t * t; }
    return 0.0;
}

inline double wrap01 (double p) { return p - std::floor (p); }

inline double sawBL (double ph, double dt) { return 2.0 * ph - 1.0 - polyBlep (ph, dt); }
inline double pulseBL (double ph, double dt, double width)
{
    double y = ph < width ? 1.0 : -1.0;
    y += polyBlep (ph, dt);
    y -= polyBlep (wrap01 (ph - width + 1.0), dt);
    return y - (2.0 * width - 1.0);     // remove the DC of an uneven pulse
}
inline double triBL (double ph, double dt)
{
    double y = ph < 0.5 ? 4.0 * ph - 1.0 : 3.0 - 4.0 * ph;
    y += 4.0 * dt * (polyBlamp (ph, dt) - polyBlamp (wrap01 (ph + 0.5), dt));
    return y;
}

// One-pole sections, topology preserving (stable to Nyquist)
struct OnePole
{
    double g, s;
    OnePole() : g (0.5), s (0.0) {}
    void set (double hz, double sr) { const double w = std::tan (kPi * clampd (hz, 1.0, 0.45 * sr) / sr); g = w / (1.0 + w); }
    void reset (double v = 0.0) { s = v; }
    double lp (double x) { const double v = (x - s) * g; const double y = v + s; s = y + v; return y; }
    double hp (double x) { return x - lp (x); }
};

// State variable filter (TPT): low, band, high outputs
struct SVF
{
    double g, k, a1, a2, a3, ic1, ic2;
    double low, band, high;
    SVF() : g (0.1), k (1.4), a1 (0), a2 (0), a3 (0), ic1 (0), ic2 (0), low (0), band (0), high (0) { setGK (0.1, 1.4); }
    void setGK (double gg, double kk)
    {
        g = gg; k = kk;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }
    void set (double hz, double q, double sr) { setGK (std::tan (kPi * clampd (hz, 5.0, 0.45 * sr) / sr), 1.0 / clampd (q, 0.05, 100.0)); }
    void reset() { ic1 = ic2 = low = band = high = 0.0; }
    void tick (double x)
    {
        const double v3 = x - ic2;
        const double v1 = a1 * ic1 + a2 * v3;
        const double v2 = ic2 + a2 * ic1 + a3 * v3;
        ic1 = 2.0 * v1 - ic1;
        ic2 = 2.0 * v2 - ic2;
        low = v2; band = v1; high = x - k * v1 - v2;
    }
};

// RBJ biquad (direct form 2 transposed), for fixed tone shaping
struct Biquad
{
    double b0, b1, b2, a1, a2, z1, z2;
    Biquad() : b0 (1), b1 (0), b2 (0), a1 (0), a2 (0), z1 (0), z2 (0) {}
    void reset() { z1 = z2 = 0.0; }
    double tick (double x) { const double y = b0 * x + z1; z1 = b1 * x - a1 * y + z2; z2 = b2 * x - a2 * y; return y; }
    void norm (double B0, double B1, double B2, double A0, double A1, double A2)
    { b0 = B0 / A0; b1 = B1 / A0; b2 = B2 / A0; a1 = A1 / A0; a2 = A2 / A0; }
    static double w0 (double hz, double sr) { return kTwoPi * clampd (hz, 1.0, 0.45 * sr) / sr; }
    void lowpass (double hz, double q, double sr)
    { const double w = w0 (hz, sr), c = std::cos (w), al = std::sin (w) / (2.0 * q);
      norm ((1 - c) / 2, 1 - c, (1 - c) / 2, 1 + al, -2 * c, 1 - al); }
    void highpass (double hz, double q, double sr)
    { const double w = w0 (hz, sr), c = std::cos (w), al = std::sin (w) / (2.0 * q);
      norm ((1 + c) / 2, -(1 + c), (1 + c) / 2, 1 + al, -2 * c, 1 - al); }
    void bandpass (double hz, double q, double sr)     // 0 dB peak
    { const double w = w0 (hz, sr), c = std::cos (w), al = std::sin (w) / (2.0 * q);
      norm (al, 0, -al, 1 + al, -2 * c, 1 - al); }
    void peak (double hz, double q, double db, double sr)
    { const double A = std::pow (10.0, db / 40.0), w = w0 (hz, sr), c = std::cos (w), al = std::sin (w) / (2.0 * q);
      norm (1 + al * A, -2 * c, 1 - al * A, 1 + al / A, -2 * c, 1 - al / A); }
    void lowShelf (double hz, double db, double sr)
    { const double A = std::pow (10.0, db / 40.0), w = w0 (hz, sr), c = std::cos (w), s = std::sin (w);
      const double al = s / 2.0 * std::sqrt (2.0), sq = 2.0 * std::sqrt (A) * al;
      norm (A * ((A + 1) - (A - 1) * c + sq), 2 * A * ((A - 1) - (A + 1) * c), A * ((A + 1) - (A - 1) * c - sq),
            (A + 1) + (A - 1) * c + sq, -2 * ((A - 1) + (A + 1) * c), (A + 1) + (A - 1) * c - sq); }
    void highShelf (double hz, double db, double sr)
    { const double A = std::pow (10.0, db / 40.0), w = w0 (hz, sr), c = std::cos (w), s = std::sin (w);
      const double al = s / 2.0 * std::sqrt (2.0), sq = 2.0 * std::sqrt (A) * al;
      norm (A * ((A + 1) + (A - 1) * c + sq), -2 * A * ((A - 1) + (A + 1) * c), A * ((A + 1) + (A - 1) * c - sq),
            (A + 1) - (A - 1) * c + sq, 2 * ((A - 1) - (A + 1) * c), (A + 1) - (A - 1) * c - sq); }
};

// Analog-style ADSR: RC charging toward an overshoot target in attack (the
// curve of a capacitor charging toward a rail, cut off at full level),
// exponential decay and release. Times are to within -60 dB (attack: to full).
struct Envelope
{
    enum Stage { kIdle, kAttack, kDecay, kSustain, kRelease };
    int stage;
    double level, sr;
    double atkCoef, decCoef, relCoef, sustain;
    Envelope() : stage (kIdle), level (0.0), sr (44100.0), atkCoef (0.0), decCoef (0.0), relCoef (0.0), sustain (1.0) {}
    static double coefFor (double secs, double sr, double ratio)
    {
        const double n = secs * sr;
        return n < 1.0 ? 0.0 : std::exp (-std::log (ratio) / n);
    }
    void setRate (double s) { sr = s; }
    void setAttack (double secs)  { atkCoef = coefFor (secs, sr, 1.0 + 1.0 / 0.3); }   // charges toward 1.3
    void setDecay (double secs)   { decCoef = coefFor (secs, sr, 1000.0); }
    void setRelease (double secs) { relCoef = coefFor (secs, sr, 1000.0); }
    void setSustain (double s)    { sustain = clampd (s, 0.0, 1.0); }
    void gate (bool on, bool retrigger = true)
    {
        if (on) { if (retrigger || stage == kIdle || stage == kRelease) stage = kAttack; }
        else if (stage != kIdle) stage = kRelease;
    }
    void kill() { stage = kIdle; level = 0.0; }
    bool active() const { return stage != kIdle; }
    double tick()
    {
        switch (stage)
        {
            case kAttack:
                level = 1.3 + (level - 1.3) * atkCoef;
                if (level >= 1.0) { level = 1.0; stage = kDecay; }
                break;
            case kDecay:
                level = sustain + (level - sustain) * decCoef;
                if (level - sustain < 1e-4) { level = sustain; stage = kSustain; }
                break;
            case kSustain:
                level = sustain + (level - sustain) * 0.999;      // follows a moving SUSTAIN knob smoothly
                break;
            case kRelease:
                level *= relCoef;
                if (level < 1e-5) { level = 0.0; stage = kIdle; }
                break;
            default: break;
        }
        return level;
    }
};

// Parameter smoothing (one pole, ~ms time constant)
struct Smooth
{
    double v, k;
    Smooth() : v (0.0), k (0.01) {}
    void setTime (double secs, double sr) { k = 1.0 - std::exp (-1.0 / (clampd (secs, 1e-4, 10.0) * sr)); }
    double tick (double target) { v += (target - v) * k; return v; }
};

// Fixed-size delay line with linear or cubic fractional reads
template <int N>
struct Delay
{
    float buf[N];
    int w;
    Delay() : w (0) { clear(); }
    void clear() { std::memset (buf, 0, sizeof (buf)); w = 0; }
    void push (double x) { buf[w] = (float) x; w = (w + 1) & (N - 1); }
    // d samples ago (d >= 1)
    double read (double d) const
    {
        d = clampd (d, 1.0, (double) (N - 4));
        const double pos = (double) w - d;
        const int i = (int) std::floor (pos);
        const double f = pos - i;
        const double xm1 = buf[(i - 1) & (N - 1)], x0 = buf[i & (N - 1)], x1 = buf[(i + 1) & (N - 1)], x2 = buf[(i + 2) & (N - 1)];
        // 4-point Hermite
        const double c1 = 0.5 * (x1 - xm1), c2 = xm1 - 2.5 * x0 + 2.0 * x1 - 0.5 * x2, c3 = 0.5 * (x2 - xm1) + 1.5 * (x0 - x1);
        return ((c3 * f + c2) * f + c1) * f + x0;
    }
};

// Sine lookup (4096 points, linear interpolation), phase 0..1
struct SineTable
{
    float t[4097];
    SineTable() { for (int i = 0; i <= 4096; ++i) t[i] = (float) std::sin (kTwoPi * i / 4096.0); }
    inline float operator() (double ph) const
    {
        const double p = ph * 4096.0;
        const int i = (int) p;
        const float f = (float) (p - i);
        return t[i & 4095] + (t[(i & 4095) + 1] - t[i & 4095]) * f;
    }
};
const SineTable& sineTable();

} // namespace tbsyn

#endif
