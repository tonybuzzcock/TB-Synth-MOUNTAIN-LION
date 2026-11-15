// Synth.h
// TB Synth's core: the seven instrument engines, the INSTRUMENT selector (an
// 8 ms fade out, the old instrument silenced, the new one faded in), MIDI
// (notes, pitch bend, modulation wheel, sustain pedal, all notes off) and the
// MASTER level. Mono out. Plain C++98; allocation only in the constructor.

#ifndef TBSYNTH_SYNTH_H
#define TBSYNTH_SYNTH_H

#include "Engines.h"

namespace tbsyn
{

class Synth
{
public:
    Synth();
    ~Synth();

    void prepare (double sampleRate);
    void setParam (int p, float v);
    float getParam (int p) const { return (p >= 0 && p < kNumParams) ? pv[p] : 0.0f; }
    void midi (unsigned char status, unsigned char d1, unsigned char d2);
    void allNotesOff();
    void process (float* out, int n);       // mono, replaces
    int instrument() const { return cur; }
    double peak() const { return peakLevel; }   // recent output peak (for a meter)

private:
    Synth (const Synth&);
    Synth& operator= (const Synth&);

    Engine* eng[kNumInstruments];
    float pv[kNumParams];
    double sr;
    int cur, want;
    double fade, fadeStep;
    double master, masterK;
    double peakLevel, peakK;
};

} // namespace tbsyn

#endif
