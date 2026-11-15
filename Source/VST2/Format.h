// Format.h
// How TB Synth shows its values: short (8 characters, for the host's generic
// display) and long (with units, for the panel's readout). Header only.

#ifndef TBSYNTH_FORMAT_H
#define TBSYNTH_FORMAT_H

#include "../DSP/SynthParams.h"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace tbsyn
{

// The k-th name of a "A|B|C" list
inline void choiceName (const char* list, int k, char* out, size_t size)
{
    out[0] = 0;
    if (! list || size == 0) return;
    int i = 0;
    const char* p = list;
    while (i < k && *p) { if (*p == '|') ++i; ++p; }
    size_t n = 0;
    while (p[n] && p[n] != '|' && n + 1 < size) { out[n] = p[n]; ++n; }
    out[n] = 0;
}

// The physical value behind a knob position
inline double realValue (int p, double v)
{
    switch (p)
    {
        case kMgTune: return map::mgTuneSemis (v);
        case kMgGlide: return map::mgGlide (v);
        case kMgO2Freq: case kMgO3Freq: return map::mgOscFreqSemis (v);
        case kMgCutoff: return map::mgCutoff (v);
        case kMgContAm: return map::mgContour (v);
        case kMgFAtk: case kMgLAtk: return map::mgAttack (v);
        case kMgFDec: case kMgLDec: return map::mgDecay (v);
        case kOxLfoRt: return map::oxLfo (v);
        case kOxO1Frq: case kOxO2Frq: return map::oxOscSemis (v);
        case kOxO2Det: return map::oxDetuneCents (v);
        case kOxPW: return 100.0 * map::oxPulseWidth (v);
        case kOxCut: return map::oxCutoff (v);
        case kOxEnvAm: return map::oxEnvOct (v);
        case kOxPorta: return map::oxPorta (v);
        case kOxFA: case kOxLA: return map::oxAttack (v);
        case kOxFD: case kOxFR: case kOxLD: case kOxLR: return map::oxDecay (v);
        case kSoTune: return map::soTuneCents (v);
        case kSoCresc: return map::soCresc (v);
        case kSoSust: return map::soSustain (v);
        case kCrAtk: return map::crAttack (v);
        case kCrDec: return map::crDecay (v);
        case kCrCont: return map::crContour (v);
        case kCrSust: return map::crSustain (v);
        case kCrVibSp: return map::crVibSpeed (v);
        default: return v;
    }
}

inline void formatValue (int p, float v, char* out, size_t size, bool longForm)
{
    out[0] = 0;
    if (p < 0 || p >= kNumParams) return;
    const ParamInfo& pi = kParams[p];
    const double r = realValue (p, v);
    char buf[64];
    switch (pi.unit)
    {
        case kUInst:
            std::snprintf (buf, sizeof (buf), "%s", longForm ? instrumentName (paramStep (p, v)) : instrumentShort (paramStep (p, v)));
            break;
        case kUMaster:
        {
            const double g = map::masterGain (v);
            if (g < 1e-5) std::snprintf (buf, sizeof (buf), "-inf");
            else std::snprintf (buf, sizeof (buf), longForm ? "%+.1f dB" : "%+.1f", 20.0 * std::log10 (g));
            break;
        }
        case kUDial:   std::snprintf (buf, sizeof (buf), "%.1f", v * 10.0); break;
        case kUSwitch: std::snprintf (buf, sizeof (buf), "%s", isOn (v) ? (longForm ? "ON" : "On") : (longForm ? "OFF" : "Off")); break;
        case kUChoice:
        {
            char c[40];
            choiceName (pi.choices, paramStep (p, v), c, sizeof (c));
            if (! longForm && std::strlen (c) > 8)
            {
                char* sp = std::strchr (c, ' ');
                if (sp) *sp = 0;          // first word: "WIDE", "NARROW", "SAMPLE"
            }
            std::snprintf (buf, sizeof (buf), "%s", c);
            break;
        }
        case kUSemis: std::snprintf (buf, sizeof (buf), longForm ? "%+.2f semitones" : "%+.2f", r); break;
        case kUCents: std::snprintf (buf, sizeof (buf), longForm ? "%+.0f cents" : "%+.0f c", r); break;
        case kUHz:
            if (r >= 1000.0) std::snprintf (buf, sizeof (buf), longForm ? "%.2f kHz" : "%.2fk", r / 1000.0);
            else if (r >= 100.0) std::snprintf (buf, sizeof (buf), longForm ? "%.0f Hz" : "%.0f", r);
            else if (r >= 10.0) std::snprintf (buf, sizeof (buf), longForm ? "%.1f Hz" : "%.1f", r);
            else std::snprintf (buf, sizeof (buf), longForm ? "%.2f Hz" : "%.2f", r);
            break;
        case kUSecs:
            if (r <= 0.0) std::snprintf (buf, sizeof (buf), "%s", longForm ? "OFF" : "Off");
            else if (r < 1.0) std::snprintf (buf, sizeof (buf), longForm ? "%.0f ms" : "%.0fms", r * 1000.0 < 1.0 ? 1.0 : r * 1000.0);
            else std::snprintf (buf, sizeof (buf), longForm ? "%.2f s" : "%.2fs", r);
            break;
        case kUOct:  std::snprintf (buf, sizeof (buf), longForm ? "%.1f octaves" : "%.1f oct", r); break;
        case kUPct:  std::snprintf (buf, sizeof (buf), longForm ? "%.0f %%" : "%.0f%%", r); break;
        case kUDrawbar: std::snprintf (buf, sizeof (buf), "%d", paramStep (p, v)); break;
        default: buf[0] = 0; break;
    }
    std::strncpy (out, buf, size - 1);
    out[size - 1] = 0;
}

} // namespace tbsyn

#endif
