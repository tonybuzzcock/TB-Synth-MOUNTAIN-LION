// preset_test.cpp
// Checks the preset file: save, reload in a fresh store (another instance or a
// later session), merging slots, folder creation, and damaged files.

#include "../Source/VST2/Presets.h"
#include "../Source/VST2/FactoryPresets.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>

using namespace tbsyn;

static int failures = 0;
static void check (bool ok, const char* what)
{
    std::printf ("  %s  %s\n", ok ? "ok  " : "FAIL", what);
    if (! ok) ++failures;
}

int main()
{
    char tmpl[] = "/tmp/tbsynth_presets_XXXXXX";
    const char* dir = mkdtemp (tmpl);
    if (! dir) { std::printf ("mkdtemp failed\n"); return 1; }
    const std::string file = std::string (dir) + "/nested/folders/Presets.txt";

    std::printf ("Presets\n");
    PresetStore a;
    a.setPath (file);
    check (! a.load(), "missing file reads as factory presets only");
    {
        bool emptyOk = true, factoryOk = true;
        for (int s = 0; s < kNumPresetSlots; ++s)
        {
            bool isFactory = false;
            for (int f = 0; f < kNumFactoryPresets; ++f)
                if (bankSlot (kFactoryPresets[f].instrument, kFactoryPresets[f].key) == s) isFactory = true;
            if (! isFactory && a.has (s)) emptyOk = false;
            if (isFactory && ! a.has (s)) factoryOk = false;
        }
        check (emptyOk, "every other key starts empty");
        check (factoryOk, "the factory presets are there");
        float w[kNumParams];
        const int wurli = bankSlot (kWurlitzer, 0), clav = bankSlot (kClavinet, 0), clav2 = bankSlot (kClavinet, 1), sol = bankSlot (kSolina, 0);
        check (a.name (wurli) == "Supertramp" && a.name (clav) == "Superstition" && a.name (clav2) == "Quatro" && a.name (sol) == "Moroder",
               "Wurlitzer 1 Supertramp, Clavinet 1 Superstition, Clavinet 2 Quatro, Solina 1 Moroder");
        check (a.get (wurli, w) && paramStep (kInst, w[kInst]) == kWurlitzer && std::fabs (w[kWuVol] - 0.85f) < 1e-6f
               && w[kWuVib] == 0.0f && w[kMgCutoff] == kParams[kMgCutoff].def,
               "a factory preset selects its instrument, sets what it lists and leaves the rest at defaults");
    }

    float v1[kNumParams], v2[kNumParams];
    for (int i = 0; i < kNumParams; ++i) { v1[i] = (float) i / kNumParams; v2[i] = 1.0f - v1[i]; }
    a.put (2, v1);
    check (a.save(), "saves, creating the folders");

    PresetStore b;
    b.setPath (file);
    check (b.load() && b.has (2) && ! b.has (0), "a fresh store sees the saved slot");
    float got[kNumParams];
    bool same = b.get (2, got);
    for (int i = 0; i < kNumParams; ++i) if (std::fabs (got[i] - v1[i]) > 1e-5f) same = false;
    check (same, "every value round-trips");

    // Another instance adds a slot; the first one reloads before saving its own
    b.put (7, v2); b.save();
    a.load(); a.put (0, v2); a.save();
    PresetStore c; c.setPath (file); c.load();
    check (c.has (0) && c.has (2) && c.has (7), "slots from different instances are kept");

    // Names: saved, reloaded, kept when the slot's settings are saved again,
    // cleaned (one line, trimmed, 40 characters at most)
    c.setName (2, "  Big slow flange\nwith POS  ");
    c.setName (7, "Slapback");
    c.save();
    PresetStore n; n.setPath (file); n.load();
    check (n.name (2) == "Big slow flange with POS" && n.name (7) == "Slapback" && n.name (0).empty(),
           "names round-trip, cleaned to one line");
    n.put (2, v2); n.save();
    PresetStore n2; n2.setPath (file); n2.load();
    check (n2.name (2) == "Big slow flange with POS", "re-saving a slot keeps its name");
    n2.setName (7, std::string (60, 'x'));
    check (n2.name (7).size() == (size_t) kMaxPresetName, "long names cut to 40 characters");
    n2.setName (7, "");
    n2.save();
    PresetStore n3; n3.setPath (file); n3.load();
    check (n3.has (7) && n3.name (7).empty(), "a name can be cleared");

    // Damaged lines are skipped, out-of-range values clamped, short lines padded
    FILE* f = std::fopen (file.c_str(), "a");
    std::fprintf (f, "slot 99: 0.5 0.5\nslot 5: rubbish\nslot 4: 7.0 -3.0 0.25\n");
    std::fclose (f);
    PresetStore d; d.setPath (file); d.load();
    float s4[kNumParams];
    const bool okDamaged = d.has (2) && ! d.has (4) && d.get (3, s4)
                        && s4[0] == 1.0f && s4[1] == 0.0f && std::fabs (s4[2] - 0.25f) < 1e-6f
                        && s4[kNumParams - 1] == kParams[kNumParams - 1].def;
    check (okDamaged, "damaged lines ignored, values clamped, missing values defaulted");

    // A file that is not ours is ignored rather than misread
    f = std::fopen (file.c_str(), "w"); std::fprintf (f, "hello\nslot 1: 0.1\n"); std::fclose (f);
    PresetStore e; e.setPath (file);
    check (! e.load() && ! e.has (0), "foreign file ignored");

    // A first-release file (8 shared slots) moves into the instruments' banks
    {
        f = std::fopen (file.c_str(), "w");
        std::fprintf (f, "TBSYNTH-PRESETS 1\n");
        const float instHammond = stepValue (kHammond, kNumInstruments), instWurli = stepValue (kWurlitzer, kNumInstruments);
        std::fprintf (f, "slot 1: %.6f 0.5\nname 1: Gospel organ\n", instHammond);
        std::fprintf (f, "slot 2: %.6f 0.6\nname 2: My Wurli\n", instWurli);
        std::fprintf (f, "slot 5: %.6f 0.7\n", instHammond);
        std::fclose (f);
        PresetStore m; m.setPath (file);
        check (m.load(), "a first-release file is read");
        check (m.has (bankSlot (kHammond, 0)) && m.name (bankSlot (kHammond, 0)) == "Gospel organ" && m.has (bankSlot (kHammond, 1)),
               "its B-3 presets go to the B-3 bank, in order");
        check (m.name (bankSlot (kWurlitzer, 0)) == "Supertramp" && m.name (bankSlot (kWurlitzer, 1)) == "My Wurli",
               "factory presets keep the first keys; earlier saves follow them");
        check (! m.has (0) && ! m.has (1), "nothing is left in the Minimoog bank");
    }

    std::printf ("  default location: %s\n", PresetStore::defaultPath().c_str());
    std::string cmd = std::string ("rm -rf '") + dir + "'";
    if (std::system (cmd.c_str()) != 0) std::printf ("  (could not remove %s)\n", dir);

    std::printf ("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL PRESET CHECKS PASSED", failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
