// SynthVST.cpp
// VST 2.4 instrument wrapper for TB Synth: seven classic keyboards in one
// plugin, chosen with the INSTRUMENT selector (Minimoog Model D, Oberheim OB-X,
// Solina String Ensemble, Wurlitzer 200A, Hammond B-3 with Leslie 122, Hohner
// Cembalet, Hohner Clavinet D6).
//
// - MIDI in: notes (sample-accurate within the block), pitch bend, modulation
//   wheel, sustain pedal, all notes off.
// - Mono, like the originals: the same signal on both outputs, so it sits on a
//   mono or a stereo instrument track (Blue Cat PatchWork in Pro Tools 11).
// - Every control is a host parameter, so a session reopens exactly as it was.
// - On the Mac, the custom panel (PanelView.mm); elsewhere generic controls.
// - No latency. Plain C++98; the editor uses only Cocoa APIs in Mac OS X 10.8.5.

#include "vst2_abi.h"
#include "Editor.h"
#include "Format.h"
#include "Presets.h"
#include "../DSP/Synth.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#if defined(__SSE__) || defined(__x86_64__) || defined(_M_X64)
  #include <xmmintrin.h>
  #define TBSYN_HAVE_SSE 1
#endif

namespace
{

const VstInt32 kUniqueID    = VST_FOURCC ('T', 'B', 's', 'y');
const VstInt32 kVersion     = 1000;       // 1.0.0
const VstInt32 kNumOutputs  = 2;          // the mono signal on both
const char* const kPluginName = "TB Synth";

using namespace tbsyn;

void copyString (char* dst, const char* src, size_t maxLen)
{
    std::strncpy (dst, src, maxLen);
    dst[maxLen] = 0;
}

class SynthVST : public EditorHost
{
public:
    explicit SynthVST (audioMasterCallback hostCallback)
        : host (hostCallback), sampleRate (44100.0), editor (0), numEvents (0)
    {
        std::memset (&effect, 0, sizeof (effect));
        effect.magic = kEffectMagic;
        effect.dispatcher = &dispatcherProc;
        effect.process = &processAccumulateProc;
        effect.setParameter = &setParameterProc;
        effect.getParameter = &getParameterProc;
        effect.processReplacing = &processReplacingProc;
        effect.processDoubleReplacing = 0;
        effect.numPrograms = 1;
        effect.numParams = kNumParams;
        effect.numInputs = 0;
        effect.numOutputs = kNumOutputs;
        effect.flags = effFlagsCanReplacing | effFlagsIsSynth;
#if defined(__APPLE__)
        effect.flags |= effFlagsHasEditor;
#endif
        effect.initialDelay = 0;
        effect.ioRatio = 1.0f;
        effect.object = this;
        effect.uniqueID = kUniqueID;
        effect.version = kVersion;

        for (int i = 0; i < kNumParams; ++i) params[i] = kParams[i].def;
        copyString (programName, "Default", kVstMaxProgNameLen);
        synth = new Synth();
        for (int i = 0; i < kNumParams; ++i) synth->setParam (i, params[i]);
        synth->prepare (sampleRate);
    }

    ~SynthVST()
    {
#if defined(__APPLE__)
        if (editor) editorClose (editor);
#endif
        delete synth;
    }

    AEffect* getAEffect() { return &effect; }

    //==========================================================================
    // EditorHost
    float getParam (int index) const { return (index >= 0 && index < kNumParams) ? params[index] : 0.0f; }
    void getParamText (int index, char* text, int size) const
    {
        if (index >= 0 && index < kNumParams && size > 0) formatValue (index, params[index], text, (size_t) size, true);
        else if (size > 0) text[0] = 0;
    }
    float getMeter (int which) const { return which == kMeterPeak ? (float) synth->peak() : 0.0f; }
    void guiBeginEdit (int index) { if (host) host (&effect, audioMasterBeginEdit, index, 0, 0, 0.0f); }
    void guiEndEdit (int index)   { if (host) host (&effect, audioMasterEndEdit, index, 0, 0, 0.0f); }
    void guiSetParam (int index, float value)
    {
        setParameter (index, value);
        if (host && index >= 0 && index < kNumParams)
            host (&effect, audioMasterAutomate, index, 0, 0, params[index]);
    }

