// Editor.h
// Plain C++ interface between the VST wrapper and the Cocoa editor
// (PanelView.mm), so the wrapper itself stays free of Objective-C.
// The editor only exists on the Mac; elsewhere the plugin reports no editor.

#ifndef TBSYNTH_EDITOR_H
#define TBSYNTH_EDITOR_H

namespace tbsyn
{

// A wide landscape face (kFaceWidth x kFaceHeight) inside a mahogany frame
// kPanelFrame wide, like the TB M-50 strips
enum { kFaceWidth = 1240, kFaceHeight = 640, kPanelFrame = 24,
       kEditorWidth = kFaceWidth + 2 * kPanelFrame, kEditorHeight = kFaceHeight + 2 * kPanelFrame };

enum Meter
{
    kMeterPeak = 0,     // output peak, linear (the panel's level LED)
    kNumMeters
};

class EditorHost
{
public:
    virtual ~EditorHost() {}
    virtual float getParam (int index) const = 0;
    virtual void  getParamText (int index, char* text, int size) const = 0;   // with units
    virtual void  guiBeginEdit (int index) = 0;
    virtual void  guiSetParam (int index, float value) = 0;           // also notifies the host
    virtual void  guiEndEdit (int index) = 0;
    virtual float getMeter (int which) const = 0;

    // User presets (slots 0..kNumPresetSlots-1), stored in a file shared by every instance
    virtual void  refreshPresets() = 0;
    virtual bool  presetExists (int slot) const = 0;
    virtual bool  presetIsFactory (int slot) const = 0;   // can't be saved over or renamed
    virtual bool  savePreset (int slot) = 0;
    virtual bool  loadPreset (int slot) = 0;
    virtual void  presetName (int slot, char* text, int size) const = 0;
    virtual bool  renamePreset (int slot, const char* name) = 0;
    virtual bool  deletePreset (int slot) = 0;                   // a factory preset is hidden instead
    virtual bool  restoreFactoryPresets (int instrument) = 0;   // brings back hidden factory presets
    virtual bool  factoryHidden (int instrument) = 0;
    virtual bool  copyPreset (int from, int to) = 0;            // to an ordinary key, keeping the name
};

#if defined(__APPLE__)
void* editorOpen (void* parentView, EditorHost* host);
void  editorClose (void* handle);
#endif

} // namespace tbsyn

#endif
