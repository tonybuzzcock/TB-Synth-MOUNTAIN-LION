// SynthParams.h
// The parameter table of TB Synth, shared by the DSP, the VST wrapper, the panel
// and the preset store, with the value mappings (knob position -> Hz, seconds,
// semitones...) used both for the sound and for the readout.
//
// Values are 0..1 like every VST parameter. Switches and selectors are stored as
// k / (steps - 1). Parameter names are at most 8 characters (VST 2.4); every
// name starts with the instrument's two-letter prefix so they stay unique and
// sort together in a host's generic list: Mg Minimoog Model D, Ox OB-X,
// So Solina String Ensemble, Wu Wurlitzer 200A, B3 Hammond B-3 (Ls its Leslie
// 122), Cb Hohner Cembalet, Cl Hohner Clavinet D6, Cr Crumar Multiman, Om ARP Omni.
//
// Plain C++98, header only.

#ifndef TBSYNTH_PARAMS_H
#define TBSYNTH_PARAMS_H

#include <cmath>

namespace tbsyn
{

enum Instrument { kMinimoog = 0, kOBX, kSolina, kWurlitzer, kHammond, kCembalet, kClavinet, kMultiman, kOmni, kSK30, kRhapsody, kNumInstruments };

enum Param
{
    // the whole plugin
    kInst = 0, kMaster,

    // Minimoog Model D: CONTROLLERS
    kMgTune, kMgGlide, kMgModMix, kMgOscMod, kMgO3Ctrl, kMgGlideOn, kMgDecay,
    // OSCILLATOR BANK
    kMgO1Rng, kMgO1Wave, kMgO2Rng, kMgO2Freq, kMgO2Wave, kMgO3Rng, kMgO3Freq, kMgO3Wave,
    // MIXER
    kMgO1Vol, kMgO1On, kMgO2Vol, kMgO2On, kMgO3Vol, kMgO3On, kMgNzVol, kMgNzOn, kMgNzCol,
    // MODIFIERS
    kMgFltMod, kMgKbd1, kMgKbd2, kMgCutoff, kMgEmph, kMgContAm,
    kMgFAtk, kMgFDec, kMgFSus, kMgLAtk, kMgLDec, kMgLSus,
    // OUTPUT
    kMgVol,

    // Oberheim OB-X
    kOxLfoRt, kOxLfoWv, kOxModDp, kOxModO1, kOxModO2, kOxModFl, kOxModPw,
    kOxO1Frq, kOxO2Frq, kOxO2Det, kOxPW, kOxO1Saw, kOxO1Pls, kOxO2Saw, kOxO2Pls, kOxSync, kOxXmod,
    kOxCut, kOxRes, kOxEnvAm, kOxNoise, kOxKbd, kOxPorta,
    kOxFA, kOxFD, kOxFS, kOxFR, kOxLA, kOxLD, kOxLS, kOxLR,
    kOxVol,

    // Solina String Ensemble
    kSoViolin, kSoViola, kSoTrump, kSoHorn, kSoCello, kSoBass,
    kSoTune, kSoCresc, kSoSust, kSoVol,

    // Wurlitzer 200A
    kWuVol, kWuVib,

    // Hammond B-3 (upper manual) + Leslie 122
    kB3Db1, kB3Db2, kB3Db3, kB3Db4, kB3Db5, kB3Db6, kB3Db7, kB3Db8, kB3Db9,
    kB3Perc, kB3PVol, kB3PDec, kB3PHarm, kB3VibOn, kB3VibSel, kB3Drive, kB3Vol,
    kLsOn, kLsSpeed,

    // Hohner Cembalet
    kCbVol,

    // Hohner Clavinet D6
    kClBrill, kClTreb, kClMed, kClSoft, kClCD, kClAB, kClMute, kClVol,

    // Crumar Multiman (1975): brass filter, the section volumes (lower / upper half
    // of the keyboard) and their ON tabs, sustain, vibrato, master and bass pedals
    kCrAtk, kCrDec, kCrEmph, kCrCont,
    kCrBrLo, kCrBrHi, kCrBrOnLo, kCrBrOnHi,
    kCrPnLo, kCrPnHi, kCrPiano, kCrClav,
    kCrCeLo, kCrCeHi, kCrViLo, kCrViHi, kCrCello, kCrViolin,
    kCrSust, kCrVibSp, kCrVibDp, kCrVibOn,
    kCrMaster, kCrBassV, kCrBassOn,

    // ARP Omni (model 2480): strings, synthesizer, bass, general
    kOmViolin, kOmViola, kOmCello, kOmBass, kOmSAtk, kOmSRel,
    kOmSy4, kOmSy8, kOmSyB8, kOmSyB16, kOmWave, kOmEnh, kOmChorus,
    kOmVcfF, kOmVcfR, kOmVcfEnv, kOmVcfLfo, kOmLfo,
    kOmA, kOmD, kOmS, kOmR,
    kOmMix, kOmBassV, kOmMaster,