    void refreshPresets() { presets.load(); }
    bool presetExists (int slot) const { return presets.has (slot); }
    bool presetIsFactory (int slot) const { return PresetStore::isFactory (slot); }
    bool savePreset (int slot)
    {
        if (PresetStore::isFactory (slot)) return false;       // factory keys always keep their preset
        presets.load();
        presets.put (slot, params);
        return presets.save();
    }
    void presetName (int slot, char* text, int size) const
    {
        if (! text || size <= 0) return;
        const std::string n = presets.name (slot);
        std::strncpy (text, n.c_str(), (size_t) size - 1);
        text[size - 1] = 0;
    }
    bool renamePreset (int slot, const char* name)
    {
        presets.load();
        if (! presets.has (slot) || PresetStore::isFactory (slot)) return false;
        presets.setName (slot, name ? name : "");
        return presets.save();
    }
    // A preset brings back every control of every instrument, and which instrument
    bool loadPreset (int slot)
    {
        float v[kNumParams];
        if (! presets.get (slot, v)) return false;
        for (int i = 0; i < kNumParams; ++i)
        {
            guiBeginEdit (i);
            guiSetParam (i, v[i]);
            guiEndEdit (i);
        }
        return true;
    }

private:
    //==========================================================================
    static SynthVST* self (AEffect* e) { return static_cast<SynthVST*> (e->object); }

    static VstIntPtr VST_CALL dispatcherProc (AEffect* e, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        SynthVST* p = self (e);
        if (opcode == effClose) { delete p; return 1; }
        return p->dispatch (opcode, index, value, ptr, opt);
    }
    static void VST_CALL setParameterProc (AEffect* e, VstInt32 index, float value) { self (e)->setParameter (index, value); }
    static float VST_CALL getParameterProc (AEffect* e, VstInt32 index)
    {
        SynthVST* p = self (e);
        return (index >= 0 && index < kNumParams) ? p->params[index] : 0.0f;
    }
    static void VST_CALL processReplacingProc (AEffect* e, float** in, float** out, VstInt32 n) { self (e)->processReplacing (in, out, n, false); }
    static void VST_CALL processAccumulateProc (AEffect* e, float** in, float** out, VstInt32 n) { self (e)->processReplacing (in, out, n, true); }

