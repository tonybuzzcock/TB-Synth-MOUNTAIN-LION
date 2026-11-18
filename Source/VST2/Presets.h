// Presets.h
// A preset menu of up to 99 for each instrument (slot = instrument * 99 + entry), each
// holding every parameter (the instrument included) and a name, kept in one small
// text file so all instances and sessions share them:
//   Mac:   ~/Library/Application Support/TB/TB Synth/Presets.txt
//   other: ~/.config/TB/TB Synth/Presets.txt
// The TBSYNTH_PRESET_FILE environment variable overrides the location (tests).
// Factory presets (FactoryPresets.h) always hold their keys, the first of each bank:
// they can't be saved over or renamed, and a user preset found on one moves to the
// next free key. A deleted factory preset is only hidden ("hidden" lines in the file)
// and comes back with restoreFactory().
// A file from the first release (8 shared slots) is moved into the banks of the
// instruments its presets were made on.
// Plain C++98. File access happens only on the editor's thread, never in audio.

#ifndef TBSYNTH_PRESETS_H
#define TBSYNTH_PRESETS_H

#include "../DSP/SynthParams.h"

#include <string>

namespace tbsyn
{

enum { kSlotsPerBank = 99, kNumPresetSlots = kSlotsPerBank * kNumInstruments, kMaxPresetName = 40 };

inline int bankSlot (int instrument, int key) { return instrument * kSlotsPerBank + key; }


class PresetStore
{
public:
    PresetStore();
    ~PresetStore();

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

    // Deleting: a user preset is removed; a factory preset is hidden until restored
    void remove (int slot);
    bool restoreFactory (int instrument);      // false when none of that bank's were hidden
    bool anyHidden (int instrument) const;

private:
    void addFactory();
    void placeFactory();
    std::string path;
    bool used[kNumPresetSlots];
    bool hidden[kNumPresetSlots];   // factory presets the user deleted
    std::string names[kNumPresetSlots];
    float (*vals)[kNumParams];      // on the heap: about 1.3 MB, too big for a thread's stack
    PresetStore (const PresetStore&);
    PresetStore& operator= (const PresetStore&);
};

} // namespace tbsyn

#endif