    // Yamaha SK-30 Symphonic Ensemble: organ, poly-synth / strings, solo synthesizer, output
    kSkOrgSel, kSkO16, kSkO8, kSkO513, kSkO4, kSkO223, kSkO2, kSkO135, kSkO113, kSkO1,
    kSkPerc2, kSkPerc3, kSkPerc5, kSkPercDec, kSkOrgDec, kSkOrgSus, kSkOrgDS, kSkOrgBril, kSkOrgTrem, kSkOrgEns,
    kSkStrSel, kSkPolySel, kSkMode, kSkFeet, kSkWave, kSkCut, kSkRes, kSkEgDp, kSkEgA, kSkEgD,
    kSkPBril, kSkSlow, kSkVibDl, kSkVibSp, kSkVibDp, kSkPSus, kSkPSusT, kSkDetune, kSkPTrem, kSkPEns,
    kSkTrFast,
    kSkSoMode, kSkSoFeet, kSkSoWave, kSkSoPW, kSkSoPwm, kSkSoCut, kSkSoRes, kSkSoMod, kSkSoEgDp,
    kSkSoVca, kSkSoA, kSkSoD, kSkSoS, kSkSoR, kSkSoLfo, kSkSoTune, kSkSoPorta, kSkSoGlide,
    kSkOrgV, kSkPolyV, kSkStrV, kSkSoloV,

    // Elka Rhapsody 610: voices (the CANCELS), split, volumes for each half, decays, volume pedal
    kRhCello, kRhStrings, kRhPiano, kRhClav, kRhSplit,
    kRhVolStrLo, kRhVolStrHi, kRhVolPnLo, kRhVolPnHi,
    kRhDecCello, kRhDecStr, kRhDecPno, kRhMaster,