    //==========================================================================
    VstIntPtr dispatch (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
    {
        switch (opcode)
        {
            case effOpen:
                if (host) host (&effect, audioMasterWantMidi, 0, 1, 0, 0.0f);    // older hosts ask for this
                return 0;
            case effSetProgram:        return 0;
            case effGetProgram:        return 0;
            case effSetProgramName:
                if (ptr) copyString (programName, (const char*) ptr, kVstMaxProgNameLen);
                return 0;
            case effGetProgramName:
                if (ptr) copyString ((char*) ptr, programName, kVstMaxProgNameLen);
                return 0;
            case effGetProgramNameIndexed:
                if (ptr && index == 0) { copyString ((char*) ptr, programName, kVstMaxProgNameLen); return 1; }
                return 0;

            case effGetParamLabel:
                if (ptr) ((char*) ptr)[0] = 0;       // units are in the display string
                return 0;
            case effGetParamDisplay:
                if (ptr && index >= 0 && index < kNumParams)
                {
                    char buf[64];
                    formatValue (index, params[index], buf, sizeof (buf), false);
                    copyString ((char*) ptr, buf, kVstMaxParamStrLen);
                }
                return 0;
            case effGetParamName:
                if (ptr && index >= 0 && index < kNumParams) copyString ((char*) ptr, kParams[index].name, kVstMaxParamStrLen);
                return 0;
            case effCanBeAutomated:
                return (index >= 0 && index < kNumParams) ? 1 : 0;

            case effSetSampleRate:
                if (opt > 0.0f) sampleRate = opt;
                return 0;
            case effSetBlockSize:
                return 0;
            case effMainsChanged:
                if (value)
                {
                    synth->prepare (sampleRate);
                    numEvents = 0;
                    if (host) host (&effect, audioMasterWantMidi, 0, 1, 0, 0.0f);
                }
                return 0;

            case effProcessEvents:
                if (ptr) queueEvents ((const VstEvents*) ptr);
                return 1;

#if defined(__APPLE__)
            case effEditGetRect:
                if (ptr)
                {
                    editRect.top = 0; editRect.left = 0;
                    editRect.bottom = (short) kEditorHeight; editRect.right = (short) kEditorWidth;
                    *(ERect**) ptr = &editRect;
                    return 1;
                }
                return 0;
            case effEditOpen:
                if (editor) { editorClose (editor); editor = 0; }
                if (ptr) editor = editorOpen (ptr, this);
                return editor ? 1 : 0;
            case effEditClose:
                if (editor) { editorClose (editor); editor = 0; }
                return 0;
            case effEditIdle:
                return 0;
#endif
            case effGetPlugCategory:   return kPlugCategSynth;
            case effGetEffectName:
                if (ptr) copyString ((char*) ptr, kPluginName, kVstMaxEffectNameLen);
                return 1;
            case effGetProductString:
                if (ptr) copyString ((char*) ptr, kPluginName, kVstMaxProductStrLen);
                return 1;
            case effGetVendorString:
                if (ptr) copyString ((char*) ptr, "TB", kVstMaxVendorStrLen);
                return 1;
            case effGetVendorVersion:  return kVersion;
            case effGetVstVersion:     return 2400;

            case effCanDo:
                if (ptr)
                {
                    const char* s = (const char*) ptr;
                    if (! std::strcmp (s, "receiveVstEvents") || ! std::strcmp (s, "receiveVstMidiEvent")) return 1;
                    if (! std::strcmp (s, "sendVstEvents") || ! std::strcmp (s, "sendVstMidiEvent")) return -1;
                    if (! std::strcmp (s, "plugAsChannelInsert") || ! std::strcmp (s, "plugAsSend")) return -1;
                }
                return 0;

            case effGetTailSize:       return (VstIntPtr) (sampleRate * 6.0);   // long releases
            case effStartProcess:      return 0;
            case effStopProcess:       return 0;
            default:                   return 0;
        }
    }

    void setParameter (VstInt32 index, float value)
    {
        if (index < 0 || index >= kNumParams) return;
        if (! (value == value)) return;
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        params[index] = value;
        synth->setParam (index, value);
    }

    //==========================================================================
    // MIDI: events are kept with their offsets and played at the right sample
    enum { kMaxEvents = 1024 };
    struct Ev { int delta; unsigned char st, d1, d2; };

    void queueEvents (const VstEvents* ev)
    {
        for (int i = 0; i < ev->numEvents; ++i)
        {
            const VstEvent* e = ev->events[i];
            if (! e || e->type != kVstMidiType || numEvents >= kMaxEvents) continue;
            const VstMidiEvent* m = (const VstMidiEvent*) e;
            Ev& q = events[numEvents++];
            q.delta = m->deltaFrames < 0 ? 0 : m->deltaFrames;
            q.st = (unsigned char) m->midiData[0];
            q.d1 = (unsigned char) m->midiData[1];
            q.d2 = (unsigned char) m->midiData[2];
        }
        // keep them in time order (hosts normally send them sorted)
        for (int i = 1; i < numEvents; ++i)
        {
            Ev x = events[i];
            int j = i - 1;
            while (j >= 0 && events[j].delta > x.delta) { events[j + 1] = events[j]; --j; }
            events[j + 1] = x;
        }
    }

    void processReplacing (float** in, float** out, VstInt32 n, bool accumulate)
    {
        (void) in;
        if (n <= 0 || ! out || ! out[0]) return;
#if TBSYN_HAVE_SSE
        const unsigned int oldCsr = _mm_getcsr();
        _mm_setcsr (oldCsr | 0x8040);   // flush denormals to zero
#endif
        VstInt32 pos = 0;
        int e = 0;
        while (pos < n)
        {
            while (e < numEvents && events[e].delta <= pos) { synth->midi (events[e].st, events[e].d1, events[e].d2); ++e; }
            VstInt32 end = n;
            if (e < numEvents && events[e].delta < n) end = events[e].delta;
            if (end > pos + kScratch) end = pos + kScratch;
            const int len = (int) (end - pos);
            synth->process (scratch, len);
            for (int c = 0; c < kNumOutputs; ++c)
            {
                if (! out[c]) continue;
                if (accumulate) for (int i = 0; i < len; ++i) out[c][pos + i] += scratch[i];
                else std::memcpy (out[c] + pos, scratch, sizeof (float) * (size_t) len);
            }
            pos = end;
        }
        for (; e < numEvents; ++e) synth->midi (events[e].st, events[e].d1, events[e].d2);   // offsets past the block
        numEvents = 0;
#if TBSYN_HAVE_SSE
        _mm_setcsr (oldCsr);
#endif
    }

    //==========================================================================
    enum { kScratch = 512 };

    AEffect effect;
    audioMasterCallback host;
    double sampleRate;
    float params[kNumParams];
    char programName[kVstMaxProgNameLen + 1];
    Synth* synth;
    void* editor;
    ERect editRect;
    PresetStore presets;
    Ev events[kMaxEvents];
    int numEvents;
    float scratch[kScratch];
};

} // namespace

//==============================================================================
VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback host)
{
    if (! host) return 0;
    SynthVST* plugin = new SynthVST (host);
    return plugin->getAEffect();
}

#if defined(__APPLE__)
VST_EXPORT AEffect* main_macho (audioMasterCallback host)
{
    return VSTPluginMain (host);
}
#endif
