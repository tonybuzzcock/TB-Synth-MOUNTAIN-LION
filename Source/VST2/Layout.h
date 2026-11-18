// Layout.h
// Where every control sits on TB Synth's panel (face coordinates, y down).
// Shared by PanelView.mm and Tests/editor_test.mm, so the test clicks exactly
// where the panel draws. Each instrument has its own page, laid out like the
// original's front panel; the INSTRUMENT menu, MASTER, the presets and the
// readout are on every page. Plain C++98 data.

#ifndef TBSYNTH_LAYOUT_H
#define TBSYNTH_LAYOUT_H

#include "../DSP/SynthParams.h"

namespace tbsyn
{
namespace layout
{

enum CtlType
{
    kKnob,          // knurled knob, 300 degrees
    kSelector,      // rotary switch with marked positions
    kRocker,        // small rocker, its engaged half lit
    kTab,           // a tab / tablet that lights when down
    kDrawbar,       // Hammond drawbar, pulled down 0..8
    kSlide,         // 2- or 3-position slide switch, lever lit
    kVSlider        // vertical slider with a cream cap
};

// Knob caps (the M-50's colours): grey cap, red cap (levels), orange skirt (frequency)
enum Style { kGrey = 0, kRed, kOrange, kWave = 10, kLever = 20, kSmallTab = 21 };   // kSmallTab: a smaller tab   // kWave: selector positions drawn as waveforms; kLever: compact slider

struct Ctl
{
    int param;
    int type;
    double x, y, r;         // centre; r = knob radius (sliders: half travel)
    const char* label;      // lettering on the panel ("" = none; '\n' for two lines)
    const char* lo;         // knob scale end labels, or a tab's face lettering
    const char* hi;
    int style;
};

struct Section { int inst; double x, y, w, h; const char* title; };

// Rows of the main control area
const double R1 = 170.0, R2 = 300.0, R3 = 430.0;
// the SK-30's three lever rows
const double S1 = 160.0, S2 = 295.0, S3 = 430.0, kLevR = 32.0;

const Ctl kCtls[] = {
    // ------------------------------------------------------------ Minimoog Model D
    // CONTROLLERS
    { kMgTune,    kKnob,     122, R1, 22, "TUNE", "-", "+", kGrey },
    { kMgGlide,   kKnob,      72, R2, 22, "GLIDE", "0", "10", kGrey },
    { kMgModMix,  kKnob,     172, R2, 22, "MODULATION\nMIX", "OSC3", "NOISE", kGrey },
    { kMgOscMod,  kRocker,    37, R3, 0, "OSC.\nMOD.", "", "", 0 },
    { kMgO3Ctrl,  kRocker,    87, R3, 0, "OSC. 3\nCONTROL", "", "", 0 },
    { kMgGlideOn, kRocker,   142, R3, 0, "GLIDE", "", "", 0 },
    { kMgDecay,   kRocker,   197, R3, 0, "DECAY", "", "", 0 },
    // OSCILLATOR BANK
    { kMgO1Rng,   kSelector, 300, R1, 18, "RANGE", "", "", 0 },
    { kMgO1Wave,  kSelector, 500, R1, 18, "WAVEFORM", "", "", kWave },
    { kMgO2Rng,   kSelector, 300, R2, 18, "RANGE", "", "", 0 },
    { kMgO2Freq,  kKnob,     400, R2, 22, "FREQUENCY", "-7", "+7", kGrey },
    { kMgO2Wave,  kSelector, 500, R2, 18, "WAVEFORM", "", "", kWave },
    { kMgO3Rng,   kSelector, 300, R3, 18, "RANGE", "", "", 0 },
    { kMgO3Freq,  kKnob,     400, R3, 22, "FREQUENCY", "-7", "+7", kGrey },
    { kMgO3Wave,  kSelector, 500, R3, 18, "WAVEFORM", "", "", kWave + 1 },
    // MIXER
    { kMgO1Vol,   kKnob,     620, R1, 22, "OSC. 1", "0", "10", kGrey },
    { kMgO1On,    kRocker,   675, R1, 0, "", "", "", 0 },
    { kMgO2Vol,   kKnob,     620, R2, 22, "OSC. 2", "0", "10", kGrey },
    { kMgO2On,    kRocker,   675, R2, 0, "", "", "", 0 },
    { kMgO3Vol,   kKnob,     620, R3, 22, "OSC. 3", "0", "10", kGrey },
    { kMgO3On,    kRocker,   675, R3, 0, "", "", "", 0 },
    { kMgNzVol,   kKnob,     728, R1, 22, "NOISE", "0", "10", kGrey },
    { kMgNzOn,    kRocker,   728, R2 - 10, 0, "", "", "", 0 },
    { kMgNzCol,   kSlide,    728, R3, 0, "", "", "", 0 },
    // MODIFIERS
    { kMgFltMod,  kRocker,   835, R1, 0, "FILTER\nMOD.", "", "", 0 },
    { kMgKbd1,    kRocker,   835, R2, 0, "KEYBD.\nCTRL 1", "", "", 0 },
    { kMgKbd2,    kRocker,   835, R3, 0, "KEYBD.\nCTRL 2", "", "", 0 },
    { kMgCutoff,  kKnob,     910, R1, 22, "CUTOFF\nFREQUENCY", "-5", "+5", kOrange },
    { kMgEmph,    kKnob,     995, R1, 22, "EMPHASIS", "0", "10", kGrey },
    { kMgContAm,  kKnob,    1075, R1, 22, "AMOUNT OF\nCONTOUR", "0", "10", kGrey },
    { kMgFAtk,    kKnob,     910, R2, 22, "ATTACK", "0", "10", kGrey },
    { kMgFDec,    kKnob,     995, R2, 22, "DECAY", "0", "10", kGrey },
    { kMgFSus,    kKnob,    1075, R2, 22, "SUSTAIN", "0", "10", kGrey },
    { kMgLAtk,    kKnob,     910, R3, 22, "ATTACK", "0", "10", kGrey },
    { kMgLDec,    kKnob,     995, R3, 22, "DECAY", "0", "10", kGrey },
    { kMgLSus,    kKnob,    1075, R3, 22, "SUSTAIN", "0", "10", kGrey },
    // OUTPUT
    { kMgVol,     kKnob,    1174, R1, 22, "VOLUME", "0", "10", kRed },

    // ------------------------------------------------------------ Oberheim OB-X
    { kOxLfoRt,   kKnob,      72, R1, 22, "RATE", "0", "10", kGrey },
    { kOxModDp,   kKnob,     192, R1, 22, "DEPTH", "0", "10", kGrey },
    { kOxLfoWv,   kSelector, 132, R2, 18, "WAVEFORM", "", "", kWave + 2 },
    { kOxModO1,   kRocker,    42, R3, 0, "OSC 1", "", "", 0 },
    { kOxModO2,   kRocker,   102, R3, 0, "OSC 2", "", "", 0 },
    { kOxModFl,   kRocker,   162, R3, 0, "FILTER", "", "", 0 },
    { kOxModPw,   kRocker,   222, R3, 0, "PULSE\nWIDTH", "", "", 0 },
    { kOxO1Frq,   kKnob,     330, R1, 22, "OSC 1\nFREQUENCY", "-2", "+2", kGrey },
    { kOxO2Frq,   kKnob,     455, R1, 22, "OSC 2\nFREQUENCY", "-2", "+2", kGrey },
    { kOxO2Det,   kKnob,     580, R1, 22, "OSC 2\nDETUNE", "-", "+", kGrey },
    { kOxPW,      kKnob,     330, R2, 22, "PULSE\nWIDTH", "50", "5", kGrey },
    { kOxSync,    kRocker,   455, R2, 0, "SYNC", "", "", 0 },
    { kOxXmod,    kRocker,   580, R2, 0, "X-MOD", "", "", 0 },
    { kOxO1Saw,   kRocker,   300, R3, 0, "OSC 1\nSAW", "", "", 0 },
    { kOxO1Pls,   kRocker,   360, R3, 0, "OSC 1\nPULSE", "", "", 0 },
    { kOxO2Saw,   kRocker,   520, R3, 0, "OSC 2\nSAW", "", "", 0 },
    { kOxO2Pls,   kRocker,   580, R3, 0, "OSC 2\nPULSE", "", "", 0 },
    { kOxCut,     kKnob,     715, R1, 22, "FREQUENCY", "0", "10", kOrange },
    { kOxRes,     kKnob,     835, R1, 22, "RESONANCE", "0", "10", kGrey },
    { kOxEnvAm,   kKnob,     715, R2, 22, "MODULATION", "0", "10", kGrey },
    { kOxNoise,   kKnob,     835, R2, 22, "NOISE", "0", "10", kGrey },
    { kOxKbd,     kRocker,   715, R3, 0, "KEYBOARD", "", "", 0 },
    { kOxPorta,   kKnob,     835, R3, 22, "PORTAMENTO", "0", "10", kGrey },
    { kOxFA,      kKnob,     950, R1, 21, "ATTACK", "0", "10", kGrey },
    { kOxFD,      kKnob,    1030, R1, 21, "DECAY", "0", "10", kGrey },
    { kOxFS,      kKnob,    1110, R1, 21, "SUSTAIN", "0", "10", kGrey },
    { kOxFR,      kKnob,    1190, R1, 21, "RELEASE", "0", "10", kGrey },
    { kOxLA,      kKnob,     950, R2, 21, "ATTACK", "0", "10", kGrey },
    { kOxLD,      kKnob,    1030, R2, 21, "DECAY", "0", "10", kGrey },
    { kOxLS,      kKnob,    1110, R2, 21, "SUSTAIN", "0", "10", kGrey },
    { kOxLR,      kKnob,    1190, R2, 21, "RELEASE", "0", "10", kGrey },
    { kOxVol,     kKnob,    1190, R3, 22, "VOLUME", "0", "10", kRed },

    // ------------------------------------------------------------ Solina String Ensemble
    { kSoTune,    kKnob,     160, 285, 26, "TUNING", "-", "+", kGrey },
    { kSoViolin,  kTab,      345, 285, 0, "VIOLIN", "", "", 0 },
    { kSoViola,   kTab,      430, 285, 0, "VIOLA", "", "", 0 },
    { kSoTrump,   kTab,      515, 285, 0, "TRUMPET", "", "", 0 },
    { kSoHorn,    kTab,      600, 285, 0, "HORN", "", "", 0 },
    { kSoCello,   kTab,      685, 285, 0, "CELLO", "", "", 0 },
    { kSoBass,    kTab,      770, 285, 0, "CONTRA-\nBASS", "", "", 0 },
    { kSoCresc,   kVSlider,  895, 290, 105, "CRESCENDO", "", "", 0 },
    { kSoSust,    kVSlider,  995, 290, 105, "SUSTAIN", "", "", 0 },
    { kSoVol,     kVSlider, 1145, 290, 105, "VOLUME", "", "", 0 },

    // ------------------------------------------------------------ Wurlitzer 200A
    { kWuVol,     kKnob,     530, 290, 32, "VOLUME", "0", "10", kRed },
    { kWuVib,     kKnob,     710, 290, 32, "VIBRATO", "0", "10", kGrey },

    // ------------------------------------------------------------ Hammond B-3 + Leslie 122
    { kB3Db1,     kDrawbar,   90, 0, 0, "16'", "", "", 0 },
    { kB3Db2,     kDrawbar,  149, 0, 0, "5 1/3'", "", "", 0 },
    { kB3Db3,     kDrawbar,  208, 0, 0, "8'", "", "", 1 },
    { kB3Db4,     kDrawbar,  267, 0, 0, "4'", "", "", 1 },
    { kB3Db5,     kDrawbar,  326, 0, 0, "2 2/3'", "", "", 2 },
    { kB3Db6,     kDrawbar,  385, 0, 0, "2'", "", "", 1 },
    { kB3Db7,     kDrawbar,  444, 0, 0, "1 3/5'", "", "", 2 },
    { kB3Db8,     kDrawbar,  503, 0, 0, "1 1/3'", "", "", 2 },
    { kB3Db9,     kDrawbar,  562, 0, 0, "1'", "", "", 1 },
    { kB3Perc,    kTab,      690, 190, 0, "PERCUSSION", "ON", "", 0 },
    { kB3PVol,    kTab,      755, 190, 0, "VOLUME", "SOFT", "", 0 },
    { kB3PDec,    kTab,      820, 190, 0, "DECAY", "FAST", "", 0 },
    { kB3PHarm,   kTab,      885, 190, 0, "HARMONIC", "THIRD", "", 0 },
    { kB3VibOn,   kTab,      710, 400, 0, "VIBRATO", "ON", "", 0 },
    { kB3VibSel,  kSelector, 845, 400, 20, "VIBRATO &\nCHORUS", "", "", 0 },
    { kLsOn,      kTab,      995, 190, 0, "LESLIE", "ON", "", 0 },
    { kLsSpeed,   kSlide,   1135, 190, 0, "SPEED", "", "", 0 },
    { kB3Drive,   kKnob,    1015, 400, 24, "DRIVE", "0", "10", kGrey },
    { kB3Vol,     kKnob,    1150, 400, 24, "EXPRESSION", "0", "10", kRed },

    // ------------------------------------------------------------ Hohner Cembalet
    { kCbVol,     kKnob,     620, 290, 32, "VOLUME", "0", "10", kRed },

    // ------------------------------------------------------------ Hohner Clavinet D6
    { kClBrill,   kTab,      265, 285, 0, "BRILLIANT", "", "", 0 },
    { kClTreb,    kTab,      345, 285, 0, "TREBLE", "", "", 0 },
    { kClMed,     kTab,      425, 285, 0, "MEDIUM", "", "", 0 },
    { kClSoft,    kTab,      505, 285, 0, "SOFT", "", "", 0 },
    { kClCD,      kSlide,    645, 285, 0, "", "", "", 0 },
    { kClAB,      kSlide,    755, 285, 0, "", "", "", 0 },
    { kClVol,     kKnob,     905, 285, 30, "VOLUME", "0", "10", kRed },
    { kClMute,    kVSlider, 1080, 290, 105, "MUTE", "", "", 0 },

    // ------------------------------------------------------------ Crumar Multiman
    // BRASS: the filter on top, the two halves' volumes, their ON tabs
    { kCrAtk,     kKnob,      62, R1, 20, "ATTACK", "0", "10", kGrey },
    { kCrDec,     kKnob,     142, R1, 20, "DECAY", "0", "10", kGrey },
    { kCrEmph,    kKnob,     222, R1, 20, "EMPHASIS", "0", "10", kGrey },
    { kCrCont,    kKnob,     302, R1, 20, "CONTOUR", "0", "10", kOrange },
    { kCrBrLo,    kKnob,     107, R2, 22, "VOLUME\nLOWER", "0", "10", kGrey },
    { kCrBrHi,    kKnob,     247, R2, 22, "VOLUME\nUPPER", "0", "10", kGrey },
    { kCrBrOnLo,  kTab,      107, R3, 0, "LOWER", "ON", "", 0 },
    { kCrBrOnHi,  kTab,      247, R3, 0, "UPPER", "ON", "", 0 },
    // PIANO / CLAVICHORD
    { kCrPnLo,    kKnob,     405, R2, 22, "VOLUME\nLOWER", "0", "10", kGrey },
    { kCrPnHi,    kKnob,     505, R2, 22, "VOLUME\nUPPER", "0", "10", kGrey },
    { kCrPiano,   kTab,      405, R3, 0, "PIANO", "ON", "", 0 },
    { kCrClav,    kTab,      505, R3, 0, "CLAVICH.", "ON", "", 0 },
    // STRINGS
    { kCrCeLo,    kKnob,     613, R2, 20, "CELLO\nLOWER", "0", "10", kGrey },
    { kCrCeHi,    kKnob,     693, R2, 20, "CELLO\nUPPER", "0", "10", kGrey },
    { kCrViLo,    kKnob,     777, R2, 20, "VIOLIN\nLOWER", "0", "10", kGrey },
    { kCrViHi,    kKnob,     857, R2, 20, "VIOLIN\nUPPER", "0", "10", kGrey },
    { kCrCello,   kTab,      653, R3, 0, "CELLO", "ON", "", 0 },
    { kCrViolin,  kTab,      817, R3, 0, "VIOLIN", "ON", "", 0 },
    // SUSTAIN and VIBRATO
    { kCrSust,    kKnob,    1001, R1, 22, "SUSTAIN\nLENGTH", "0", "10", kGrey },
    { kCrVibSp,   kKnob,     956, R2, 20, "SPEED", "0", "10", kGrey },
    { kCrVibDp,   kKnob,    1046, R2, 20, "DEPTH", "0", "10", kGrey },
    { kCrVibOn,   kTab,     1001, R3, 0, "VIBRATO", "ON", "", 0 },
    // OUTPUT
    { kCrMaster,  kKnob,    1164, R1, 22, "MASTER", "0", "10", kRed },
    { kCrBassV,   kKnob,    1164, R2, 22, "BASS\nPEDAL", "0", "10", kGrey },
    { kCrBassOn,  kTab,     1164, R3, 0, "BASS", "ON", "", 0 },

    // ------------------------------------------------------------ ARP Omni
    // STRINGS
    { kOmViolin,  kTab,       57, R1, 0, "VIOLINS\n4'", "", "", 0 },
    { kOmViola,   kTab,      142, R1, 0, "VIOLAS\n8'", "", "", 0 },
    { kOmCello,   kTab,      227, R1, 0, "CELLOS\n8'", "", "", 0 },
    { kOmBass,    kTab,      312, R1, 0, "BASS\n16'", "", "", 0 },
    { kOmSAtk,    kKnob,     102, R2 + 20, 22, "ATTACK", "0", "10", kGrey },
    { kOmSRel,    kKnob,     267, R2 + 20, 22, "RELEASE", "0", "10", kGrey },
    // SYNTHESIZER
    { kOmSy4,     kTab,      425, R1, 0, "4'", "", "", 0 },
    { kOmSy8,     kTab,      490, R1, 0, "8'", "", "", 0 },
    { kOmSyB8,    kTab,      555, R1, 0, "BASS\n8'", "", "", 0 },
    { kOmSyB16,   kTab,      620, R1, 0, "BASS\n16'", "", "", 0 },
    { kOmWave,    kSlide,    735, R1, 0, "WAVEFORM", "", "", 0 },
    { kOmEnh,     kTab,      845, R1, 0, "ENHANCE", "", "", 0 },
    { kOmVcfF,    kKnob,     430, R2, 20, "FREQUENCY", "0", "10", kOrange },
    { kOmVcfR,    kKnob,     510, R2, 20, "RESONANCE", "0", "10", kGrey },
    { kOmVcfEnv,  kKnob,     590, R2, 20, "ADSR", "0", "10", kGrey },
    { kOmVcfLfo,  kKnob,     670, R2, 20, "LFO", "0", "10", kGrey },
    { kOmLfo,     kKnob,     760, R2, 20, "LFO\nSPEED", "0", "10", kGrey },
    { kOmChorus,  kTab,      845, R2, 0, "CHORUS\nPHASER", "", "", 0 },
    { kOmA,       kKnob,     430, R3, 20, "ATTACK", "0", "10", kGrey },
    { kOmD,       kKnob,     510, R3, 20, "DECAY", "0", "10", kGrey },
    { kOmS,       kKnob,     590, R3, 20, "SUSTAIN", "0", "10", kGrey },
    { kOmR,       kKnob,     670, R3, 20, "RELEASE", "0", "10", kGrey },
    // GENERAL
    { kOmMix,     kKnob,    1068, R1, 22, "STRING / SYNTH\nMIX", "STR", "SYN", kOrange },
    { kOmBassV,   kKnob,     998, R2 + 20, 22, "BASS\nVOLUME", "0", "10", kGrey },
    { kOmMaster,  kKnob,    1138, R2 + 20, 22, "MASTER", "0", "10", kRed },

    // ------------------------------------------------------------ Yamaha SK-30
    // ORGAN
    { kSkOrgSel, kSelector,   66, S1, 16, "ORGAN", "", "", 0 },
    { kSkOrgDec, kTab,       150, S1, 0, "DECAY", "", "", kSmallTab },
    { kSkOrgSus, kTab,       205, S1, 0, "SUSTAIN", "", "", kSmallTab },
    { kSkOrgDS, kVSlider, 256, S1, kLevR, "D / S\nTIME", "", "", kLever },
    { kSkOrgBril, kVSlider, 296, S1, kLevR, "BRILL-\nIANCE", "", "", kLever },
    { kSkO16, kVSlider, 40, S2, kLevR, "16'", "", "", kLever },
    { kSkO8, kVSlider, 78, S2, kLevR, "8'", "", "", kLever },
    { kSkO513, kVSlider, 116, S2, kLevR, "5 1/3'", "", "", kLever },
    { kSkO4, kVSlider, 154, S2, kLevR, "4'", "", "", kLever },
    { kSkO223, kVSlider, 192, S2, kLevR, "2 2/3'", "", "", kLever },
    { kSkO2, kVSlider, 230, S2, kLevR, "2'", "", "", kLever },
    { kSkO135, kVSlider, 268, S2, kLevR, "1 3/5'", "", "", kLever },
    { kSkO113, kVSlider, 306, S2, kLevR, "1 1/3'", "", "", kLever },
    { kSkO1, kVSlider, 344, S2, kLevR, "1'", "", "", kLever },
    { kSkPerc2, kVSlider, 40, S3, kLevR, "2ND", "", "", kLever },
    { kSkPerc3, kVSlider, 78, S3, kLevR, "3RD", "", "", kLever },
    { kSkPerc5, kVSlider, 116, S3, kLevR, "5TH", "", "", kLever },
    { kSkPercDec, kVSlider, 154, S3, kLevR, "DECAY\nTIME", "", "", kLever },
    { kSkOrgTrem, kTab,      222, S3, 0, "TREMOLO", "", "", kSmallTab },
    { kSkOrgEns, kTab,       278, S3, 0, "ENSEMBLE", "", "", kSmallTab },
    { kSkTrFast, kSlide,     340, S3, 0, "SPEED", "", "", 0 },
    // POLY-SYNTH / STRINGS
    { kSkStrSel, kSelector,  470, S1, 16, "STRINGS", "", "", 0 },
    { kSkPolySel, kSelector, 590, S1, 16, "POLY-SYNTH", "", "", 0 },
    { kSkMode,   kSlide,     680, S1, 0, "MODE", "", "", 0 },
    { kSkFeet,   kSelector,  805, S1, 16, "FEET", "", "", 0 },
    { kSkWave,   kSlide,     786, S3, 0, "WAVE", "", "", 0 },
    { kSkCut, kVSlider, 452, S2, kLevR, "CUT OFF\nFREQ", "", "", kLever },
    { kSkRes, kVSlider, 490, S2, kLevR, "RESO-\nNANCE", "", "", kLever },
    { kSkEgDp, kVSlider, 528, S2, kLevR, "EG\nDEPTH", "", "", kLever },
    { kSkEgA, kVSlider, 566, S2, kLevR, "ATTACK", "", "", kLever },
    { kSkEgD, kVSlider, 604, S2, kLevR, "DECAY", "", "", kLever },
    { kSkPBril, kVSlider, 660, S2, kLevR, "BRILL-\nIANCE", "", "", kLever },
    { kSkSlow,   kTab,       718, S2, 0, "SLOW\nATTACK", "", "", kSmallTab },
    { kSkPSus,   kTab,       776, S2, 0, "SUSTAIN", "", "", kSmallTab },
    { kSkPSusT, kVSlider, 826, S2, kLevR, "SUSTAIN\nTIME", "", "", kLever },
    { kSkVibDl, kVSlider, 452, S3, kLevR, "DELAY", "", "", kLever },
    { kSkVibSp, kVSlider, 490, S3, kLevR, "SPEED", "", "", kLever },
    { kSkVibDp, kVSlider, 528, S3, kLevR, "DEPTH", "", "", kLever },
    { kSkDetune, kVSlider, 580, S3, kLevR, "DETUNE", "", "", kLever },
    { kSkPTrem,  kTab,       640, S3, 0, "TREMOLO", "", "", kSmallTab },
    { kSkPEns,   kTab,       700, S3, 0, "ENSEMBLE", "", "", kSmallTab },
    // SOLO SYNTHESIZER
    { kSkSoFeet, kSelector,  904, S1, 16, "FEET", "", "", 0 },
    { kSkSoMode, kTab,       974, S1, 0, "MANUAL\nBASS", "", "", kSmallTab },
    { kSkSoPwm,  kSlide,     1052, S1, 0, "PW MOD", "", "", 0 },
    { kSkSoVca,  kSelector,  1186, S1, 16, "VCA", "", "", 0 },
    { kSkSoWave, kVSlider, 893, S2, kLevR, "WAVE", "", "", kLever },
    { kSkSoPW, kVSlider, 931, S2, kLevR, "PW", "", "", kLever },
    { kSkSoCut, kVSlider, 969, S2, kLevR, "CUT OFF\nFREQ", "", "", kLever },
    { kSkSoRes, kVSlider, 1007, S2, kLevR, "RESO-\nNANCE", "", "", kLever },
    { kSkSoMod, kVSlider, 1045, S2, kLevR, "MODU-\nLATION", "", "", kLever },
    { kSkSoEgDp, kVSlider, 1083, S2, kLevR, "EG\nDEPTH", "", "", kLever },
    { kSkSoLfo, kVSlider, 1121, S2, kLevR, "LFO\nSPEED", "", "", kLever },
    { kSkSoTune, kVSlider, 1159, S2, kLevR, "SOLO\nTUNE", "", "", kLever },
    { kSkSoPorta, kVSlider, 1197, S2, kLevR, "PORTA-\nMENTO", "", "", kLever },
    { kSkSoA, kVSlider, 893, S3, kLevR, "ATTACK", "", "", kLever },
    { kSkSoD, kVSlider, 931, S3, kLevR, "DECAY", "", "", kLever },
    { kSkSoS, kVSlider, 969, S3, kLevR, "SUSTAIN", "", "", kLever },
    { kSkSoR, kVSlider, 1007, S3, kLevR, "RELEASE", "", "", kLever },
    { kSkSoGlide, kVSlider, 1045, S3, kLevR, "GLIDE\nDEPTH", "", "", kLever },
    { kSkOrgV, kVSlider, 1097, S3, kLevR, "ORGAN", "", "", kLever },
    { kSkPolyV, kVSlider, 1135, S3, kLevR, "POLY", "", "", kLever },
    { kSkStrV, kVSlider, 1173, S3, kLevR, "STRINGS", "", "", kLever },
    { kSkSoloV, kVSlider, 1211, S3, kLevR, "SOLO", "", "", kLever },

    // ------------------------------------------------------------ every page
    { kMaster,    kKnob,     714, 592, 17, "MASTER", "", "", kRed },
};
const int kNumCtls = (int) (sizeof (kCtls) / sizeof (kCtls[0]));

const Section kSections[] = {
    { kMinimoog,   12, 76, 222, 424, "CONTROLLERS" },
    { kMinimoog,  242, 76, 318, 424, "OSCILLATOR BANK" },
    { kMinimoog,  568, 76, 222, 424, "MIXER" },
    { kMinimoog,  798, 76, 314, 424, "MODIFIERS" },
    { kMinimoog, 1120, 76, 108, 424, "OUTPUT" },
    { kOBX,        12, 76, 252, 424, "MODULATION" },
    { kOBX,       272, 76, 368, 424, "OSCILLATORS" },
    { kOBX,       648, 76, 252, 424, "FILTER" },
    { kOBX,       908, 76, 320, 424, "ENVELOPES" },
    { kSolina,     12, 120, 290, 330, "TUNING" },
    { kSolina,    310, 120, 500, 330, "ORCHESTRA" },
    { kSolina,    818, 120, 252, 330, "ENVELOPE" },
    { kSolina,   1078, 120, 150, 330, "OUTPUT" },
    { kWurlitzer, 400, 140, 440, 300, "ELECTRONIC PIANO" },
    { kHammond,    12, 76, 628, 424, "UPPER MANUAL DRAWBARS" },
    { kHammond,   648, 76, 284, 214, "PERCUSSION" },
    { kHammond,   648, 298, 284, 202, "VIBRATO" },
    { kHammond,   940, 76, 288, 214, "LESLIE 122" },
    { kHammond,   940, 298, 288, 202, "PREAMP" },
    { kCembalet,  500, 140, 240, 300, "CEMBALET" },
    { kClavinet,  200, 140, 360, 300, "TONE" },
    { kClavinet,  580, 140, 240, 300, "PICKUPS" },
    { kClavinet,  828, 140, 154, 300, "OUTPUT" },
    { kClavinet,  990, 140, 180, 300, "MUTE" },
    { kMultiman,   12, 76, 330, 424, "BRASS" },
    { kMultiman,  350, 76, 210, 424, "PIANO" },
    { kMultiman,  568, 76, 334, 424, "STRINGS" },
    { kMultiman,  910, 76, 182, 424, "SUSTAIN  VIBRATO" },
    { kMultiman, 1100, 76, 128, 424, "OUTPUT" },
    { kOmni,       12, 76, 360, 424, "STRINGS" },
    { kOmni,      380, 76, 520, 424, "SYNTHESIZER" },
    { kOmni,      908, 76, 320, 424, "GENERAL" },
    { kSK30,       12, 76, 400, 424, "ORGAN" },
    { kSK30,      420, 76, 440, 424, "POLY-SYNTH / STRINGS" },
    { kSK30,      868, 76, 360, 424, "SOLO SYNTHESIZER" },
};
const int kNumSections = (int) (sizeof (kSections) / sizeof (kSections[0]));

// Sub-headings across a row (instrument, x centre, y, text)
struct Caption { int inst; double x, y; const char* text; };
const Caption kCaptions[] = {
    { kMinimoog,  400, R1 - 2, "OSCILLATOR - 1" },
    { kMinimoog,  992, R2 - 62, "FILTER CONTOUR" },
    { kMinimoog,  992, R3 - 62, "LOUDNESS CONTOUR" },
    { kOBX,       132, R3 - 52, "MODULATION TO" },
    { kOBX,       440, R3 - 52, "WAVEFORMS" },
    { kOBX,      1070, R1 - 62, "FILTER ENVELOPE" },
    { kOBX,      1070, R2 - 62, "LOUDNESS ENVELOPE" },
    { kMultiman,  182, R1 - 62, "BRASS FILTER" },
    { kOmni,      550, R2 - 58, "VCF" },
    { kOmni,      550, R3 - 58, "ADSR" },
    { kSK30,      192, S2 - 55, "TONE LEVERS" },
    { kSK30,       97, S3 - 52, "PERCUSSIVE" },
    { kSK30,      528, S2 - 55, "MANUAL VOICE" },
    { kSK30,      490, S3 - 52, "VIBRATO" },
    { kSK30,     1045, S2 - 55, "VCO  /  VCF  /  LFO" },
    { kSK30,      969, S3 - 52, "EG" },
    { kSK30,     1154, S3 - 52, "VOLUME" },
};
const int kNumCaptions = (int) (sizeof (kCaptions) / sizeof (kCaptions[0]));

// The top bar: title, INSTRUMENT menu with step arrows, level LED
const double kMenuX = 300.0, kMenuY = 14.0, kMenuW = 420.0, kMenuH = 36.0, kArrowW = 36.0;
const double kLedX = 1196.0, kLedY = 32.0;

// Drawbars: slot top, handle travel per step
const double kBarTop = 118.0, kBarStep = 34.0, kBarHandleH = 30.0, kBarW = 40.0;
inline double drawbarY (int step) { return kBarTop + 18.0 + step * kBarStep; }   // handle centre

// Plugin strip
const double kStripTop = 530.0;
const double kLogoX = 14.0, kLogoY = 548.0, kLogoW = 120.0, kLogoH = 70.0;
const double kDefaultX = 146.0, kDefaultY = 576.0, kDefaultW = 92.0, kDefaultH = 32.0;   // this instrument back to its defaults
const double kSaveX = 246.0, kSaveY = 576.0, kSaveW = 74.0, kSaveH = 32.0;
const double kPresetX = 368.0, kPresetY = 572.0, kPresetW = 264.0, kPresetH = 40.0;   // the preset menu
const double kPrevX = 330.0, kNextX = 640.0, kStepY = 576.0, kStepW = 32.0, kStepH = 32.0;   // previous / next preset arrows
const double kReadX = 760.0, kReadY = 556.0, kReadW = 466.0, kReadH = 62.0;

// Slide switches: detent spacing
const double kSlideHalf = 30.0;
// Tabs
const double kTabW = 44.0, kTabH = 58.0;
inline double tabW (const Ctl& c) { return c.style == kSmallTab ? 34.0 : kTabW; }
inline double tabH (const Ctl& c) { return c.style == kSmallTab ? 38.0 : kTabH; }

} // namespace layout
} // namespace tbsyn

#endif
