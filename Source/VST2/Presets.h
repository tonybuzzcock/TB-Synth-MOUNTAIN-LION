// Presets.h
// A preset menu of up to 32 for each instrument (slot = instrument * 32 + entry), each
// holding every parameter (the instrument included) and a name, kept in one small
// text file so all instances and sessions share them:
//   Mac:   ~/Library/Application Support/TB/TB Synth/Presets.txt
//   other: ~/.config/TB/TB Synth/Presets.txt
// The TBSYNTH_PRESET_FILE environment variable overrides the location (tests).
// Factory presets (FactoryPresets.h) always hold their keys, the first of each bank:
// they can't be saved over or renamed, and a user preset found on one moves to the
// next free key.
// A file from the first release (8 shared slots) is moved into the banks of the
// instruments its presets were made on.
// Plain C++98. File access happens only on the editor's thread, never in audio.

#ifndef TBSYNTH_PRESETS_H
#define TBSYNTH_PRESETS_H

#include "../DSP/SynthParams.h"

#include <string>

namespace tbsyn
{

enum { kSlotsPerBank = 32, kNumPresetSlots = kSlotsPerBank * kNumInstruments, kMaxPresetName = 40 };

inline int bankSlot (int instrument, int key) { return instrument * kSlotsPerBank + key; }


class PresetStore
{
public:
    PresetStore();

    void setPath (const std::string& p) { path = p; }
    const std::string& getPath() const { return path; }
    static std::string defaultPath();

    bool load();                    // re-reads the file; missing file = factory presets only
    bool save() const;              // writes every slot, creating folders as needed

    bool has (int slot) const;
    static bool isFactory (int slot);   // a factory key: always its factory preset
    bool get (int slot, float values[kNumParams]) const;
    void put (int slot, const float values[kNumParams]);   // keeps the slot's name; factory keys refuse

    // A name per slot (up to kMaxPresetName characters, printable, one line)
    std::string name (int slot) const;
    void setName (int slot, const std::string& n);

private:
    void addFactory();
    void placeFactory();
    std::string path;
    bool used[kNumPresetSlots];
    std::string names[kNumPresetSlots];
    float vals[kNumPresetSlots][kNumParams];
};

} // namespace tbsyn

#endif
