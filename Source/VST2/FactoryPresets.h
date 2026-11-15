// FactoryPresets.h
// Presets that come with TB Synth, each on its instrument's bank. A factory
// preset starts from every default and changes only what it lists. Saving over
// its key replaces it. Plain C++98 data.

#ifndef TBSYNTH_FACTORY_PRESETS_H
#define TBSYNTH_FACTORY_PRESETS_H

#include "../DSP/SynthParams.h"

namespace tbsyn
{

struct FactorySetting { int param; float value; };
struct FactoryPreset
{
    int instrument, key;          // key 0 = the first preset key
    const char* name;
    FactorySetting settings[16];  // ends at param -1
};

const FactoryPreset kFactoryPresets[] = {
    // Supertramp, "Bloody Well Right" (Rick Davies' intro): dry, no vibrato,
    // played hard into an amp that's starting to grit
    { kWurlitzer, 0, "Supertramp", {
        { kWuVol, 0.85f }, { kWuVib, 0.0f },
        { -1, 0.0f } } },

    // Stevie Wonder, "Superstition": bright and punchy, both pickups in phase
    // for the body of the low riff, BRILLIANT + TREBLE for the snap
    { kClavinet, 0, "Superstition", {
        { kClBrill, 1.0f }, { kClTreb, 1.0f }, { kClMed, 0.0f }, { kClSoft, 0.0f },
        { kClCD, 1.0f }, { kClAB, 0.0f }, { kClMute, 0.0f }, { kClVol, 0.7f },
        { -1, 0.0f } } },

    // Suzi Quatro's keyboard player: a glam-rock clav, TREBLE + MEDIUM for a gritty
    // middle, the bridge pickup, a little mute for chunky staccato, the preamp pushed
    { kClavinet, 1, "Quatro", {
        { kClBrill, 0.0f }, { kClTreb, 1.0f }, { kClMed, 1.0f }, { kClSoft, 0.0f },
        { kClCD, 0.0f }, { kClAB, 1.0f }, { kClMute, 0.15f }, { kClVol, 0.95f },
        { -1, 0.0f } } },

    // Donna Summer, "I Feel Love" (Giorgio Moroder): the record's pad is the Moog
    // modular; this is the Solina's take on it, bright strings with a soft start
    { kSolina, 0, "Moroder", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f },
        { kSoCello, 1.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.48f }, { kSoSust, 0.55f }, { kSoVol, 0.7f },
        { -1, 0.0f } } },
};
const int kNumFactoryPresets = (int) (sizeof (kFactoryPresets) / sizeof (kFactoryPresets[0]));

} // namespace tbsyn

#endif
