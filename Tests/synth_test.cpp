// synth_test.cpp
// Offline checks of TB Synth's DSP: every instrument plays, is in tune, stays
// finite and quiet when it should, the controls do what the panel says, and
// demo WAVs of each instrument are written for listening.
//
//   synth_test <output dir for WAVs>

#include "../Source/DSP/Synth.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>

using namespace tbsyn;

static int failures = 0;
static void check (bool ok, const char* what)
{
    std::printf ("  %s  %s\n", ok ? "ok  " : "FAIL", what);
    if (! ok) ++failures;
}

static const int SR = 44100;

static void writeWav (const std::string& path, const std::vector<float>& x, int sr)
{
    FILE* f = std::fopen (path.c_str(), "wb");
    if (! f) return;
    const unsigned int n = (unsigned int) x.size();
    const unsigned int bytes = n * 2;
    unsigned char h[44];
    std::memcpy (h, "RIFF", 4);
    const unsigned int riff = 36 + bytes;
    for (int i = 0; i < 4; ++i) h[4 + i] = (unsigned char) (riff >> (8 * i));
    std::memcpy (h + 8, "WAVEfmt ", 8);
    const unsigned int fmt[4] = { 16, 0, 0, 0 };
    (void) fmt;
    h[16] = 16; h[17] = h[18] = h[19] = 0;
    h[20] = 1; h[21] = 0; h[22] = 1; h[23] = 0;
    for (int i = 0; i < 4; ++i) h[24 + i] = (unsigned char) ((unsigned) sr >> (8 * i));
    const unsigned int br = sr * 2;
    for (int i = 0; i < 4; ++i) h[28 + i] = (unsigned char) (br >> (8 * i));
    h[32] = 2; h[33] = 0; h[34] = 16; h[35] = 0;
    std::memcpy (h + 36, "data", 4);
    for (int i = 0; i < 4; ++i) h[40 + i] = (unsigned char) (bytes >> (8 * i));
    std::fwrite (h, 1, 44, f);
    for (unsigned int i = 0; i < n; ++i)
    {
        double v = x[i];
        v = v > 1.0 ? 1.0 : (v < -1.0 ? -1.0 : v);
        const short s = (short) (v * 32767.0);
        unsigned char b[2] = { (unsigned char) (s & 0xff), (unsigned char) ((s >> 8) & 0xff) };
        std::fwrite (b, 1, 2, f);
    }
    std::fclose (f);
}

static bool finite (const std::vector<float>& x)
{
    for (size_t i = 0; i < x.size(); ++i) if (! (x[i] == x[i]) || std::fabs (x[i]) > 4.0f) return false;
    return true;
}
static double peakOf (const std::vector<float>& x, size_t a, size_t b)
{
    double p = 0.0;
    for (size_t i = a; i < b && i < x.size(); ++i) p = std::max (p, (double) std::fabs (x[i]));
    return p;
}
static double rmsOf (const std::vector<float>& x, size_t a, size_t b)
{
    double s = 0.0; size_t n = 0;
    for (size_t i = a; i < b && i < x.size(); ++i) { s += (double) x[i] * x[i]; ++n; }
    return n ? std::sqrt (s / n) : 0.0;
}
static double db (double x) { return 20.0 * std::log10 (std::max (x, 1e-12)); }

// Strongest spectral peak between lo and hi Hz (Hann-windowed DFT, coarse then fine)
static double peakHz (const std::vector<float>& x, size_t a, size_t b, double lo, double hi, double rate = SR)
{
    const size_t n = b - a;
    double best = lo, bm = -1.0;
    for (int pass = 0; pass < 2; ++pass)
    {
        const double from = pass ? best - 0.6 : lo, to = pass ? best + 0.6 : hi, step = pass ? 0.01 : 0.5;
        for (double f = from; f <= to; f += step)
        {
            double re = 0.0, im = 0.0;
            const double w = kTwoPi * f / rate;
            for (size_t i = 0; i < n; ++i)
            {
                const double win = 0.5 - 0.5 * std::cos (kTwoPi * i / n);
                re += x[a + i] * win * std::cos (w * i);
                im += x[a + i] * win * std::sin (w * i);
            }
            const double m = re * re + im * im;
            if (m > bm) { bm = m; best = f; }
        }
    }
    return best;
}
// The fundamental near an expected pitch (within a fifth either way)
static double pitchOf (const std::vector<float>& x, size_t a, size_t b, double expect)
{
    return peakHz (x, a, b, expect / 1.45, expect * 1.45);
}

