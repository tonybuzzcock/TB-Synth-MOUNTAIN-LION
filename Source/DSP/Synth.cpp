// Synth.cpp

#include "Synth.h"

#include <algorithm>

namespace tbsyn
{

const SineTable& sineTable()
{
    static SineTable t;
    return t;
}

Synth::Synth()
    : sr (44100.0), cur (0), want (0), fade (1.0), fadeStep (0.003), master (1.0), masterK (0.01), peakLevel (0.0), peakK (0.9999)
{
    sineTable();            // build the table now, not in the audio thread
    eng[kMinimoog] = new MoogEngine();
    eng[kOBX] = new OBXEngine();
    eng[kSolina] = new SolinaEngine();
    eng[kWurlitzer] = new WurliEngine();
    eng[kHammond] = new HammondEngine();
    eng[kCembalet] = new CembaletEngine();
    eng[kClavinet] = new ClavEngine();
    eng[kMultiman] = new MultimanEngine();
    eng[kOmni] = new OmniEngine();
    eng[kSK30] = new SK30Engine();
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    cur = want = instIndex (pv[kInst]);
    master = map::masterGain (pv[kMaster]);
}

Synth::~Synth()
{
    for (int i = 0; i < kNumInstruments; ++i) delete eng[i];
}

void Synth::prepare (double s)
{
    sr = s > 1000.0 ? s : 44100.0;
    for (int i = 0; i < kNumInstruments; ++i)
    {
        for (int p = 0; p < kNumParams; ++p)
            if (kParams[p].inst == i) eng[i]->setParam (p, pv[p]);
        eng[i]->prepare (sr);
    }
    fadeStep = 1.0 / (0.008 * sr);
    masterK = 1.0 - std::exp (-1.0 / (0.01 * sr));
    peakK = std::exp (-1.0 / (0.3 * sr));
    cur = want;
    fade = 1.0;
    master = map::masterGain (pv[kMaster]);
    peakLevel = 0.0;
}

void Synth::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    if (! (v == v)) return;
    v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
    pv[p] = v;
    if (p == kInst) { want = instIndex (v); return; }
    const int i = kParams[p].inst;
    if (i >= 0 && i < kNumInstruments) eng[i]->setParam (p, v);
}

void Synth::allNotesOff()
{
    for (int i = 0; i < kNumInstruments; ++i) { eng[i]->allNotesOff(); eng[i]->sustainPedal (false); }
}

void Synth::midi (unsigned char status, unsigned char d1, unsigned char d2)
{
    Engine* e = eng[want];      // while switching, play the new instrument
    const int type = status & 0xF0;
    switch (type)
    {
        case 0x90:
        case 0x80:
            if (type == 0x90 && d2 > 0) e->noteOn (d1 & 0x7F, d2 & 0x7F);
            else for (int i = 0; i < kNumInstruments; ++i) eng[i]->noteOff (d1 & 0x7F);   // velocity 0 = note off
            break;
        case 0xE0:
        {
            const int v = ((d2 & 0x7F) << 7) | (d1 & 0x7F);
            const double b = (v - 8192) / 8192.0;
            for (int i = 0; i < kNumInstruments; ++i) eng[i]->pitchBend (b);
            break;
        }
        case 0xB0:
            if (d1 == 1) { for (int i = 0; i < kNumInstruments; ++i) eng[i]->modWheel ((d2 & 0x7F) / 127.0); }
            else if (d1 == 64) { for (int i = 0; i < kNumInstruments; ++i) eng[i]->sustainPedal (d2 >= 64); }
            else if (d1 == 120) { for (int i = 0; i < kNumInstruments; ++i) eng[i]->silence(); }
            else if (d1 == 123 || d1 == 121) allNotesOff();
            break;
        default: break;
    }
}

void Synth::process (float* out, int n)
{
    if (n <= 0) return;
    eng[cur]->render (out, n);
    const double mt = map::masterGain (pv[kMaster]);
    for (int i = 0; i < n; ++i)
    {
        if (want != cur)
        {
            fade -= fadeStep;
            if (fade <= 0.0)
            {
                fade = 0.0;
                eng[cur]->silence();
                cur = want;
                // the rest of this block comes from the new instrument
                eng[cur]->render (out + i, n - i);
            }
        }
        else if (fade < 1.0) fade = std::min (1.0, fade + fadeStep);
        master += (mt - master) * masterK;
        double y = out[i] * fade * master;
        if (! (y == y) || y > 1e4 || y < -1e4)
        {
            // never pass on a broken value: reset the instrument
            eng[cur]->silence();
            y = 0.0;
        }
        const double a = std::fabs (y);
        peakLevel = a > peakLevel ? a : peakLevel * peakK;
        out[i] = (float) y;
    }
}

} // namespace tbsyn
