// Presets.cpp

#include "Presets.h"
#include "FactoryPresets.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

namespace tbsyn
{

namespace
{
    const char* const kHeader = "TBSYNTH-PRESETS 2";        // banks: slot = instrument * 8 + key
    const char* const kHeaderV1 = "TBSYNTH-PRESETS 1";      // the first release: 8 shared slots

    // mkdir -p for the folder containing 'file'
    void makeParentDirs (const std::string& file)
    {
        const std::string::size_type last = file.rfind ('/');
        if (last == std::string::npos || last == 0) return;
        const std::string dir = file.substr (0, last);
        for (std::string::size_type i = 1; i <= dir.size(); ++i)
            if (i == dir.size() || dir[i] == '/')
                mkdir (dir.substr (0, i).c_str(), 0755);   // existing folders just fail harmlessly
    }

    // One line of printable characters, trimmed, at most kMaxPresetName long
    std::string cleanName (const std::string& in)
    {
        std::string out;
        for (std::string::size_type i = 0; i < in.size(); ++i)
        {
            const unsigned char c = (unsigned char) in[i];
            if (c == '\n' || c == '\r' || c == '\t') out += ' ';
            else if (c >= 32 && c != 127) out += (char) c;      // UTF-8 bytes pass through
        }
        std::string::size_type a = out.find_first_not_of (' ');
        if (a == std::string::npos) return std::string();
        std::string::size_type b = out.find_last_not_of (' ');
        out = out.substr (a, b - a + 1);
        if (out.size() > (std::string::size_type) kMaxPresetName)
        {
            std::string::size_type n = kMaxPresetName;
            while (n > 0 && (((unsigned char) out[n]) & 0xC0) == 0x80) --n;   // don't split a UTF-8 character
            out = out.substr (0, n);
        }
        return out;
    }
}

PresetStore::PresetStore()
    : path (defaultPath())
{
    for (int s = 0; s < kNumPresetSlots; ++s)
    {
        used[s] = false;
        for (int i = 0; i < kNumParams; ++i) vals[s][i] = kParams[i].def;
    }
}

std::string PresetStore::defaultPath()
{
    const char* over = std::getenv ("TBSYNTH_PRESET_FILE");
    if (over && *over) return over;
    const char* home = std::getenv ("HOME");
    std::string base = (home && *home) ? home : "/tmp";
#if defined(__APPLE__)
    return base + "/Library/Application Support/TB/TB Synth/Presets.txt";
#else
    return base + "/.config/TB/TB Synth/Presets.txt";
#endif
}

bool PresetStore::load()
{
    for (int s = 0; s < kNumPresetSlots; ++s) { used[s] = false; names[s].clear(); }

    FILE* f = std::fopen (path.c_str(), "r");
    if (! f) { addFactory(); return false; }

    char line[4096];
    bool headerOk = false, v1 = false;
    while (std::fgets (line, sizeof (line), f))
    {
        if (! headerOk)
        {
            headerOk = std::strncmp (line, kHeader, std::strlen (kHeader)) == 0;
            if (! headerOk && std::strncmp (line, kHeaderV1, std::strlen (kHeaderV1)) == 0) { headerOk = true; v1 = true; }
            if (! headerOk) break;
            continue;
        }
        int slot = -1, used0 = 0;
        if (std::sscanf (line, "name %d:%n", &slot, &used0) == 1 && slot >= 1 && slot <= kNumPresetSlots && used0 > 0)
        {
            std::string n (line + used0);
            if (! n.empty() && n[0] == ' ') n.erase (0, 1);
            names[slot - 1] = cleanName (n);
            continue;
        }
        slot = -1; used0 = 0;
        if (std::sscanf (line, "slot %d:%n", &slot, &used0) != 1 || slot < 1 || slot > kNumPresetSlots)
            continue;

        float v[kNumParams];
        const char* p = line + used0;
        int got = 0;
        for (; got < kNumParams; ++got)
        {
            char* end = 0;
            const double d = std::strtod (p, &end);
            if (end == p) break;
            v[got] = (d == d) ? (float) (d < 0.0 ? 0.0 : (d > 1.0 ? 1.0 : d)) : kParams[got].def;
            p = end;
        }
        if (got == 0) continue;
        for (int i = got; i < kNumParams; ++i) v[i] = kParams[i].def;   // older files: fill the rest
        for (int i = 0; i < kNumParams; ++i) vals[slot - 1][i] = v[i];
        used[slot - 1] = true;
    }
    std::fclose (f);

    // a first-release file: its 8 shared slots go to the banks of the instruments they
    // were made on, after that bank's factory presets
    if (headerOk && v1)
    {
        bool oldUsed[kSlotsPerBank];
        std::string oldName[kSlotsPerBank];
        static float oldVals[kSlotsPerBank][kNumParams];
        for (int k = 0; k < kSlotsPerBank; ++k)
        {
            oldUsed[k] = used[k]; oldName[k] = names[k];
            for (int i = 0; i < kNumParams; ++i) oldVals[k][i] = vals[k][i];
        }
        for (int s2 = 0; s2 < kNumPresetSlots; ++s2) { used[s2] = false; names[s2].clear(); }
        addFactory();       // the factory presets keep their keys; earlier saves follow them
        for (int k = 0; k < kSlotsPerBank; ++k)
        {
            if (! oldUsed[k]) continue;
            const int inst = paramStep (kInst, oldVals[k][kInst]);
            for (int key = 0; key < kSlotsPerBank; ++key)
            {
                const int dst = bankSlot (inst, key);
                if (used[dst]) continue;
                used[dst] = true;
                names[dst] = oldName[k];
                for (int i = 0; i < kNumParams; ++i) vals[dst][i] = oldVals[k][i];
                break;
            }
        }
    }
    if (! headerOk) for (int s2 = 0; s2 < kNumPresetSlots; ++s2) { used[s2] = false; names[s2].clear(); }
    addFactory();
    return headerOk;
}

// Factory presets fill their keys unless the user has saved something there
void PresetStore::addFactory()
{
    for (int f = 0; f < kNumFactoryPresets; ++f)
    {
        const FactoryPreset& fp = kFactoryPresets[f];
        const int slot = bankSlot (fp.instrument, fp.key);
        if (slot < 0 || slot >= kNumPresetSlots || used[slot]) continue;
        for (int i = 0; i < kNumParams; ++i) vals[slot][i] = kParams[i].def;
        vals[slot][kInst] = stepValue (fp.instrument, kNumInstruments);
        for (int k = 0; k < 16 && fp.settings[k].param >= 0; ++k)
            if (fp.settings[k].param < kNumParams) vals[slot][fp.settings[k].param] = fp.settings[k].value;
        used[slot] = true;
        names[slot] = fp.name;
    }
}

bool PresetStore::save() const
{
    makeParentDirs (path);
    const std::string tmp = path + ".tmp";
    FILE* f = std::fopen (tmp.c_str(), "w");
    if (! f) return false;
    std::fprintf (f, "%s\n", kHeader);
    std::fprintf (f, "# TB Synth presets. Slot = instrument x 8 + key (Minimoog 1-8, OB-X 9-16, Solina 17-24, Wurlitzer 25-32,\n");
    std::fprintf (f, "# B-3 33-40, Cembalet 41-48, Clavinet 49-56). Values 0..1 in parameter order; then its name.\n");
    for (int s = 0; s < kNumPresetSlots; ++s)
    {
        if (! used[s]) continue;
        std::fprintf (f, "slot %d:", s + 1);
        for (int i = 0; i < kNumParams; ++i) std::fprintf (f, " %.6f", vals[s][i]);
        std::fprintf (f, "\n");
        if (! names[s].empty()) std::fprintf (f, "name %d: %s\n", s + 1, names[s].c_str());
    }
    const bool ok = std::fclose (f) == 0;
    if (! ok) { std::remove (tmp.c_str()); return false; }
    return std::rename (tmp.c_str(), path.c_str()) == 0;
}

bool PresetStore::has (int slot) const
{
    return slot >= 0 && slot < kNumPresetSlots && used[slot];
}

bool PresetStore::get (int slot, float values[kNumParams]) const
{
    if (! has (slot)) return false;
    for (int i = 0; i < kNumParams; ++i) values[i] = vals[slot][i];
    return true;
}

void PresetStore::put (int slot, const float values[kNumParams])
{
    if (slot < 0 || slot >= kNumPresetSlots) return;
    for (int i = 0; i < kNumParams; ++i) vals[slot][i] = values[i];
    used[slot] = true;
}

std::string PresetStore::name (int slot) const
{
    return has (slot) ? names[slot] : std::string();
}

void PresetStore::setName (int slot, const std::string& n)
{
    if (slot < 0 || slot >= kNumPresetSlots) return;
    names[slot] = cleanName (n);
}

} // namespace tbsyn