static void render (Synth& s, std::vector<float>& out, size_t from, size_t count, int block = 256)
{
    if (out.size() < from + count) out.resize (from + count, 0.0f);
    for (size_t pos = 0; pos < count; pos += block)
    {
        const int n = (int) std::min ((size_t) block, count - pos);
        s.process (&out[from + pos], n);
    }
}

static void selectInst (Synth& s, int inst)
{
    s.setParam (kInst, instValue (inst));
    std::vector<float> tmp;
    render (s, tmp, 0, SR / 20);      // let the switch fade through
}

static void defaults (Synth& s)
{
    for (int i = 0; i < kNumParams; ++i) s.setParam (i, kParams[i].def);
}

int main (int argc, char** argv)
{
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf ("Parameter table\n");
    {
        bool namesOk = true, uniq = true, defOk = true, choicesOk = true;
        for (int i = 0; i < kNumParams; ++i)
        {
            if (std::strlen (kParams[i].name) > 8) { namesOk = false; std::printf ("  long name %s\n", kParams[i].name); }
            for (int j = 0; j < i; ++j) if (! std::strcmp (kParams[i].name, kParams[j].name)) { uniq = false; std::printf ("  duplicate %s\n", kParams[i].name); }
            if (kParams[i].def < 0.0f || kParams[i].def > 1.0f) defOk = false;
            if (kParams[i].unit == kUChoice)
            {
                int bars = 0;
                for (const char* c = kParams[i].choices; c && *c; ++c) if (*c == '|') ++bars;
                if (! kParams[i].choices || bars + 1 != kParams[i].steps) { choicesOk = false; std::printf ("  choices %s\n", kParams[i].name); }
            }
            if (kParams[i].steps >= 2 && std::fabs (stepValue (paramStep (i, kParams[i].def), kParams[i].steps) - kParams[i].def) > 1e-4)
            { defOk = false; std::printf ("  default between steps: %s\n", kParams[i].name); }
        }
        std::printf ("  %d parameters\n", (int) kNumParams);
        check (namesOk, "every name fits 8 characters");
        check (uniq, "names are unique");
        check (defOk, "defaults in range and on a step for switches");
        check (choicesOk, "every selector has a name for each position");
    }

    Synth* syn = new Synth();
    Synth& s = *syn;
    s.prepare (SR);

    // ---------------------------------------------------------------- demos
    std::printf ("\nEvery instrument plays (defaults, a C major chord or a line)\n");
    for (int inst = 0; inst < kNumInstruments; ++inst)
    {
        defaults (s);
        selectInst (s, inst);
        std::vector<float> y;
        const bool mono = inst == kMinimoog;
        const int chord[4] = { 48, 55, 64, 67 };
        size_t t = 0;
        if (mono)
        {
            const int line[8] = { 36, 43, 48, 46, 43, 41, 43, 36 };
            for (int k = 0; k < 8; ++k)
            {
                s.midi (0x90, line[k], 100);
                render (s, y, t, SR / 4); t += SR / 4;
                s.midi (0x80, line[k], 0);
                render (s, y, t, SR / 20); t += SR / 20;
            }
        }
        else
        {
            for (int k = 0; k < 4; ++k) s.midi (0x90, chord[k], 96);
            render (s, y, t, SR * 2); t += SR * 2;
            for (int k = 0; k < 4; ++k) s.midi (0x80, chord[k], 0);
        }
        render (s, y, t, SR * 2); t += SR * 2;
        const double pk = peakOf (y, 0, t), rm = rmsOf (y, 0, SR);
        const double tail = peakOf (y, t - SR / 2, t);
        char name[128];
        std::snprintf (name, sizeof (name), "%s: peak %.1f dBFS, rms %.1f dBFS, tail %.1f dBFS",
                       instrumentName (inst), db (pk), db (rm), db (tail));
        std::printf ("  %s\n", name);
        check (finite (y), "finite");
        check (pk > 0.03 && pk < 1.0, "plays at a sensible level (no clipping)");
        check (tail < 0.003, "falls silent after the keys are up");
        std::string fn = dir + "/demo_" + instrumentShort (inst) + ".wav";
        for (size_t c = 0; c < fn.size(); ++c) if (fn[c] == ' ') fn[c] = '_';
        writeWav (fn, y, SR);
    }

    // ---------------------------------------------------------------- tuning
    std::printf ("\nTuning\n");
    {
        // Minimoog: one sawtooth at 8', filter open, A4
        defaults (s);
        selectInst (s, kMinimoog);
        s.setParam (kMgO1Rng, stepValue (3, 6)); s.setParam (kMgO2On, 0.0f); s.setParam (kMgCutoff, 1.0f);
        s.setParam (kMgEmph, 0.0f); s.setParam (kMgO1Wave, stepValue (0, 6));
        std::vector<float> y;
        s.midi (0x90, 69, 100);
        render (s, y, 0, SR);
        s.midi (0x80, 69, 0);
        const double hz = pitchOf (y, SR / 4, SR, 440.0);
        std::printf ("  Minimoog A4 at 8': %.2f Hz\n", hz);
        check (std::fabs (hz - 440.0) < 1.0, "Minimoog 8' plays concert pitch");
        render (s, y, SR, SR / 2);
        // 16' range: an octave down
        s.setParam (kMgO1Rng, stepValue (2, 6));
        y.clear();
        s.midi (0x90, 69, 100);
        render (s, y, 0, SR);
        s.midi (0x80, 69, 0);
        const double hz16 = pitchOf (y, SR / 4, SR, 220.0);
        std::printf ("  Minimoog A4 at 16': %.2f Hz\n", hz16);
        check (std::fabs (hz16 - 220.0) < 1.0, "16' is an octave down");
        render (s, y, SR, SR / 2);
    }
    {
        // Hammond: 8' alone, A4: the tonewheel gear gives 440 Hz exactly
        defaults (s);
        selectInst (s, kHammond);
        for (int b = 0; b < 9; ++b) s.setParam (kB3Db1 + b, b == 2 ? 1.0f : 0.0f);
        s.setParam (kB3Perc, 0.0f); s.setParam (kB3VibOn, 0.0f); s.setParam (kLsOn, 0.0f); s.setParam (kB3Drive, 0.0f);
        std::vector<float> y;
        s.midi (0x90, 69, 100);
        render (s, y, 0, SR);
        const double hz = pitchOf (y, SR / 4, SR, 440.0);
        s.midi (0x80, 69, 0);
        render (s, y, SR, SR / 2);
        std::printf ("  B-3 A4 on 8': %.2f Hz\n", hz);
        check (std::fabs (hz - 440.0) < 0.5, "B-3 tonewheel A is 440 Hz");
        y.clear();
        s.midi (0x90, 60, 100);
        render (s, y, 0, SR);
        const double c4 = pitchOf (y, SR / 4, SR, 261.6);
        s.midi (0x80, 60, 0);
        render (s, y, SR, SR / 2);
        std::printf ("  B-3 C4 on 8': %.3f Hz (equal temperament 261.626)\n", c4);
        check (std::fabs (c4 - 261.538) < 0.2, "B-3 C is the 85/104 gear's 261.54 Hz, a little flat of equal temperament");
    }
    {
        const int insts[7] = { kOBX, kWurlitzer, kCembalet, kClavinet, kSolina, kMultiman, kOmni };
        for (int k = 0; k < 7; ++k)
        {
            defaults (s);
            selectInst (s, insts[k]);
            if (insts[k] == kOBX) { s.setParam (kOxO2Det, 0.5f); s.setParam (kOxCut, 0.4f); s.setParam (kOxEnvAm, 0.0f); }
            std::vector<float> y;
            s.midi (0x90, 57, 100);         // A3, 220 Hz
            render (s, y, 0, SR / 2);
            const double hz = pitchOf (y, SR / 10, SR / 2, 220.0);
            s.midi (0x80, 57, 0);
            render (s, y, SR / 2, SR * 2);
            std::printf ("  %s A3: %.2f Hz\n", instrumentName (insts[k]), hz);
            check (std::fabs (hz - 220.0) < 2.0, "in tune");
        }
    }

    // ---------------------------------------------------------------- behaviour
    std::printf ("\nBehaviour\n");
    {
        // Minimoog low-note priority: hold C3, add G3: still C3
        defaults (s);
        selectInst (s, kMinimoog);
        s.setParam (kMgO1Rng, stepValue (3, 6)); s.setParam (kMgO2On, 0.0f); s.setParam (kMgCutoff, 1.0f); s.setParam (kMgEmph, 0.0f);
        s.setParam (kMgO1Wave, stepValue (0, 6));
        std::vector<float> y;
        s.midi (0x90, 48, 100);
        render (s, y, 0, SR / 4);
        s.midi (0x90, 55, 100);
        render (s, y, SR / 4, SR / 2);
        const double hz = pitchOf (y, SR / 4 + SR / 10, SR * 3 / 4, 150.0);
        std::printf ("  C3 held, G3 added: %.1f Hz\n", hz);
        check (std::fabs (hz - noteHz (48)) < 1.5, "Minimoog keeps the lowest note");
        s.midi (0x80, 48, 0);
        y.clear();
        render (s, y, 0, SR / 2);
        const double hz2 = pitchOf (y, SR / 10, SR / 2, 180.0);
        check (std::fabs (hz2 - noteHz (55)) < 2.0, "and goes to G3 when C3 is let go");
        s.midi (0x80, 55, 0);
        render (s, y, 0, SR);
        // emphasis at 10 self-oscillates with no oscillators on
        s.setParam (kMgO1On, 0.0f); s.setParam (kMgEmph, 1.0f); s.setParam (kMgCutoff, 0.55f);
        y.clear();
        s.midi (0x90, 60, 100);
        render (s, y, 0, SR);
        s.midi (0x80, 60, 0);
        render (s, y, SR, SR);
        std::printf ("  EMPHASIS 10, mixer off: %.1f dBFS\n", db (rmsOf (y, SR / 2, SR)));
        check (rmsOf (y, SR / 2, SR) > 0.01, "the ladder whistles on its own at full EMPHASIS");
        s.setParam (kMgEmph, 0.6f);
        y.clear();
        s.midi (0x90, 60, 100);
        render (s, y, 0, SR);
        s.midi (0x80, 60, 0);
        render (s, y, SR, SR);
        check (rmsOf (y, SR / 2, SR) < 0.002, "and not at 6");
    }
    {
        // Clavinet: no tone switch, no sound
        defaults (s);
        selectInst (s, kClavinet);
        for (int p = kClBrill; p <= kClSoft; ++p) s.setParam (p, 0.0f);
        std::vector<float> y;
        s.midi (0x90, 60, 110);
        render (s, y, 0, SR / 2);
        s.midi (0x80, 60, 0);
        check (peakOf (y, 0, SR / 2) < 1e-6, "Clavinet: no tone switch down, no sound (as the D6)");
        // D-B out of phase is thinner than D-A
        s.setParam (kClTreb, 1.0f);
        s.setParam (kClCD, 1.0f);
        double lv[2];
        for (int ab = 0; ab < 2; ++ab)
        {
            s.setParam (kClAB, (float) ab);
            y.clear();
            render (s, y, 0, SR / 5);
            s.midi (0x90, 48, 110);
            render (s, y, 0, SR / 2);
            s.midi (0x80, 48, 0);
            lv[ab] = rmsOf (y, 0, SR / 2);
        }
        std::printf ("  D-A %.1f dB, D-B %.1f dB\n", db (lv[0]), db (lv[1]));
        check (lv[1] < lv[0], "D-B (out of phase) is thinner than D-A");
        // the mute shortens notes
        s.setParam (kClCD, 0.0f);
        double dec[2];
        for (int m = 0; m < 2; ++m)
        {
            s.setParam (kClMute, m ? 0.8f : 0.0f);
            y.clear();
            render (s, y, 0, SR / 5);
            s.midi (0x90, 48, 110);
            render (s, y, 0, SR);
            s.midi (0x80, 48, 0);
            dec[m] = rmsOf (y, SR * 3 / 4, SR) / rmsOf (y, 0, SR / 8);
        }
        check (dec[1] < dec[0] * 0.5, "MUTE shortens the notes");
        s.setParam (kClMute, 0.0f);
    }
    {
        // Wurlitzer: harder = more harmonics (the bark)
        defaults (s);
        selectInst (s, kWurlitzer);
        double ratio[2];
        const int vels[2] = { 30, 127 };
        for (int k = 0; k < 2; ++k)
        {
            std::vector<float> y;
            render (s, y, 0, SR / 2);
            y.clear();
            s.midi (0x90, 57, vels[k]);
            render (s, y, 0, SR / 2);
            s.midi (0x80, 57, 0);
            // energy above the fundamental: difference against a low-passed copy
            double lp = 0.0, hiE = 0.0, allE = 0.0;
            for (int i = SR / 20; i < SR / 2; ++i)
            {
                lp += (y[i] - lp) * 0.04;          // ~ 290 Hz one-pole
                hiE += (y[i] - lp) * (y[i] - lp);
                allE += (double) y[i] * y[i];
            }
            ratio[k] = hiE / allE;
        }
        std::printf ("  harmonic share: soft %.3f, hard %.3f\n", ratio[0], ratio[1]);
        check (ratio[1] > ratio[0] * 1.3, "Wurlitzer barks when played hard");
        // VIBRATO: the 200A's tremolo
        s.setParam (kWuVib, 1.0f);
        std::vector<float> y;
        render (s, y, 0, SR / 2);
        y.clear();
        s.midi (0x90, 69, 90);
        render (s, y, 0, SR);
        s.midi (0x80, 69, 0);
        double mx = 0.0, mn = 1e9;
        for (int w = 0; w < 10; ++w)
        {
            const double r = rmsOf (y, SR / 10 + w * SR / 60, SR / 10 + (w + 1) * SR / 60);
            mx = std::max (mx, r); mn = std::min (mn, r);
        }
        check (mx / mn > 1.8, "VIBRATO modulates the level");
        s.setParam (kWuVib, 0.0f);
    }
    {
        // Hammond percussion: single trigger
        defaults (s);
        selectInst (s, kHammond);
        s.setParam (kLsOn, 0.0f); s.setParam (kB3VibOn, 0.0f);
        for (int b = 0; b < 9; ++b) s.setParam (kB3Db1 + b, 0.0f);
        std::vector<float> y;
        s.midi (0x90, 60, 100);
        render (s, y, 0, SR / 4);
        const double first = rmsOf (y, 0, SR / 20);
        s.midi (0x90, 64, 100);         // legato: no new percussion
        const size_t t = y.size();
        render (s, y, t, SR / 4);
        const double legato = rmsOf (y, t, t + SR / 20);
        s.midi (0x80, 60, 0); s.midi (0x80, 64, 0);
        render (s, y, y.size(), SR / 2);
        std::printf ("  percussion first %.1f dB, legato %.1f dB\n", db (first), db (legato));
        check (first > 0.01 && legato < first * 0.5, "percussion only from all keys up (single trigger)");
        // the Leslie: FAST spins the horn up, a level wobble at about 6.7 Hz
        defaults (s);
        selectInst (s, kHammond);
        s.setParam (kLsSpeed, 1.0f);
        s.setParam (kB3Perc, 0.0f); s.setParam (kB3VibOn, 0.0f);
        for (int b = 0; b < 9; ++b) s.setParam (kB3Db1 + b, b == 3 || b == 5 ? 1.0f : 0.0f);
        y.clear();
        s.midi (0x90, 81, 100);
        render (s, y, 0, SR * 3);
        s.midi (0x80, 81, 0);
        // the level's wobble once at speed: its strongest rate between 2 and 12 Hz
        std::vector<float> envl;
        double e = 0.0;
        for (int i = SR; i < SR * 3; ++i)
        {
            e += (std::fabs (y[i]) - e) * 0.004;
            if (i % 49 == 0) envl.push_back ((float) e);      // 900 Hz rate
        }
        double mean = 0.0;
        for (size_t i = 0; i < envl.size(); ++i) mean += envl[i];
        mean /= envl.size();
        for (size_t i = 0; i < envl.size(); ++i) envl[i] -= (float) mean;
        const double rate = peakHz (envl, envl.size() / 2, envl.size(), 2.0, 12.0, SR / 49.0);
        std::printf ("  Leslie fast: the level swings at %.2f Hz\n", rate);
        check (rate > 5.0 && rate < 8.0, "Leslie FAST: the rotors turn about 6 to 7 times a second");
        render (s, y, 0, SR);
    }
    {
        // Solina: the ensemble moves (no two cycles alike), crescendo slows the start
        defaults (s);
        selectInst (s, kSolina);
        double early[2];
        for (int c = 0; c < 2; ++c)
        {
            s.setParam (kSoCresc, c ? 0.9f : 0.0f);
            std::vector<float> y;
            render (s, y, 0, SR / 2);
            y.clear();
            s.midi (0x90, 60, 100);
            render (s, y, 0, SR);
            s.midi (0x80, 60, 0);
            early[c] = rmsOf (y, SR / 20, SR / 10) / std::max (1e-9, rmsOf (y, SR / 2, SR));
            render (s, y, 0, SR * 3);
        }
        check (early[1] < early[0] * 0.5, "CRESCENDO slows the attack");
    }
    {
        // Multiman: the bass pedals play from the lowest 13 keys, an octave down, one at a time
        defaults (s);
        selectInst (s, kMultiman);
        s.setParam (kCrCello, 0.0f); s.setParam (kCrViolin, 0.0f); s.setParam (kCrBassOn, 1.0f);
        std::vector<float> y;
        s.midi (0x90, 33, 100);       // A1 -> the A below, 27.5 Hz... the pedal's 55 Hz with the 16' octave
        render (s, y, 0, SR);
        const double hz = pitchOf (y, SR / 4, SR, 27.5);
        s.midi (0x80, 33, 0);
        render (s, y, SR, SR / 2);
        std::printf ("  Multiman bass pedal from A1: %.2f Hz\n", hz);
        check (std::fabs (hz - 27.5) < 0.5, "Multiman bass pedals: the lowest keys, an octave down");
        y.clear();
        s.midi (0x90, 72, 100);       // above the pedal keys: no bass
        render (s, y, 0, SR / 2);
        s.midi (0x80, 72, 0);
        check (peakOf (y, 0, SR / 2) < 1e-6, "keys above F2 don't play the bass");
        // brass: every key contact retriggers the contour (a second key brightens it again)
        defaults (s);
        selectInst (s, kMultiman);
        s.setParam (kCrCello, 0.0f); s.setParam (kCrViolin, 0.0f);
        s.setParam (kCrBrOnLo, 1.0f); s.setParam (kCrBrOnHi, 1.0f); s.setParam (kCrDec, 0.2f); s.setParam (kCrAtk, 0.0f);
        s.setParam (kCrCont, 0.1f);     // filter nearly closed once the contour has decayed
        y.clear();
        s.midi (0x90, 48, 100);
        render (s, y, 0, SR);
        const double settled = rmsOf (y, SR * 3 / 4, SR);
        s.midi (0x90, 55, 100);
        render (s, y, SR, SR / 10);
        const double again = rmsOf (y, SR + SR / 200, SR + SR / 40);
        s.midi (0x80, 48, 0); s.midi (0x80, 55, 0);
        render (s, y, y.size(), SR);
        std::printf ("  brass held %.1f dB, after a second key %.1f dB\n", db (settled), db (again));
        check (again > settled * 1.5, "Multiman brass: each key retriggers the filter contour");
    }
    {
        // Omni: CELLOS / BASS are monophonic on the bottom 20 keys, low-note priority; BASS 16' an octave down
        defaults (s);
        selectInst (s, kOmni);
        s.setParam (kOmViolin, 0.0f); s.setParam (kOmViola, 0.0f); s.setParam (kOmSy8, 0.0f); s.setParam (kOmBass, 1.0f);
        std::vector<float> y;
        s.midi (0x90, 45, 100);        // A2 -> BASS 16' at 55 Hz
        s.midi (0x90, 52, 100);        // E3 held too: the lower key wins
        render (s, y, 0, SR);
        const double hz = pitchOf (y, SR / 4, SR / 2, 55.0);
        const double early = rmsOf (y, SR / 10, SR / 5), late = rmsOf (y, SR * 4 / 5, SR);
        s.midi (0x80, 45, 0); s.midi (0x80, 52, 0);
        render (s, y, SR, SR / 2);
        std::printf ("  Omni BASS 16' from A2 (+E3): %.2f Hz, decay while held %.1f dB\n", hz, db (late / early));
        check (std::fabs (hz - 55.0) < 0.6, "Omni bass: low-note priority, 16' an octave down");
        check (late < early * 0.9, "Omni bass keying decays while the key is held");
        check (rmsOf (y, SR + SR / 5, SR + SR / 2) < 1e-4, "Omni bass stops quickly when the keys are up");
        y.clear();
        s.midi (0x90, 60, 100);        // above the bass keys
        render (s, y, 0, SR / 2);
        s.midi (0x80, 60, 0);
        check (peakOf (y, 0, SR / 2) < 1e-6, "keys above G3 don't play the bass");
        render (s, y, 0, SR * 2);
        // the sustain pedal holds the strings
        defaults (s);
        selectInst (s, kOmni);
        s.setParam (kOmSRel, 0.0f);
        y.clear();
        s.midi (0xB0, 64, 127);
        s.midi (0x90, 69, 100);
        render (s, y, 0, SR / 2);
        s.midi (0x80, 69, 0);
        render (s, y, SR / 2, SR / 2);
        const double held = rmsOf (y, SR * 3 / 4, SR);
        s.midi (0xB0, 64, 0);
        render (s, y, SR, SR / 2);
        const double after = rmsOf (y, SR + SR / 4, SR + SR / 2);
        std::printf ("  Omni pedal held %.1f dB, released %.1f dB\n", db (held), db (after));
        check (held > 1e-3 && after < held * 0.01, "Omni: the sustain pedal holds every note");
        // the synthesizer: one VCF, opened by the ADSR (a closed filter gets brighter with ADSR depth)
        double cent[2];
        for (int c = 0; c < 2; ++c)
        {
            defaults (s);
            selectInst (s, kOmni);
            s.setParam (kOmMix, 1.0f); s.setParam (kOmVcfF, 0.2f); s.setParam (kOmVcfEnv, c ? 0.8f : 0.0f);
            s.setParam (kOmS, 1.0f);
            y.clear();
            s.midi (0x90, 57, 100);
            render (s, y, 0, SR / 2);
            s.midi (0x80, 57, 0);
            cent[c] = peakHz (y, SR / 4, SR / 2, 100.0, 8000.0);
            double hi = 0.0; for (size_t i = SR / 4 + 1; i < (size_t) SR / 2; ++i) hi += (y[i] - y[i - 1]) * (y[i] - y[i - 1]);
            cent[c] = hi / std::max (1e-12, rmsOf (y, SR / 4, SR / 2) * rmsOf (y, SR / 4, SR / 2) * (SR / 4));
            render (s, y, 0, SR * 3);
        }
        std::printf ("  Omni VCF brightness: ADSR 0 %.4f, ADSR 8 %.4f\n", cent[0], cent[1]);
        check (cent[1] > cent[0] * 2.0, "Omni VCF: the ADSR opens the filter");
    }
    {
        // SK-30: organ 8' alone is in tune; 7-note polyphony; STRING 3 an octave above STRING 2;
        // the solo synthesizer plays the highest key of the upper three octaves
        defaults (s);
        selectInst (s, kSK30);
        s.setParam (kSkOrgSel, 1.0f);
        const int lv[9] = { kSkO16, kSkO8, kSkO513, kSkO4, kSkO223, kSkO2, kSkO135, kSkO113, kSkO1 };
        for (int i = 0; i < 9; ++i) s.setParam (lv[i], i == 1 ? 1.0f : 0.0f);
        std::vector<float> y;
        s.midi (0x90, 69, 100);
        render (s, y, 0, SR / 2);
        s.midi (0x80, 69, 0);
        const double a4 = pitchOf (y, SR / 8, SR / 2, 440.0);
        render (s, y, 0, SR / 2);
        std::printf ("  SK-30 organ 8' A4: %.2f Hz\n", a4);
        check (std::fabs (a4 - 440.0) < 2.0, "SK-30 organ in tune");
        // eight keys: the first is taken for the eighth
        y.clear();
        const int keys[8] = { 48, 52, 55, 60, 64, 67, 71, 74 };
        for (int k = 0; k < 7; ++k) s.midi (0x90, keys[k], 100);
        render (s, y, 0, SR / 4);
        const double seven = rmsOf (y, SR / 8, SR / 4);
        s.midi (0x90, keys[7], 100);
        render (s, y, SR / 4, SR / 4);
        const double eight = rmsOf (y, SR * 3 / 8, SR / 2);
        for (int k = 0; k < 8; ++k) s.midi (0x80, keys[k], 0);
        render (s, y, 0, SR / 2);
        std::printf ("  SK-30 seven keys %.1f dB, eight keys %.1f dB\n", db (seven), db (eight));
        check (std::fabs (db (eight) - db (seven)) < 2.5, "SK-30: 7-note polyphony (the eighth key takes a channel)");
        // strings
        double hz[2];
        for (int k = 0; k < 2; ++k)
        {
            defaults (s);
            selectInst (s, kSK30);
            s.setParam (kSkOrgV, 0.0f); s.setParam (kSkStrV, 0.8f); s.setParam (kSkStrSel, k ? 1.0f : 2.0f / 3.0f);
            y.clear();
            s.midi (0x90, 57, 100);
            render (s, y, 0, SR / 2);
            s.midi (0x80, 57, 0);
            hz[k] = pitchOf (y, SR / 4, SR / 2, k ? 440.0 : 220.0);
            render (s, y, 0, SR);
        }
        std::printf ("  SK-30 A3 on STRING 2: %.1f Hz, STRING 3: %.1f Hz\n", hz[0], hz[1]);
        check (std::fabs (hz[0] - 220.0) < 2.0 && std::fabs (hz[1] - 440.0) < 3.0, "SK-30 strings: each preset an octave higher");
        // solo: high-note priority on the upper three octaves, nothing below
        defaults (s);
        selectInst (s, kSK30);
        s.setParam (kSkOrgV, 0.0f); s.setParam (kSkSoloV, 0.8f); s.setParam (kSkSoVca, 0.5f); s.setParam (kSkSoCut, 0.9f);
        y.clear();
        s.midi (0x90, 48, 100);
        render (s, y, 0, SR / 4);
        const double low = peakOf (y, 0, SR / 4);
        s.midi (0x90, 69, 100); s.midi (0x90, 64, 100);
        render (s, y, SR / 4, SR / 2);
        const double solo = pitchOf (y, SR / 2, SR * 3 / 4, 440.0);
        s.midi (0x80, 48, 0); s.midi (0x80, 69, 0); s.midi (0x80, 64, 0);
        render (s, y, 0, SR / 2);
        std::printf ("  SK-30 solo with E4 + A4 held: %.1f Hz\n", solo);
        check (low < 1e-6 && std::fabs (solo - 440.0) < 2.0, "SK-30 solo: the upper three octaves, high-note priority");
    }
    {
        // instrument switch while a note sounds: no jump bigger than the signal itself
        defaults (s);
        selectInst (s, kOBX);
        std::vector<float> y;
        s.midi (0x90, 60, 100);
        render (s, y, 0, SR / 2);
        s.setParam (kInst, instValue (kHammond));
        render (s, y, SR / 2, SR / 2);
        s.midi (0x80, 60, 0);
        double jump = 0.0;
        for (int i = SR / 2 - 10; i < SR / 2 + SR / 50; ++i) jump = std::max (jump, (double) std::fabs (y[i] - y[i - 1]));
        std::printf ("  largest step through the switch: %.3f\n", jump);
        check (jump < 0.2, "switching instruments fades, no click");
        check (s.instrument() == kHammond, "the new instrument is playing");
    }

    // ---------------------------------------------------------------- robustness
    std::printf ("\nRobustness\n");
    {
        std::clock_t t0 = std::clock();
        double secsAudio = 0.0;
        Noise rnd; rnd.seed (99u);
        bool ok = true;
        for (int inst = 0; inst < kNumInstruments; ++inst)
        {
            selectInst (s, inst);
            for (int trial = 0; trial < 6; ++trial)
            {
                for (int p = 2; p < kNumParams; ++p)
                    if (kParams[p].inst == inst) s.setParam (p, trial == 0 ? 1.0f : (trial == 1 ? 0.0f : (float) rnd.uniform()));
                if (inst == kMinimoog) s.setParam (kMgLAtk, 0.0f);
                std::vector<float> y;
                for (int k = 0; k < 10; ++k) s.midi (0x90, 24 + (int) (rnd.uniform() * 84), 1 + (int) (rnd.uniform() * 126));
                s.midi (0xE0, 0, (unsigned char) (rnd.uniform() * 127));
                s.midi (0xB0, 1, (unsigned char) (rnd.uniform() * 127));
                render (s, y, 0, SR / 2, 1 + (int) (rnd.uniform() * 700));
                // automate everything while playing
                for (int blk = 0; blk < 40; ++blk)
                {
                    for (int p = 2; p < kNumParams; ++p) if (kParams[p].inst == inst) s.setParam (p, (float) rnd.uniform());
                    render (s, y, y.size(), 128);
                }
                s.midi (0xB0, 123, 0);
                render (s, y, y.size(), SR / 4);
                secsAudio += y.size() / (double) SR;
                if (! finite (y)) { ok = false; std::printf ("  not finite: %s trial %d\n", instrumentName (inst), trial); }
            }
        }
        check (ok, "random and extreme settings, all controls moving, every instrument: finite");
        const double cpu = (double) (std::clock() - t0) / CLOCKS_PER_SEC;
        std::printf ("  CPU: %.1f%% of one core (busy, 10 notes)\n", 100.0 * cpu / secsAudio);
    }
    {
        // 96 kHz and 192 kHz
        bool ok = true;
        const double rates[2] = { 96000.0, 192000.0 };
        for (int r = 0; r < 2; ++r)
        {
            s.prepare (rates[r]);
            for (int inst = 0; inst < kNumInstruments; ++inst)
            {
                defaults (s);
                s.setParam (kInst, instValue (inst));
                std::vector<float> y;
                render (s, y, 0, (size_t) (rates[r] / 20));
                s.midi (0x90, 29, 120); s.midi (0x90, 60, 120); s.midi (0x90, 96, 120);
                render (s, y, 0, (size_t) (rates[r] / 2));
                s.midi (0xB0, 123, 0);
                if (! finite (y) || peakOf (y, 0, y.size()) < 0.01) { ok = false; std::printf ("  %s at %.0f\n", instrumentName (inst), rates[r]); }
            }
        }
        check (ok, "every instrument plays at 96 and 192 kHz");
        s.prepare (SR);
    }

    delete syn;
    std::printf ("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL SYNTH CHECKS PASSED", failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
