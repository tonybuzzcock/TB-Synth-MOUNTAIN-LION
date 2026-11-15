// vst_host_test.cpp
// A minimal VST 2.4 host that loads the built TB Synth binary through its entry
// point and checks it as an instrument: identity, parameters and their
// display strings, MIDI notes at the right sample, both outputs, every
// instrument, block sizes, sample-rate changes, automation, and shutdown.
//
//   vst_host_test <path to plugin binary>
// On macOS pass the binary inside the bundle: "TB Synth.vst/Contents/MacOS/TBSynth"

#include "../Source/VST2/vst2_abi.h"
#include "../Source/DSP/SynthParams.h"
#include "../Source/DSP/Synth.h"

#include <dlfcn.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace tbsyn;

static int failures = 0;
static void check (bool ok, const char* what)
{
    std::printf ("  %s  %s\n", ok ? "ok  " : "FAIL", what);
    if (! ok) ++failures;
}

static int automateCalls = 0, wantMidi = 0;
static VstIntPtr VST_CALL hostCallback (AEffect*, VstInt32 opcode, VstInt32, VstIntPtr, void*, float)
{
    switch (opcode)
    {
        case audioMasterVersion:   return 2400;
        case audioMasterAutomate:  ++automateCalls; return 0;
        case audioMasterWantMidi:  ++wantMidi; return 1;
        default:                   return 0;
    }
}

typedef AEffect* (*EntryProc) (audioMasterCallback);

static VstIntPtr call (AEffect* e, VstInt32 op, VstInt32 idx = 0, VstIntPtr val = 0, void* ptr = 0, float opt = 0.0f)
{
    return e->dispatcher (e, op, idx, val, ptr, opt);
}

// Send MIDI events for the next block
struct Midi
{
    VstMidiEvent ev[64];
    struct { VstInt32 numEvents; VstIntPtr reserved; VstEvent* events[64]; } list;
    int n;
    Midi() : n (0) { std::memset (ev, 0, sizeof (ev)); list.numEvents = 0; list.reserved = 0; }
    void add (int delta, int st, int d1, int d2)
    {
        if (n >= 64) return;
        VstMidiEvent& m = ev[n];
        m.type = kVstMidiType; m.byteSize = sizeof (VstMidiEvent); m.deltaFrames = delta;
        m.midiData[0] = (char) st; m.midiData[1] = (char) d1; m.midiData[2] = (char) d2;
        list.events[n] = (VstEvent*) &ev[n];
        ++n;
    }
    void send (AEffect* e)
    {
        list.numEvents = n;
        call (e, effProcessEvents, 0, 0, &list);
        n = 0;
    }
};

static bool finite (const std::vector<float>& x)
{
    for (size_t i = 0; i < x.size(); ++i) if (! (x[i] == x[i]) || std::fabs (x[i]) > 4.0f) return false;
    return true;
}

static void run (AEffect* e, std::vector<float>& L, std::vector<float>& R, int frames, int block)
{
    const size_t start = L.size();
    L.resize (start + frames, 0.0f);
    R.resize (start + frames, 0.0f);
    for (int pos = 0; pos < frames; pos += block)
    {
        const int n = std::min (block, frames - pos);
        float* out[2] = { &L[start + pos], &R[start + pos] };
        e->processReplacing (e, 0, out, n);
    }
}

