// Engines.h
// The instrument models in TB Synth. Each engine receives its own
// parameters (setParam with the global Param index), MIDI notes, and renders
// mono audio. Plain C++98, no allocation after construction.
//
//   MoogEngine      Minimoog Model D: three oscillators, mixer, ladder filter,
//                   two contours, low-note priority, single triggering
//   OBXEngine       Oberheim OB-X: eight voices of two oscillators, 2-pole
//                   state-variable filter, ADSR x 2, one modulation LFO
//   SolinaEngine    Solina String Ensemble: divide-down top-octave generator,
//                   per-key crescendo / sustain, voice formants, the ensemble
//   WurliEngine     Wurlitzer 200A: struck steel reeds, electrostatic pickup,
//                   preamp, vibrato (the 200A's tremolo), amp and speakers
//   HammondEngine   Hammond B-3 upper manual: 91 tonewheels, nine drawbars,
//                   key contacts, percussion, scanner vibrato / chorus, the
//                   preamp, and a Leslie 122 (horn and drum, slow / fast / stop)
//   CembaletEngine  Hohner Cembalet: plucked steel reeds, capacitive pickup
//   ClavEngine      Hohner Clavinet D6: struck strings (waveguide), two pickups
//                   (C/D, A/B), BRILLIANT / TREBLE / MEDIUM / SOFT, mute slider
//   MultimanEngine  Crumar Multiman (1975): divide-down generator, keyboard split
//                   in two halves; brass through a transistor ladder with its
//                   own attack / decay, piano and clavichord, cello and violin
//                   through a three-line ensemble, sustain, vibrato, bass pedals
//   OmniEngine      ARP Omni: divide-down generator; violins, violas, cellos and
//                   bass, the synthesizer through one shared ladder VCF / ADSR,
//                   monophonic bass on the bottom 20 keys, string phasers

#ifndef TBSYNTH_ENGINES_H
#define TBSYNTH_ENGINES_H

#include "Common.h"
#include "SynthParams.h"

