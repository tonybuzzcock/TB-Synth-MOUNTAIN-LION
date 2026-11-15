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
enum Style { kGrey = 0, kRed, kOrange, kWave = 10 };   // kWave: selector positions drawn as waveforms

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

    // ------------------------------------------------------------ every page
    { kMaster,    kKnob,     700, 592, 17, "MASTER", "", "", kRed },
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
const double kSaveX = 156.0, kSaveY = 576.0, kSaveW = 74.0, kSaveH = 32.0;
const double kSlotX0 = 244.0, kSlotPitch = 46.0, kSlotW = 40.0, kSlotY = 576.0, kSlotH = 32.0;
const double kReadX = 760.0, kReadY = 556.0, kReadW = 466.0, kReadH = 62.0;

// Slide switches: detent spacing
const double kSlideHalf = 30.0;
// Tabs
const double kTabW = 44.0, kTabH = 58.0;

} // namespace layout
} // namespace tbsyn

#endif