int main (int argc, char** argv)
{
    if (argc < 2) { std::printf ("usage: %s <plugin binary>\n", argv[0]); return 2; }
    setenv ("TBSYNTH_PRESET_FILE", "/tmp/tbsynth_host_test_presets.txt", 1);
    std::printf ("Loading %s\n", argv[1]);
    void* lib = dlopen (argv[1], RTLD_NOW | RTLD_LOCAL);
    if (! lib) { std::printf ("dlopen failed: %s\n", dlerror()); return 1; }
    EntryProc entry = (EntryProc) dlsym (lib, "VSTPluginMain");
    check (entry != 0, "exports VSTPluginMain");
#if defined(__APPLE__)
    check (dlsym (lib, "main_macho") != 0, "exports main_macho (older Mac hosts)");
#endif
    if (! entry) return 1;
    AEffect* e = entry (hostCallback);
    check (e != 0, "entry point returns an effect");
    if (! e) return 1;

    std::printf ("\nIdentity\n");
    check (e->magic == kEffectMagic, "magic is 'VstP'");
    check ((e->flags & effFlagsIsSynth) != 0, "flagged as an instrument");
    check (e->numInputs == 0 && e->numOutputs == 2, "no inputs, two outputs (mono on both)");
    check (e->numParams == kNumParams, "every control is a parameter");
    check ((e->flags & effFlagsCanReplacing) != 0, "processReplacing supported");
    check (e->initialDelay == 0, "no latency");
    check (e->uniqueID == VST_FOURCC ('T', 'B', 's', 'y'), "unique ID 'TBsy'");
    char buf[256];
    std::memset (buf, 0, sizeof (buf));
    call (e, effGetEffectName, 0, 0, buf);
    check (std::strcmp (buf, "TB Synth") == 0, "name 'TB Synth'");
    check (call (e, effGetPlugCategory) == kPlugCategSynth, "category: synth");
    check (call (e, effCanDo, 0, 0, (void*) "receiveVstMidiEvent") == 1, "takes MIDI");

    call (e, effOpen);
    check (wantMidi > 0, "asks the host for MIDI");
    call (e, effSetSampleRate, 0, 0, 0, 44100.0f);
    call (e, effSetBlockSize, 0, 512);

    std::printf ("\nParameters\n");
    {
        bool namesOk = true, defsOk = true, dispOk = true;
        for (int i = 0; i < kNumParams; ++i)
        {
            char name[64] = { 0 }, disp[64] = { 0 };
            call (e, effGetParamName, i, 0, name);
            call (e, effGetParamDisplay, i, 0, disp);
            if (std::strcmp (name, kParams[i].name) != 0 || std::strlen (name) > 8) namesOk = false;
            if (std::strlen (disp) > 8 || disp[0] == 0) { dispOk = false; std::printf ("  display '%s' for %s\n", disp, name); }
            if (e->getParameter (e, i) != kParams[i].def) defsOk = false;
        }
        check (namesOk, "names as the table, 8 characters at most");
        check (dispOk, "every display string 1 to 8 characters");
        check (defsOk, "defaults as the table");
        struct { int p; float v; const char* want; } shows[] = {
            { kInst, 0.0f, "Minimoog" }, { kInst, 1.0f, "Clavinet" }, { kInst, stepValue (kHammond, kNumInstruments), "B-3" },
            { kMaster, 0.75f, "+0.0" }, { kMaster, 0.0f, "-inf" },
            { kMgCutoff, 1.0f, "20.00k" }, { kMgCutoff, 0.0f, "10.0" }, { kMgO1Rng, 0.0f, "LO" }, { kMgO1Rng, 1.0f, "2'" },
            { kMgO1Wave, 1.0f, "NARROW" }, { kMgFDec, 1.0f, "35.00s" }, { kMgFAtk, 0.0f, "1ms" },
            { kOxLfoWv, 1.0f, "SAMPLE" }, { kOxPW, 0.0f, "50%" }, { kOxPorta, 0.0f, "Off" },
            { kB3Db1, 1.0f, "8" }, { kB3Db1, 0.0f, "0" }, { kB3VibSel, 1.0f, "C3" }, { kLsSpeed, 0.0f, "STOP" },
            { kClCD, 1.0f, "D" }, { kClTreb, 1.0f, "On" },
        };
        bool all = true;
        for (size_t k = 0; k < sizeof (shows) / sizeof (shows[0]); ++k)
        {
            e->setParameter (e, shows[k].p, shows[k].v);
            std::memset (buf, 0, sizeof (buf));
            call (e, effGetParamDisplay, shows[k].p, 0, buf);
            if (std::strcmp (buf, shows[k].want) != 0) { all = false; std::printf ("  %s: got '%s', want '%s'\n", kParams[shows[k].p].name, buf, shows[k].want); }
        }
        check (all, "display strings show the panel's markings and units");
        for (int i = 0; i < kNumParams; ++i) e->setParameter (e, i, kParams[i].def);
        e->setParameter (e, kMaster, 7.0f);
        check (e->getParameter (e, kMaster) == 1.0f, "out-of-range values are clamped");
        e->setParameter (e, kMaster, kParams[kMaster].def);
        e->setParameter (e, 9999, 0.5f);
        check (e->getParameter (e, 9999) == 0.0f, "invalid parameter index is harmless");
    }

    std::printf ("\nPlaying\n");
    call (e, effMainsChanged, 0, 1);
    const int sr = 44100;
    Midi midi;
    {
        // silence with no notes
        std::vector<float> L, R;
        run (e, L, R, sr / 2, 512);
        double mx = 0.0; for (size_t i = 0; i < L.size(); ++i) mx = std::max (mx, (double) std::fabs (L[i]));
        check (mx < 1e-6, "silent with no notes");
    }
    {
        // a note at offset 300 in a 512 block starts exactly there; matches the DSP core
        // (a fresh instance, so its oscillators start where a fresh core's do)
        AEffect* f = entry (hostCallback);
        call (f, effOpen);
        call (f, effSetSampleRate, 0, 0, 0, 44100.0f);
        call (f, effMainsChanged, 0, 1);
        std::vector<float> L, R;
        midi.add (300, 0x90, 60, 100);
        midi.send (f);
        run (f, L, R, 512, 512);
        run (f, L, R, sr / 2, 512);
        double before = 0.0, after = 0.0;
        for (int i = 0; i < 300; ++i) before = std::max (before, (double) std::fabs (L[i]));
        for (int i = 300; i < 512 + 2000; ++i) after = std::max (after, (double) std::fabs (L[i]));
        check (before < 1e-7 && after > 1e-3, "a note starts at its MIDI offset");
        bool same = true;
        for (size_t i = 0; i < L.size(); ++i) if (L[i] != R[i]) same = false;
        check (same, "both outputs carry the same mono signal");
        // reference: the DSP core straight
        Synth* ref = new Synth();
        ref->prepare (sr);
        std::vector<float> y (L.size());
        ref->process (&y[0], 300);
        ref->midi (0x90, 60, 100);
        for (size_t pos = 300; pos < y.size(); pos += 512) ref->process (&y[pos], (int) std::min ((size_t) 512, y.size() - pos));
        delete ref;
        double d = 0.0;
        for (size_t i = 0; i < y.size(); ++i) d = std::max (d, (double) std::fabs (y[i] - L[i]));
        std::printf ("  max difference from the DSP core: %.2g\n", d);
        check (d < 1e-6, "matches the DSP core");
        midi.add (0, 0x80, 60, 0);
        midi.send (f);
        run (f, L, R, sr * 2, 512);
        double tail = 0.0; for (size_t i = L.size() - sr / 2; i < L.size(); ++i) tail = std::max (tail, (double) std::fabs (L[i]));
        check (tail < 1e-3, "note off: the sound stops");
        call (f, effMainsChanged, 0, 0);
        call (f, effClose);
    }
    {
        // every instrument through the plugin
        bool ok = true;
        for (int inst = 0; inst < kNumInstruments; ++inst)
        {
            e->setParameter (e, kInst, stepValue (inst, kNumInstruments));
            std::vector<float> L, R;
            run (e, L, R, sr / 20, 256);
            midi.add (10, 0x90, 52, 100); midi.add (20, 0x90, 59, 100); midi.add (30, 0x90, 64, 100);
            midi.send (e);
            run (e, L, R, sr, 256);
            midi.add (0, 0x80, 52, 0); midi.add (0, 0x80, 59, 0); midi.add (0, 0x80, 64, 0);
            midi.send (e);
            run (e, L, R, sr, 256);
            double pk = 0.0; for (size_t i = 0; i < L.size(); ++i) pk = std::max (pk, (double) std::fabs (L[i]));
            std::printf ("  %-24s peak %.1f dBFS\n", instrumentName (inst), 20.0 * std::log10 (pk + 1e-12));
            if (! finite (L) || pk < 0.02) ok = false;
        }
        check (ok, "all seven instruments play through the plugin");
        e->setParameter (e, kInst, 0.0f);
    }
    {
        // in-place / odd blocks, 1-sample blocks, big blocks, accumulate
        std::vector<float> L, R;
        midi.add (0, 0x90, 48, 127); midi.send (e);
        run (e, L, R, sr / 2, 333);
        run (e, L, R, 2000, 1);
        run (e, L, R, 20000, 8192);
        check (finite (L), "odd, 1-sample and 8192-sample blocks");
        std::vector<float> a (512, 1.0f), b (512, 1.0f);
        float* out[2] = { &a[0], &b[0] };
        e->process (e, 0, out, 512);
        float mn = 10.0f; for (int i = 0; i < 512; ++i) mn = std::min (mn, std::fabs (a[i] - 1.0f));
        double sum = 0.0; for (int i = 0; i < 512; ++i) sum += std::fabs (a[i] - 1.0f);
        check (sum > 0.01, "process() adds to the outputs");
        midi.add (0, 0xB0, 123, 0); midi.send (e);
        run (e, L, R, sr, 512);
    }
    {
        // pitch bend and mod wheel reach the Minimoog
        std::vector<float> L, R;
        midi.add (0, 0x90, 57, 100); midi.add (0, 0xE0, 0x7F, 0x7F); midi.add (0, 0xB0, 1, 127); midi.send (e);
        run (e, L, R, sr / 2, 512);
        midi.add (0, 0x80, 57, 0); midi.add (0, 0xE0, 0, 0x40); midi.add (0, 0xB0, 1, 0); midi.send (e);
        run (e, L, R, sr, 512);
        check (finite (L), "pitch bend and modulation wheel");
    }

    std::printf ("\nSample-rate change to 96 kHz\n");
    call (e, effMainsChanged, 0, 0);
    call (e, effSetSampleRate, 0, 0, 0, 96000.0f);
    call (e, effMainsChanged, 0, 1);
    {
        std::vector<float> L, R;
        midi.add (0, 0x90, 60, 100); midi.add (0, 0x90, 64, 100); midi.send (e);
        std::clock_t t0 = std::clock();
        run (e, L, R, 96000, 512);
        const double secs = (double) (std::clock() - t0) / CLOCKS_PER_SEC;
        check (finite (L), "runs at 96 kHz");
        std::printf ("  CPU at 96 kHz (Minimoog): %.2f%% of one core\n", 100.0 * secs);
        midi.add (0, 0xB0, 123, 0); midi.send (e);
        run (e, L, R, 9600, 512);
    }

    std::printf ("\nAutomation while playing\n");
    {
        bool ok = true;
        for (int inst = 0; inst < kNumInstruments; ++inst)
        {
            e->setParameter (e, kInst, stepValue (inst, kNumInstruments));
            std::vector<float> L, R;
            midi.add (0, 0x90, 48, 100); midi.add (0, 0x90, 60, 100); midi.add (0, 0x90, 67, 100); midi.send (e);
            for (int blk = 0; blk < 150; ++blk)
            {
                const float t = blk / 150.0f;
                for (int p = 1; p < kNumParams; ++p)
                    if (kParams[p].inst == inst) e->setParameter (e, p, 0.5f + 0.5f * std::sin (6.2831853f * (p % 7 + 1) * t));
                run (e, L, R, 128, 128);
            }
            midi.add (0, 0xB0, 123, 0); midi.send (e);
            run (e, L, R, 9600, 512);
            if (! finite (L)) ok = false;
            for (int p = 1; p < kNumParams; ++p) e->setParameter (e, p, kParams[p].def);
        }
        check (ok, "every control of every instrument automated while playing");
    }

    call (e, effMainsChanged, 0, 0);
    call (e, effClose);
    check (true, "effClose");
    dlclose (lib);
    std::printf ("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL HOST CHECKS PASSED", failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