namespace tbsyn
{

class Engine
{
public:
    virtual ~Engine() {}
    virtual void prepare (double sampleRate) = 0;
    virtual void setParam (int p, float v) = 0;
    virtual void noteOn (int note, int velocity) = 0;
    virtual void noteOff (int note) = 0;
    virtual void allNotesOff() = 0;          // release everything (keys up)
    virtual void silence() = 0;              // stop at once, clear state
    virtual void pitchBend (double b) { (void) b; }      // -1 .. +1
    virtual void modWheel (double m) { (void) m; }       // 0 .. 1
    virtual void sustainPedal (bool down) { (void) down; }
    virtual void render (float* out, int n) = 0;          // writes n mono samples
};

//==============================================================================
class MoogEngine : public Engine
{
public:
    MoogEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void pitchBend (double b) { bend = b; }
    void modWheel (double m) { wheel = m; }
    void render (float* out, int n);

private:
    void updateEnvTimes();
    int  lowestHeld() const;
    double sr;
    float pv[kNumParams];
    bool held[128];
    int  heldCount;
    double target, pitch, glideK;
    double bend, wheel, wheelS;
    double ph[3];
    double lad[4];
    Envelope fenv, lenv;
    Noise noise;
    OnePole dcBlock;
    Smooth cutS, volS;
};

//==============================================================================
class OBXEngine : public Engine
{
public:
    enum { kVoices = 8 };
    OBXEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void pitchBend (double b) { bend = b; }
    void modWheel (double m) { wheel = m; }
    void render (float* out, int n);

private:
    struct Voice
    {
        int note;
        bool gate;
        unsigned age;
        double ph1, ph2, pitch, detune;
        Envelope fenv, lenv;
        SVF svf;
    };
    void updateEnvTimes();
    double sr;
    float pv[kNumParams];
    Voice v[kVoices];
    unsigned clock;
    int nextVoice;
    double lastPitch;
    double lfoPh, lfoHold, bend, wheel;
    Noise noise;
    OnePole dcBlock;
};

//==============================================================================
class SolinaEngine : public Engine
{
public:
    enum { kLowKey = 36, kKeys = 49 };      // C2 .. C6, 49 keys
    SolinaEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void render (float* out, int n);

private:
    void updateEnv();
    double sr;
    float pv[kNumParams];
    double topCount[12];          // top-octave generator, cycles (wrapped at 1024)
    Envelope env[kKeys];
    bool down[kKeys];
    Biquad violinHp, violinPk, violinLp, violaLp, violaPk, trumpetBp, trumpetHp, hornLp, celloLp, celloPk, bassLp, bassPk;
    Delay<8192> bbd;
    OnePole bbdLp1, bbdLp2;
    double lfoSlow, lfoFast;
    OnePole dcBlock;
};

//==============================================================================
// A struck or plucked cantilever reed: two bending modes with their own decays,
// shared by the Wurlitzer and the Cembalet (different pickups and voicing)
struct Reed
{
    int note;
    bool keyDown, damped;
    unsigned age;
    double ph1, ph2, ph3, dph1, dph2, dph3;
    double a1, a2, a3, k1, k2, k3;      // amplitudes and per-sample decay factors
    double kDamp1, kDamp2;              // when the damper is on the reed
    double attack, attackK;             // a short rise as the hammer / plectrum leaves
    double lastC;                       // pickup capacitance at the previous sample
    bool active;
};

class WurliEngine : public Engine
{
public:
    enum { kVoices = 20, kLowKey = 33, kHighKey = 96 };     // 64 keys, A1 .. C7
    WurliEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void sustainPedal (bool d);
    void render (float* out, int n);

private:
    double sr;
    float pv[kNumParams];
    Reed r[kVoices];
    unsigned clock;
    bool pedal;
    double tremPh;
    Biquad pre, spkHp, spkPk, spkLp;
    OnePole dcBlock;
    Smooth volS, vibS;
};

//==============================================================================
class HammondEngine : public Engine
{
public:
    enum { kWheels = 91, kLowKey = 36, kKeys = 61, kBars = 9, kPending = 1024 };
    HammondEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void modWheel (double m) { wheel = m; }
    void render (float* out, int n);

private:
    struct Contact { int key, bar; bool close; int when; };
    void rebuildTargets();
    int  wheelFor (int key, int bar) const;
    double sr;
    float pv[kNumParams];
    double wheelInc[kWheels + 1], wheelPh[kWheels + 1];
    double wheelAmp[kWheels + 1];
    double target[kWheels + 1], level[kWheels + 1];
    double percWheel[kWheels + 1];
    bool keyDown[kKeys];
    bool contact[kKeys][kBars];
    int keysHeld;
    Contact pending[kPending];
    int numPending;
    int sampleClock;
    double percEnv, percK;
    double smoothK;
    double barGain[kBars];
    Noise noise;
    // scanner vibrato
    Delay<1024> scanner;
    double scanPh;
    // preamp
    OnePole preDc, preLp;
    // Leslie 122
    double hornSpeed, drumSpeed, hornAngle, drumAngle;
    Biquad xoLo1, xoLo2, xoHi1, xoHi2, hornLp, drumLp;
    Delay<2048> hornLine, drumLine;
    double wheel;
    OnePole dcBlock;
    Smooth volS;
};

//==============================================================================
class CembaletEngine : public Engine
{
public:
    enum { kVoices = 20, kLowKey = 36, kHighKey = 96 };     // 61 keys, C2 .. C7
    CembaletEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void render (float* out, int n);

private:
    double sr;
    float pv[kNumParams];
    Reed r[kVoices];
    unsigned clock;
    Biquad hp, lp, body;
    OnePole dcBlock;
    Smooth volS;
};

//==============================================================================
class ClavEngine : public Engine
{
public:
    enum { kVoices = 12, kLowKey = 29, kHighKey = 88, kLen = 8192 };   // 60 keys, F1 .. E6
    ClavEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void render (float* out, int n);

private:
    struct Str
    {
        int note;
        bool keyDown, active;
        unsigned age;
        double len;          // loop delay in samples
        double lpState, gain, gainHeld, gainReleased, bright;
        double envOut;       // for idle detection
        int idle;
        int w;
        float buf[kLen];
    };
    void setLoop (Str& s);
    double sr;
    float pv[kNumParams];
    Str s[kVoices];
    unsigned clock;
    OnePole brill, treb, med, soft;
    OnePole dcBlock;
    Smooth volS;
    Noise noise;
};

//==============================================================================
class MultimanEngine : public Engine
{
public:
    enum { kLowKey = 29, kKeys = 61, kSplit = 60, kBassKeys = 13 };   // F1 .. F6; upper half from C4; bass on F1 .. F2
    MultimanEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void render (float* out, int n);

private:
    void updateEnv();
    int  lowestBassKey() const;
    double sr;
    float pv[kNumParams];
    double topCount[12];
    bool down[kKeys];
    Envelope strEnv[kKeys], pnoEnv[kKeys];
    double brassGate[kKeys];
    double gateUp, gateDown;
    Envelope brassEnv;
    double lad[4];
    // bass pedals (monophonic)
    int bassNote;
    double bassPh, bassLevel, bassTarget, bassK;
    OnePole bassLp1, bassLp2;
    // voicing
    Biquad celloLp, celloPk, violinHp, violinPk, violinLp, pianoLp, clavHp, clavPk;
    Delay<8192> ens;
    OnePole ensLp1, ensLp2;
    double lfoSlow, lfoFast, vibPh;
    OnePole dcBlock;
    Smooth masterS;
};

//==============================================================================
class OmniEngine : public Engine
{
public:
    enum { kLowKey = 36, kKeys = 49, kBassKeys = 20 };   // C2 .. C6; bass on C2 .. G3
    OmniEngine();
    void prepare (double sr);
    void setParam (int p, float v);
    void noteOn (int note, int vel);
    void noteOff (int note);
    void allNotesOff();
    void silence();
    void sustainPedal (bool down);
    void render (float* out, int n);

private:
    void updateEnv();
    void updateBass (bool newKey);
    void releaseKey (int k);
    int  lowestBassKey() const;
    bool anyHeld() const;
    double sr;
    float pv[kNumParams];
    double topCount[12];
    bool down[kKeys], held[kKeys], pedal;
    Envelope strEnv[kKeys];
    double synGate[kKeys];
    double gateUp;
    Envelope adsr;
    double lad[4];
    int bassNote;
    double bassLevel;
    bool bassHeld;
    double bassDecK, bassCutK, bassAtkK, bassTarget;
    bool bassRestrike;
    double synDown;
    double lfoPh;
    Biquad violinHp, violinPk, violinLp, violaLp, violaPk, celloLp, celloPk, sbassLp, synBassLp;
    Delay<1024> ens, cho;
    double ensPh[3], choPh[3];
    OnePole ensLp, dcBlock;
    Smooth masterS;
};

} // namespace tbsyn

#endif