    kNumParams
};

// How a value is shown (the readout and the host's generic display)
enum Unit
{
    kUInst,         // instrument name
    kUMaster,       // dB, -inf at 0
    kUDial,         // 0.0 .. 10.0
    kUSwitch,       // ON / OFF
    kUChoice,       // a name from the parameter's choice list
    kUSemis,        // +/- semitones
    kUCents,        // +/- cents
    kUHz,           // Hz / kHz
    kUSecs,         // ms / s
    kUOct,          // octaves
    kUPct,          // per cent
    kUDrawbar       // 0 .. 8
};

struct ParamInfo
{
    const char* name;       // <= 8 characters, for the host
    const char* label;      // the readout's name for it
    int inst;               // -1 = the whole plugin
    int steps;              // 0 = continuous, else number of positions
    float def;
    int unit;
    const char* choices;    // "A|B|C" for kUChoice
};

// Moog range switch: LO, 32', 16', 8', 4', 2'
// Moog waveforms (osc 1 and 2; osc 3 has reverse saw in place of tri/saw)
const ParamInfo kParams[kNumParams] = {
    { "Instrmnt", "INSTRUMENT", -1, 16, 0.0f, kUInst, 0 },
    { "Master",   "MASTER", -1, 0, 0.75f, kUMaster, 0 },

    // Minimoog: a warm lead / bass to start from
    { "MgTune",   "TUNE",           kMinimoog, 0, 0.5f,  kUSemis, 0 },
    { "MgGlide",  "GLIDE",          kMinimoog, 0, 0.35f, kUSecs, 0 },
    { "MgModMix", "MODULATION MIX", kMinimoog, 0, 0.0f,  kUDial, 0 },
    { "MgOscMod", "OSC. MODULATION", kMinimoog, 2, 0.0f, kUSwitch, 0 },
    { "MgO3Ctrl", "OSC. 3 CONTROL", kMinimoog, 2, 1.0f,  kUSwitch, 0 },
    { "MgGlidOn", "GLIDE SWITCH",   kMinimoog, 2, 0.0f,  kUSwitch, 0 },
    { "MgDecay",  "DECAY SWITCH",   kMinimoog, 2, 1.0f,  kUSwitch, 0 },
    { "MgO1Rng",  "OSC 1 RANGE",    kMinimoog, 6, 0.4f,  kUChoice, "LO|32'|16'|8'|4'|2'" },
    { "MgO1Wave", "OSC 1 WAVEFORM", kMinimoog, 6, 0.4f,  kUChoice, "TRIANGLE|TRI/SAW|SAWTOOTH|SQUARE|WIDE PULSE|NARROW PULSE" },
    { "MgO2Rng",  "OSC 2 RANGE",    kMinimoog, 6, 0.4f,  kUChoice, "LO|32'|16'|8'|4'|2'" },
    { "MgO2Freq", "OSC 2 FREQUENCY", kMinimoog, 0, 0.506f, kUSemis, 0 },
    { "MgO2Wave", "OSC 2 WAVEFORM", kMinimoog, 6, 0.4f,  kUChoice, "TRIANGLE|TRI/SAW|SAWTOOTH|SQUARE|WIDE PULSE|NARROW PULSE" },
    { "MgO3Rng",  "OSC 3 RANGE",    kMinimoog, 6, 0.4f,  kUChoice, "LO|32'|16'|8'|4'|2'" },
    { "MgO3Freq", "OSC 3 FREQUENCY", kMinimoog, 0, 0.494f, kUSemis, 0 },
    { "MgO3Wave", "OSC 3 WAVEFORM", kMinimoog, 6, 0.6f,  kUChoice, "TRIANGLE|REV SAW|SAWTOOTH|SQUARE|WIDE PULSE|NARROW PULSE" },
    { "MgO1Vol",  "OSC 1 VOLUME",   kMinimoog, 0, 0.8f,  kUDial, 0 },
    { "MgO1On",   "OSC 1 ON",       kMinimoog, 2, 1.0f,  kUSwitch, 0 },
    { "MgO2Vol",  "OSC 2 VOLUME",   kMinimoog, 0, 0.8f,  kUDial, 0 },
    { "MgO2On",   "OSC 2 ON",       kMinimoog, 2, 1.0f,  kUSwitch, 0 },
    { "MgO3Vol",  "OSC 3 VOLUME",   kMinimoog, 0, 0.6f,  kUDial, 0 },
    { "MgO3On",   "OSC 3 ON",       kMinimoog, 2, 0.0f,  kUSwitch, 0 },
    { "MgNzVol",  "NOISE VOLUME",   kMinimoog, 0, 0.5f,  kUDial, 0 },
    { "MgNzOn",   "NOISE ON",       kMinimoog, 2, 0.0f,  kUSwitch, 0 },
    { "MgNzCol",  "NOISE",          kMinimoog, 2, 0.0f,  kUChoice, "WHITE|PINK" },
    { "MgFltMod", "FILTER MODULATION", kMinimoog, 2, 0.0f, kUSwitch, 0 },
    { "MgKbd1",   "KEYBOARD CONTROL 1", kMinimoog, 2, 1.0f, kUSwitch, 0 },
    { "MgKbd2",   "KEYBOARD CONTROL 2", kMinimoog, 2, 0.0f, kUSwitch, 0 },
    { "MgCutoff", "CUTOFF FREQUENCY", kMinimoog, 0, 0.46f, kUHz, 0 },
    { "MgEmph",   "EMPHASIS",       kMinimoog, 0, 0.3f,  kUDial, 0 },
    { "MgContAm", "AMOUNT OF CONTOUR", kMinimoog, 0, 0.5f, kUOct, 0 },
    { "MgFAtk",   "FILTER ATTACK",  kMinimoog, 0, 0.12f, kUSecs, 0 },
    { "MgFDec",   "FILTER DECAY",   kMinimoog, 0, 0.45f, kUSecs, 0 },
    { "MgFSus",   "FILTER SUSTAIN", kMinimoog, 0, 0.4f,  kUDial, 0 },
    { "MgLAtk",   "LOUDNESS ATTACK", kMinimoog, 0, 0.05f, kUSecs, 0 },
    { "MgLDec",   "LOUDNESS DECAY", kMinimoog, 0, 0.45f, kUSecs, 0 },
    { "MgLSus",   "LOUDNESS SUSTAIN", kMinimoog, 0, 0.8f, kUDial, 0 },
    { "MgVol",    "VOLUME",         kMinimoog, 0, 0.6f,  kUDial, 0 },

    // OB-X: a brassy pad to start from
    { "OxLfoRt",  "MOD RATE",       kOBX, 0, 0.45f, kUHz, 0 },
    { "OxLfoWv",  "MOD WAVEFORM",   kOBX, 3, 0.0f,  kUChoice, "SINE|SQUARE|SAMPLE & HOLD" },
    { "OxModDp",  "MOD DEPTH",      kOBX, 0, 0.0f,  kUDial, 0 },
    { "OxModO1",  "MOD > OSC 1",    kOBX, 2, 1.0f,  kUSwitch, 0 },
    { "OxModO2",  "MOD > OSC 2",    kOBX, 2, 1.0f,  kUSwitch, 0 },
    { "OxModFl",  "MOD > FILTER",   kOBX, 2, 0.0f,  kUSwitch, 0 },
    { "OxModPw",  "MOD > PULSE WIDTH", kOBX, 2, 0.0f, kUSwitch, 0 },
    { "OxO1Frq",  "OSC 1 FREQUENCY", kOBX, 0, 0.5f, kUSemis, 0 },
    { "OxO2Frq",  "OSC 2 FREQUENCY", kOBX, 0, 0.5f, kUSemis, 0 },
    { "OxO2Det",  "OSC 2 DETUNE",   kOBX, 0, 0.58f, kUCents, 0 },
    { "OxPW",     "PULSE WIDTH",    kOBX, 0, 0.3f,  kUPct, 0 },
    { "OxO1Saw",  "OSC 1 SAWTOOTH", kOBX, 2, 1.0f,  kUSwitch, 0 },
    { "OxO1Pls",  "OSC 1 PULSE",    kOBX, 2, 0.0f,  kUSwitch, 0 },
    { "OxO2Saw",  "OSC 2 SAWTOOTH", kOBX, 2, 1.0f,  kUSwitch, 0 },
    { "OxO2Pls",  "OSC 2 PULSE",    kOBX, 2, 0.0f,  kUSwitch, 0 },
    { "OxSync",   "SYNC",           kOBX, 2, 0.0f,  kUSwitch, 0 },
    { "OxXmod",   "X-MOD",          kOBX, 2, 0.0f,  kUSwitch, 0 },
    { "OxCut",    "FILTER FREQUENCY", kOBX, 0, 0.5f, kUHz, 0 },
    { "OxRes",    "RESONANCE",      kOBX, 0, 0.25f, kUDial, 0 },
    { "OxEnvAm",  "FILTER MODULATION", kOBX, 0, 0.45f, kUOct, 0 },
    { "OxNoise",  "NOISE",          kOBX, 0, 0.0f,  kUDial, 0 },
    { "OxKbd",    "FILTER KEYBOARD", kOBX, 2, 1.0f, kUSwitch, 0 },
    { "OxPorta",  "PORTAMENTO",     kOBX, 0, 0.0f,  kUSecs, 0 },
    { "OxFA",     "FILTER ATTACK",  kOBX, 0, 0.3f,  kUSecs, 0 },
    { "OxFD",     "FILTER DECAY",   kOBX, 0, 0.5f,  kUSecs, 0 },
    { "OxFS",     "FILTER SUSTAIN", kOBX, 0, 0.5f,  kUDial, 0 },
    { "OxFR",     "FILTER RELEASE", kOBX, 0, 0.5f,  kUSecs, 0 },
    { "OxLA",     "LOUDNESS ATTACK", kOBX, 0, 0.2f, kUSecs, 0 },
    { "OxLD",     "LOUDNESS DECAY", kOBX, 0, 0.5f,  kUSecs, 0 },
    { "OxLS",     "LOUDNESS SUSTAIN", kOBX, 0, 0.85f, kUDial, 0 },
    { "OxLR",     "LOUDNESS RELEASE", kOBX, 0, 0.5f, kUSecs, 0 },
    { "OxVol",    "VOLUME",         kOBX, 0, 0.6f,  kUDial, 0 },

    // Solina
    { "SoViolin", "VIOLIN",         kSolina, 2, 1.0f, kUSwitch, 0 },
    { "SoViola",  "VIOLA",          kSolina, 2, 1.0f, kUSwitch, 0 },
    { "SoTrump",  "TRUMPET",        kSolina, 2, 0.0f, kUSwitch, 0 },
    { "SoHorn",   "HORN",           kSolina, 2, 0.0f, kUSwitch, 0 },
    { "SoCello",  "CELLO",          kSolina, 2, 0.0f, kUSwitch, 0 },
    { "SoBass",   "CONTRABASS",     kSolina, 2, 0.0f, kUSwitch, 0 },
    { "SoTune",   "TUNING",         kSolina, 0, 0.5f, kUCents, 0 },
    { "SoCresc",  "CRESCENDO",      kSolina, 0, 0.35f, kUSecs, 0 },
    { "SoSust",   "SUSTAIN",        kSolina, 0, 0.5f, kUSecs, 0 },
    { "SoVol",    "VOLUME",         kSolina, 0, 0.7f, kUDial, 0 },

    // Wurlitzer 200A
    { "WuVol",    "VOLUME",         kWurlitzer, 0, 0.7f, kUDial, 0 },
    { "WuVib",    "VIBRATO",        kWurlitzer, 0, 0.0f, kUDial, 0 },

    // Hammond B-3: 888000000, percussion on (3rd, fast, soft), vibrato C3, Leslie slow
    { "B3Db16",   "16' DRAWBAR",    kHammond, 9, 1.0f, kUDrawbar, 0 },
    { "B3Db513",  "5 1/3' DRAWBAR", kHammond, 9, 1.0f, kUDrawbar, 0 },
    { "B3Db8",    "8' DRAWBAR",     kHammond, 9, 1.0f, kUDrawbar, 0 },
    { "B3Db4",    "4' DRAWBAR",     kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Db223",  "2 2/3' DRAWBAR", kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Db2",    "2' DRAWBAR",     kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Db135",  "1 3/5' DRAWBAR", kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Db113",  "1 1/3' DRAWBAR", kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Db1",    "1' DRAWBAR",     kHammond, 9, 0.0f, kUDrawbar, 0 },
    { "B3Perc",   "PERCUSSION",     kHammond, 2, 1.0f, kUSwitch, 0 },
    { "B3PVol",   "PERCUSSION VOLUME", kHammond, 2, 1.0f, kUChoice, "NORMAL|SOFT" },
    { "B3PDec",   "PERCUSSION DECAY", kHammond, 2, 1.0f, kUChoice, "SLOW|FAST" },
    { "B3PHarm",  "PERCUSSION HARMONIC", kHammond, 2, 1.0f, kUChoice, "SECOND|THIRD" },
    { "B3VibOn",  "VIBRATO",        kHammond, 2, 1.0f, kUSwitch, 0 },
    { "B3VibSel", "VIBRATO / CHORUS", kHammond, 6, 1.0f, kUChoice, "V1|C1|V2|C2|V3|C3" },
    { "B3Drive",  "DRIVE",          kHammond, 0, 0.35f, kUDial, 0 },
    { "B3Vol",    "EXPRESSION",     kHammond, 0, 0.8f, kUDial, 0 },
    { "LsOn",     "LESLIE",         kHammond, 2, 1.0f, kUSwitch, 0 },
    { "LsSpeed",  "LESLIE SPEED",   kHammond, 3, 0.5f, kUChoice, "STOP|SLOW|FAST" },

    // Cembalet
    { "CbVol",    "VOLUME",         kCembalet, 0, 0.7f, kUDial, 0 },

    // Clavinet D6: TREBLE on, C/B (individual, bridge pickup)
    { "ClBrill",  "BRILLIANT",      kClavinet, 2, 0.0f, kUSwitch, 0 },
    { "ClTreb",   "TREBLE",         kClavinet, 2, 1.0f, kUSwitch, 0 },
    { "ClMed",    "MEDIUM",         kClavinet, 2, 0.0f, kUSwitch, 0 },
    { "ClSoft",   "SOFT",           kClavinet, 2, 0.0f, kUSwitch, 0 },
    { "ClCD",     "PICKUP C / D",   kClavinet, 2, 0.0f, kUChoice, "C|D" },
    { "ClAB",     "PICKUP A / B",   kClavinet, 2, 1.0f, kUChoice, "A|B" },
    { "ClMute",   "MUTE",           kClavinet, 0, 0.0f, kUDial, 0 },
    { "ClVol",    "VOLUME",         kClavinet, 0, 0.7f, kUDial, 0 },

    // Crumar Multiman: strings on (cello and violin), the rest ready to bring in
    { "CrAttack", "BRASS ATTACK",   kMultiman, 0, 0.3f, kUSecs, 0 },
    { "CrDecay",  "BRASS DECAY",    kMultiman, 0, 0.5f, kUSecs, 0 },
    { "CrEmph",   "BRASS EMPHASIS", kMultiman, 0, 0.3f, kUDial, 0 },
    { "CrContur", "BRASS CONTOUR",  kMultiman, 0, 0.45f, kUHz, 0 },
    { "CrBrLo",   "BRASS VOLUME LOWER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrBrHi",   "BRASS VOLUME UPPER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrBrOnLo", "BRASS LOWER",    kMultiman, 2, 0.0f, kUSwitch, 0 },
    { "CrBrOnHi", "BRASS UPPER",    kMultiman, 2, 0.0f, kUSwitch, 0 },
    { "CrPnLo",   "PIANO VOLUME LOWER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrPnHi",   "PIANO VOLUME UPPER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrPiano",  "PIANO",          kMultiman, 2, 0.0f, kUSwitch, 0 },
    { "CrClav",   "CLAVICHORD",     kMultiman, 2, 0.0f, kUSwitch, 0 },
    { "CrCeLo",   "CELLO VOLUME LOWER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrCeHi",   "CELLO VOLUME UPPER", kMultiman, 0, 0.5f, kUDial, 0 },
    { "CrViLo",   "VIOLIN VOLUME LOWER", kMultiman, 0, 0.5f, kUDial, 0 },
    { "CrViHi",   "VIOLIN VOLUME UPPER", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrCello",  "CELLO",          kMultiman, 2, 1.0f, kUSwitch, 0 },
    { "CrViolin", "VIOLIN",         kMultiman, 2, 1.0f, kUSwitch, 0 },
    { "CrSust",   "SUSTAIN LENGTH", kMultiman, 0, 0.4f, kUSecs, 0 },
    { "CrVibSpd", "VIBRATO SPEED",  kMultiman, 0, 0.5f, kUHz, 0 },
    { "CrVibDep", "VIBRATO DEPTH",  kMultiman, 0, 0.3f, kUDial, 0 },
    { "CrVibOn",  "VIBRATO",        kMultiman, 2, 0.0f, kUSwitch, 0 },
    { "CrMaster", "MASTER VOLUME",  kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrBassV",  "BASS PEDAL VOLUME", kMultiman, 0, 0.7f, kUDial, 0 },
    { "CrBassOn", "BASS",           kMultiman, 2, 0.0f, kUSwitch, 0 },

    // ARP Omni: violins and violas on, synthesizer ready to bring in with MIX
    { "OmViolin", "VIOLINS 4'",     kOmni, 2, 1.0f, kUSwitch, 0 },
    { "OmViola",  "VIOLAS 8'",      kOmni, 2, 1.0f, kUSwitch, 0 },
    { "OmCello",  "CELLOS 8'",      kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmBass",   "BASS 16'",       kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmSAtk",   "STRING ATTACK",  kOmni, 0, 0.25f, kUSecs, 0 },
    { "OmSRel",   "STRING RELEASE", kOmni, 0, 0.45f, kUSecs, 0 },
    { "OmSy4",    "SYNTHESIZER 4'", kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmSy8",    "SYNTHESIZER 8'", kOmni, 2, 1.0f, kUSwitch, 0 },
    { "OmSyB8",   "SYNTH BASS 8'",  kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmSyB16",  "SYNTH BASS 16'", kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmWave",   "WAVEFORM",       kOmni, 2, 0.0f, kUChoice, "SAW|PULSE" },
    { "OmEnh",    "WAVEFORM ENHANCEMENT", kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmChorus", "CHORUS PHASER",  kOmni, 2, 0.0f, kUSwitch, 0 },
    { "OmVcfF",   "VCF FREQUENCY",  kOmni, 0, 0.45f, kUHz, 0 },
    { "OmVcfR",   "VCF RESONANCE",  kOmni, 0, 0.2f, kUDial, 0 },
    { "OmVcfEnv", "VCF ADSR",       kOmni, 0, 0.5f, kUDial, 0 },
    { "OmVcfLfo", "VCF LFO",        kOmni, 0, 0.0f, kUDial, 0 },
    { "OmLfo",    "LFO SPEED",      kOmni, 0, 0.4f, kUHz, 0 },
    { "OmA",      "ATTACK",         kOmni, 0, 0.1f, kUSecs, 0 },
    { "OmD",      "DECAY",          kOmni, 0, 0.45f, kUSecs, 0 },
    { "OmS",      "SUSTAIN",        kOmni, 0, 0.6f, kUDial, 0 },
    { "OmR",      "RELEASE",        kOmni, 0, 0.4f, kUSecs, 0 },
    { "OmMix",    "STRING / SYNTH MIX", kOmni, 0, 0.35f, kUDial, 0 },
    { "OmBassV",  "BASS VOLUME",    kOmni, 0, 0.7f, kUDial, 0 },
    { "OmMaster", "MASTER VOLUME",  kOmni, 0, 0.7f, kUDial, 0 },

    // Yamaha SK-30: power-on presets ORGAN 1, POLY-SYNTH 1, STRING 1; organ up, the rest down
    { "SkOrgSel", "ORGAN",          kSK30, 5, 0.25f, kUChoice, "OFF|1|2|3|MAN" },
    { "SkO16",    "ORGAN 16'",      kSK30, 0, 0.8f, kUDial, 0 },
    { "SkO8",     "ORGAN 8'",       kSK30, 0, 0.8f, kUDial, 0 },
    { "SkO513",   "ORGAN 5 1/3'",   kSK30, 0, 0.0f, kUDial, 0 },
    { "SkO4",     "ORGAN 4'",       kSK30, 0, 0.6f, kUDial, 0 },
    { "SkO223",   "ORGAN 2 2/3'",   kSK30, 0, 0.0f, kUDial, 0 },
    { "SkO2",     "ORGAN 2'",       kSK30, 0, 0.4f, kUDial, 0 },
    { "SkO135",   "ORGAN 1 3/5'",   kSK30, 0, 0.0f, kUDial, 0 },
    { "SkO113",   "ORGAN 1 1/3'",   kSK30, 0, 0.0f, kUDial, 0 },
    { "SkO1",     "ORGAN 1'",       kSK30, 0, 0.0f, kUDial, 0 },
    { "SkPerc2",  "PERCUSSION 2ND", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkPerc3",  "PERCUSSION 3RD", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkPerc5",  "PERCUSSION 5TH", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkPrcDec", "PERC. DECAY TIME", kSK30, 0, 0.5f, kUSecs, 0 },
    { "SkOrgDec", "ORGAN DECAY",    kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkOrgSus", "ORGAN SUSTAIN",  kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkOrgDS",  "ORGAN DECAY / SUSTAIN", kSK30, 0, 0.5f, kUSecs, 0 },
    { "SkOrgBri", "ORGAN BRILLIANCE", kSK30, 0, 0.5f, kUDial, 0 },
    { "SkOrgTrm", "ORGAN TREMOLO",  kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkOrgEns", "ORGAN ENSEMBLE", kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkStrSel", "STRINGS",        kSK30, 4, 1.0f / 3.0f, kUChoice, "OFF|1|2|3" },
    { "SkPlySel", "POLY-SYNTH",     kSK30, 5, 0.25f, kUChoice, "OFF|1|2|3|MAN" },
    { "SkMode",   "MODE",           kSK30, 2, 0.0f, kUChoice, "I|II" },
    { "SkFeet",   "FEET",           kSK30, 3, 0.5f, kUChoice, "16'|8'|4'" },
    { "SkWave",   "WAVE",           kSK30, 2, 0.0f, kUChoice, "SAW|SQUARE" },
    { "SkCut",    "CUT OFF FREQ",   kSK30, 0, 0.4f, kUHz, 0 },
    { "SkRes",    "RESONANCE",      kSK30, 0, 0.15f, kUDial, 0 },
    { "SkEgDp",   "EG DEPTH",       kSK30, 0, 0.5f, kUDial, 0 },
    { "SkEgA",    "EG ATTACK TIME", kSK30, 0, 0.35f, kUSecs, 0 },
    { "SkEgD",    "EG DECAY TIME",  kSK30, 0, 0.5f, kUSecs, 0 },
    { "SkPBril",  "POLY BRILLIANCE", kSK30, 0, 0.5f, kUDial, 0 },
    { "SkSlow",   "SLOW ATTACK",    kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkVibDl",  "VIBRATO DELAY",  kSK30, 0, 0.3f, kUSecs, 0 },
    { "SkVibSp",  "VIBRATO SPEED",  kSK30, 0, 0.5f, kUHz, 0 },
    { "SkVibDp",  "VIBRATO DEPTH",  kSK30, 0, 0.0f, kUDial, 0 },
    { "SkPSus",   "POLY SUSTAIN",   kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkPSusT",  "POLY SUSTAIN TIME", kSK30, 0, 0.5f, kUSecs, 0 },
    { "SkDetune", "POLY DETUNE",    kSK30, 0, 0.6f, kUCents, 0 },
    { "SkPTrem",  "POLY TREMOLO",   kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkPEns",   "POLY ENSEMBLE",  kSK30, 2, 0.0f, kUSwitch, 0 },
    { "SkTrFast", "TREMOLO SPEED",  kSK30, 2, 0.0f, kUChoice, "SLOW|FAST" },
    { "SkSoMode", "SOLO MODE",      kSK30, 2, 0.0f, kUChoice, "NORMAL|BASS" },
    { "SkSoFeet", "SOLO FEET",      kSK30, 6, 0.6f, kUChoice, "64'|32'|16'|8'|5 1/3'|4'" },
    { "SkSoWave", "SOLO WAVE",      kSK30, 0, 0.0f, kUDial, 0 },
    { "SkSoPW",   "SOLO PULSE WIDTH", kSK30, 0, 0.0f, kUPct, 0 },
    { "SkSoPwm",  "SOLO PW MOD",    kSK30, 3, 0.0f, kUChoice, "MAN|LFO|EG" },
    { "SkSoCut",  "SOLO CUT OFF FREQ", kSK30, 0, 0.55f, kUHz, 0 },
    { "SkSoRes",  "SOLO RESONANCE", kSK30, 0, 0.2f, kUDial, 0 },
    { "SkSoMod",  "SOLO MODULATION", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkSoEgDp", "SOLO EG DEPTH",  kSK30, 0, 0.4f, kUDial, 0 },
    { "SkSoVca",  "SOLO VCA",       kSK30, 3, 1.0f, kUChoice, "HOLD|GATE|EG" },
    { "SkSoA",    "SOLO ATTACK",    kSK30, 0, 0.1f, kUSecs, 0 },
    { "SkSoD",    "SOLO DECAY",     kSK30, 0, 0.4f, kUSecs, 0 },
    { "SkSoS",    "SOLO SUSTAIN",   kSK30, 0, 0.7f, kUDial, 0 },
    { "SkSoR",    "SOLO RELEASE",   kSK30, 0, 0.3f, kUSecs, 0 },
    { "SkSoLfo",  "SOLO LFO SPEED", kSK30, 0, 0.35f, kUHz, 0 },
    { "SkSoTune", "SOLO TUNE",      kSK30, 0, 500.0f / 1200.0f, kUCents, 0 },
    { "SkSoPort", "PORTAMENTO",     kSK30, 0, 0.0f, kUSecs, 0 },
    { "SkSoGlid", "GLIDE DEPTH",    kSK30, 0, 0.5f, kUSemis, 0 },
    { "SkOrgV",   "ORGAN VOLUME",   kSK30, 0, 0.7f, kUDial, 0 },
    { "SkPolyV",  "POLY-SYNTH VOLUME", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkStrV",   "STRINGS VOLUME", kSK30, 0, 0.0f, kUDial, 0 },
    { "SkSoloV",  "SOLO VOLUME",    kSK30, 0, 0.0f, kUDial, 0 },

    // Elka Rhapsody 610: strings on, no split
    { "RhCello",  "VIOLONCELLO",    kRhapsody, 2, 0.0f, kUSwitch, 0 },
    { "RhString", "STRINGS",        kRhapsody, 2, 1.0f, kUSwitch, 0 },
    { "RhPiano",  "PIANO",          kRhapsody, 2, 0.0f, kUSwitch, 0 },
    { "RhClav",   "CLAVICHORD",     kRhapsody, 2, 0.0f, kUSwitch, 0 },
    { "RhSplit",  "KEYBOARD SPLIT", kRhapsody, 3, 0.0f, kUChoice, "OFF|2 OCT|3 OCT" },
    { "RhVStrLo", "STRINGS VOLUME LOWER", kRhapsody, 0, 0.7f, kUDial, 0 },
    { "RhVStrHi", "STRINGS VOLUME UPPER", kRhapsody, 0, 0.7f, kUDial, 0 },
    { "RhVPnLo",  "PIANO VOLUME LOWER",   kRhapsody, 0, 0.7f, kUDial, 0 },
    { "RhVPnHi",  "PIANO VOLUME UPPER",   kRhapsody, 0, 0.7f, kUDial, 0 },
    { "RhDCello", "VIOLONCELLO DECAY",    kRhapsody, 0, 0.4f, kUSecs, 0 },
    { "RhDStr",   "STRINGS DECAY",  kRhapsody, 0, 0.4f, kUSecs, 0 },
    { "RhDPno",   "PIANO / CLAV DECAY",   kRhapsody, 0, 0.45f, kUSecs, 0 },
    { "RhMaster", "VOLUME PEDAL",   kRhapsody, 0, 0.7f, kUDial, 0 },
};

inline const char* instrumentName (int k)
{
    static const char* const n[kNumInstruments] = {
        "MINIMOOG MODEL D", "OBERHEIM OB-X", "SOLINA STRING ENSEMBLE", "WURLITZER 200A",
        "HAMMOND B-3", "HOHNER CEMBALET", "HOHNER CLAVINET D6", "CRUMAR MULTIMAN", "ARP OMNI", "YAMAHA SK-30", "ELKA RHAPSODY 610" };
    return (k >= 0 && k < kNumInstruments) ? n[k] : "";
}
inline const char* instrumentShort (int k)    // <= 8 characters
{
    static const char* const n[kNumInstruments] = { "Minimoog", "OB-X", "Solina", "Wurli", "B-3", "Cembalet", "Clavinet", "Multiman", "Omni", "SK-30", "Rhapsody" };
    return (k >= 0 && k < kNumInstruments) ? n[k] : "";
}

// The INSTRUMENT value: a fixed scale with room for 16 instruments (k / 15), so adding
// an instrument never moves the ones saved in sessions and presets
const int kInstSlots = 16;

// Value helpers
inline bool  isOn (float v) { return v >= 0.5f; }
inline int   stepOf (float v, int steps)
{
    if (steps < 2) return 0;
    int k = (int) std::floor (v * (steps - 1) + 0.5f);
    return k < 0 ? 0 : (k > steps - 1 ? steps - 1 : k);
}
inline float stepValue (int k, int steps) { return steps < 2 ? 0.0f : (float) k / (float) (steps - 1); }
inline int   paramStep (int p, float v) { return stepOf (v, kParams[p].steps); }
inline float instValue (int k) { return stepValue (k, kInstSlots); }
inline int   instIndex (float v) { const int k = stepOf (v, kInstSlots); return k < kNumInstruments ? k : kNumInstruments - 1; }

inline double expMap (double v, double lo, double hi) { return lo * std::pow (hi / lo, v); }

// ---------------------------------------------------------------------------
// Mappings. Ranges marked "spec" come from the Minimoog Model D owner's manual
// specifications; the rest are estimates (see README).
namespace map
{
    inline double masterGain (double v) { return (v / 0.75) * (v / 0.75); }       // 0.75 = unity, 1.0 = +5 dB

    // Minimoog
    inline double mgTuneSemis (double v)  { return (v - 0.5) * 5.0; }             // +/- 2.5 semitones (estimate)
    inline double mgGlide (double v)      { return expMap (v, 0.001, 10.0); }     // 1 ms .. 10 s (spec)
    inline double mgOscFreqSemis (double v) { return (v - 0.5) * 14.0; }         // +/- 7 semitones (estimate)
    inline double mgCutoff (double v)     { return expMap (v, 10.0, 20000.0); }   // 10 Hz .. 20 kHz (spec)
    inline double mgContour (double v)    { return 4.0 * v; }                     // 0 .. 4 octaves (spec)
    inline double mgAttack (double v)     { return expMap (v, 0.001, 10.0); }     // 1 ms .. 10 s (spec)
    inline double mgDecay (double v)      { return expMap (v, 0.004, 35.0); }     // 4 ms .. 35 s (spec)

    // OB-X
    inline double oxLfo (double v)        { return expMap (v, 0.1, 25.0); }
    inline double oxOscSemis (double v)   { return (v - 0.5) * 48.0; }            // +/- 2 octaves
    inline double oxDetuneCents (double v) { return (v - 0.5) * 50.0; }           // +/- 25 cents
    inline double oxPulseWidth (double v) { return 0.5 - 0.45 * v; }              // 50 % .. 5 %
    inline double oxCutoff (double v)     { return expMap (v, 20.0, 20000.0); }
    inline double oxEnvOct (double v)     { return 7.0 * v; }
    inline double oxAttack (double v)     { return expMap (v, 0.001, 10.0); }
    inline double oxDecay (double v)      { return expMap (v, 0.002, 15.0); }
    inline double oxPorta (double v)      { return v < 0.01 ? 0.0 : expMap (v, 0.005, 5.0); }

    // Solina
    inline double soTuneCents (double v)  { return (v - 0.5) * 100.0; }           // +/- 50 cents
    inline double soCresc (double v)      { return expMap (v, 0.008, 4.0); }
    inline double soSustain (double v)    { return expMap (v, 0.05, 6.0); }

    // Crumar Multiman (estimates: the schematic fixes the circuit, not these ranges)
    inline double crAttack (double v)     { return expMap (v, 0.004, 3.0); }
    inline double crDecay (double v)      { return expMap (v, 0.02, 6.0); }
    inline double crContour (double v)    { return expMap (v, 60.0, 12000.0); }
    inline double crSustain (double v)    { return expMap (v, 0.08, 5.0); }
    inline double crVibSpeed (double v)   { return expMap (v, 2.5, 9.0); }

    // ARP Omni (LFO from the service manual; the rest estimates)
    inline double omStrAttack (double v)  { return expMap (v, 0.005, 3.0); }
    inline double omStrRelease (double v) { return expMap (v, 0.05, 6.0); }
    inline double omVcf (double v)        { return expMap (v, 20.0, 16000.0); }
    inline double omLfo (double v)        { return expMap (v, 0.1, 20.0); }      // about 0.1 - 20 Hz (spec)
    inline double omAttack (double v)     { return expMap (v, 0.002, 5.0); }
    inline double omDecay (double v)      { return expMap (v, 0.01, 8.0); }

    // Yamaha SK-30 (owner's manual ranges where given, the rest estimates)
    inline double skDS (double v)         { return expMap (v, 0.03, 1.6); }      // decay / sustain lever 0.03 - 1.6 s (spec)
    inline double skPercDec (double v)    { return expMap (v, 0.05, 0.8); }      // percussion decay, 0.8 s max (spec)
    inline double skCut (double v)        { return expMap (v, 40.0, 12000.0); }
    inline double skEgA (double v)        { return expMap (v, 0.003, 3.0); }
    inline double skEgD (double v)        { return expMap (v, 0.03, 10.0); }
    inline double skVibDelay (double v)   { return 3.2 * v; }                    // 0 - 3.2 s (spec)
    inline double skVibSpeed (double v)   { return 3.0 + 6.0 * v; }
    inline double skDetune (double v)     { return (v - 0.5) * 118.0; }          // +/- 59 cents (spec)
    inline double skSoCut (double v)      { return expMap (v, 30.0, 15000.0); }
    inline double skSoA (double v)        { return expMap (v, 0.003, 3.0); }     // 3 ms - 3 s (spec)
    inline double skSoDR (double v)       { return expMap (v, 0.03, 30.0); }     // 30 ms - 30 s (spec)
    inline double skSoLfo (double v)      { return expMap (v, 0.1, 100.0); }     // 0.1 - 100 Hz (spec)
    inline double skSoTune (double v)     { return v * 1200.0 - 500.0; }         // -500 .. +700 cents (spec)
    inline double skSoPW (double v)       { return 0.5 + 0.35 * v; }             // 50 - 85 % (spec)
    inline double skPorta (double v)      { return v < 0.01 ? 0.0 : expMap (v, 0.01, 3.0); }
    inline double skGlide (double v)      { return (v - 0.5) * 24.0; }           // semitones from above / below

    // Elka Rhapsody 610 (estimates: the panel's sliders are unmarked)
    inline double rhRelease (double v)    { return expMap (v, 0.05, 6.0); }      // strings / cello DECAY (release)
    inline double rhPianoDecay (double v) { return expMap (v, 0.25, 8.0); }      // piano / clavichord DECAY

    // Hammond drawbars: each step about 3 dB, 0 = off
    inline double drawbarGain (int k)     { return k <= 0 ? 0.0 : std::pow (10.0, -3.0 * (8 - k) / 20.0); }
}

} // namespace tbsyn

#endif
