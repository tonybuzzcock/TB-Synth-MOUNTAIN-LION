// editor_test.mm  (macOS only)
// Loads the built plugin, opens its panel in an offscreen window the way a host
// does, renders a screenshot of every instrument's page, drives every control
// on every page with synthetic mouse events at the coordinates in Layout.h,
// checks host automation messages, presets and naming, and closes it again.
//
//   editor_test <plugin binary> <output dir for PNGs>

#import <Cocoa/Cocoa.h>

#include "../Source/VST2/vst2_abi.h"
#include "../Source/VST2/Editor.h"
#include "../Source/VST2/Layout.h"

#include <dlfcn.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace tbsyn;
using namespace tbsyn::layout;

static int failures = 0;
static void check (bool ok, const char* what)
{
    std::printf ("  %s  %s\n", ok ? "ok  " : "FAIL", what);
    if (! ok) ++failures;
}

static int automateCalls = 0, beginCalls = 0, endCalls = 0;
static VstIntPtr VST_CALL hostCallback (AEffect*, VstInt32 opcode, VstInt32, VstIntPtr, void*, float)
{
    switch (opcode)
    {
        case audioMasterVersion:   return 2400;
        case audioMasterAutomate:  ++automateCalls; return 0;
        case audioMasterBeginEdit: ++beginCalls; return 1;
        case audioMasterEndEdit:   ++endCalls; return 1;
        default:                   return 0;
    }
}

typedef AEffect* (*EntryProc) (audioMasterCallback);
static VstIntPtr call (AEffect* e, VstInt32 op, VstInt32 idx = 0, VstIntPtr val = 0, void* ptr = 0, float opt = 0.0f)
{
    return e->dispatcher (e, op, idx, val, ptr, opt);
}

static NSData* renderPNG (NSView* v, const std::string& path)
{
    const NSRect b = [v bounds];
    NSBitmapImageRep* rep = [v bitmapImageRepForCachingDisplayInRect: b];
    [v cacheDisplayInRect: b toBitmapImageRep: rep];
    NSData* png = [rep representationUsingType: NSPNGFileType properties: [NSDictionary dictionary]];
    [png writeToFile: [NSString stringWithUTF8String: path.c_str()] atomically: YES];
    std::printf ("  wrote %s (%lu bytes)\n", path.c_str(), (unsigned long) [png length]);
    return png;
}

// A mouse event at a point in face coordinates (the face sits inside the frame)
static void mouse (NSWindow* w, NSView* v, NSEventType type, NSPoint facePt, int clicks = 1)
{
    const NSPoint wp = [v convertPoint: NSMakePoint (facePt.x + kPanelFrame, facePt.y + kPanelFrame) toView: nil];
    NSEvent* e = [NSEvent mouseEventWithType: type location: wp modifierFlags: 0 timestamp: 0
                                windowNumber: [w windowNumber] context: nil eventNumber: 0 clickCount: clicks pressure: 1.0f];
    if (type == NSRightMouseDown) [v rightMouseDown: e];
    else if (type == NSLeftMouseDown) [v mouseDown: e];
    else if (type == NSLeftMouseDragged) [v mouseDragged: e];
    else [v mouseUp: e];
}
static void click (NSWindow* w, NSView* v, NSPoint p)
{
    mouse (w, v, NSLeftMouseDown, p);
    mouse (w, v, NSLeftMouseUp, p);
}
static void drag (NSWindow* w, NSView* v, NSPoint p, CGFloat dy)
{
    mouse (w, v, NSLeftMouseDown, p);
    mouse (w, v, NSLeftMouseDragged, NSMakePoint (p.x, p.y + dy * 0.5));
    mouse (w, v, NSLeftMouseDragged, NSMakePoint (p.x, p.y + dy));
    mouse (w, v, NSLeftMouseUp, NSMakePoint (p.x, p.y + dy));
}

static void pump (double secs) { [[NSRunLoop currentRunLoop] runUntilDate: [NSDate dateWithTimeIntervalSinceNow: secs]]; }

static NSPoint menuArrowRight() { return NSMakePoint (kMenuX + kMenuW - kArrowW * 0.5, kMenuY + kMenuH * 0.5); }
static NSPoint menuArrowLeft()  { return NSMakePoint (kMenuX + kArrowW * 0.5, kMenuY + kMenuH * 0.5); }
static NSPoint slotPt (int k)   { return NSMakePoint (kSlotX0 + k * kSlotPitch + kSlotW * 0.5, kSlotY + kSlotH * 0.5); }

int main (int argc, char** argv)
{
    if (argc < 3) { std::printf ("usage: %s <plugin binary> <png dir>\n", argv[0]); return 2; }
    setvbuf (stdout, 0, _IONBF, 0);
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    const std::string dir = argv[2];
    const std::string presetFile = dir + "/test_presets/Presets.txt";
    setenv ("TBSYNTH_PRESET_FILE", presetFile.c_str(), 1);
    std::remove (presetFile.c_str());

    void* lib = dlopen (argv[1], RTLD_NOW | RTLD_LOCAL);
    if (! lib) { std::printf ("dlopen failed: %s\n", dlerror()); return 1; }
    EntryProc entry = (EntryProc) dlsym (lib, "VSTPluginMain");
    if (! entry) { std::printf ("no entry point\n"); return 1; }
    AEffect* e = entry (hostCallback);
    call (e, effOpen);
    call (e, effSetSampleRate, 0, 0, 0, 44100.0f);
    call (e, effMainsChanged, 0, 1);

    std::printf ("Editor\n");
    check ((e->flags & effFlagsHasEditor) != 0, "plugin reports a custom editor");
    ERect* r = 0;
    call (e, effEditGetRect, 0, 0, &r);
    check (r && r->right - r->left == kEditorWidth && r->bottom - r->top == kEditorHeight, "editor size as declared");
    NSWindow* win = [[NSWindow alloc] initWithContentRect: NSMakeRect (0, 0, kEditorWidth, kEditorHeight)
                                                styleMask: NSBorderlessWindowMask backing: NSBackingStoreBuffered defer: NO];
    NSView* parent = [win contentView];
    check (call (e, effEditOpen, 0, 0, parent) == 1, "effEditOpen succeeds");
    check ([[parent subviews] count] == 1, "panel view added to the host's view");
    NSView* panel = [[parent subviews] count] ? [[parent subviews] objectAtIndex: 0] : nil;
    if (! panel) return 1;
    check (NSEqualSizes ([panel frame].size, NSMakeSize (kEditorWidth, kEditorHeight)), "panel has the declared size");

    // ------------------------------------------------------------ the INSTRUMENT menu's arrows, every page
    std::printf ("\nInstrument pages\n");
    {
        static const char* const file[kNumInstruments] = { "minimoog", "obx", "solina", "wurlitzer", "hammond", "cembalet", "clavinet" };
        bool stepped = true;
        std::vector<NSData*> pages;
        for (int k = 0; k < kNumInstruments; ++k)
        {
            const int got = paramStep (kInst, e->getParameter (e, kInst));
            if (got != k) { stepped = false; std::printf ("  expected page %d, on %d\n", k, got); }
            pump (0.06);
            NSData* png = renderPNG (panel, dir + "/panel_" + file[k] + ".png");
            pages.push_back (png);
            automateCalls = beginCalls = endCalls = 0;
            click (win, panel, menuArrowRight());
            if (! (automateCalls == 1 && beginCalls == 1 && endCalls == 1)) stepped = false;
        }
        check (stepped, "the right arrow steps through all seven instruments, telling the host each time");
        check (paramStep (kInst, e->getParameter (e, kInst)) == 0, "and wraps round to the first");
        click (win, panel, menuArrowLeft());
        check (paramStep (kInst, e->getParameter (e, kInst)) == kNumInstruments - 1, "the left arrow steps back (wrapping)");
        bool differ = true;
        for (int a = 0; a < kNumInstruments; ++a)
            for (int b = a + 1; b < kNumInstruments; ++b) if ([pages[a] isEqualToData: pages[b]]) differ = false;
        check (differ, "every instrument has its own page");
        check ([pages[0] length] > 20000, "pages render");
        e->setParameter (e, kInst, 0.0f);
        pump (0.1);
    }

    // ------------------------------------------------------------ every control on every page
    std::printf ("\nEvery control\n");
    {
        bool allOk = true;
        int tested = 0;
        for (int inst = 0; inst < kNumInstruments; ++inst)
        {
            e->setParameter (e, kInst, stepValue (inst, kNumInstruments));
            pump (0.06);
            for (int i = 0; i < kNumCtls; ++i)
            {
                const Ctl& c = kCtls[i];
                const int pinst = kParams[c.param].inst;
                if (pinst != inst && ! (pinst == -1 && inst == 0)) continue;
                // start from a known value: mid for knobs and sliders, a step for switches
                const int n = kParams[c.param].steps;
                float start = n >= 2 ? stepValue (n / 2 - (n == 2 ? 1 : 0), n) : 0.5f;
                if (c.type == kDrawbar) start = stepValue (4, 9);
                std::vector<float> before (kNumParams);
                for (int p = 0; p < kNumParams; ++p) before[p] = e->getParameter (e, p);
                e->setParameter (e, c.param, start);
                before[c.param] = start;
                pump (0.06);
                automateCalls = beginCalls = endCalls = 0;
                const NSPoint at = NSMakePoint (c.x, c.type == kDrawbar ? drawbarY (4) : (c.type == kVSlider ? c.y : c.y));
                switch (c.type)
                {
                    case kKnob:     drag (win, panel, at, -30.0); break;
                    case kVSlider:  drag (win, panel, at, -30.0); break;
                    case kDrawbar:  drag (win, panel, at, kBarStep); break;
                    case kSelector: click (win, panel, at); break;     // a click on the knob steps round
                    case kSlide:    click (win, panel, NSMakePoint (c.x + (n == 2 ? 22.0 : 34.0), c.y)); break;
                    default:        click (win, panel, at); break;
                }
                const float after = e->getParameter (e, c.param);
                bool others = true;
                for (int p = 0; p < kNumParams; ++p) if (p != c.param && e->getParameter (e, p) != before[p]) others = false;
                bool moved;
                if (c.type == kDrawbar) moved = paramStep (c.param, after) == 5;
                else if (n >= 2) moved = paramStep (c.param, after) != paramStep (c.param, start);
                else moved = after > start + 0.05f;
                const bool told = automateCalls > 0 && beginCalls == 1 && endCalls == 1;
                if (! moved || ! others || ! told)
                {
                    allOk = false;
                    std::printf ("  %s (%s): %.3f -> %.3f%s%s\n", kParams[c.param].name, instrumentName (inst), start, after,
                                 others ? "" : ", moved another control", told ? "" : ", host not told");
                }
                e->setParameter (e, c.param, kParams[c.param].def);
                ++tested;
            }
        }
        std::printf ("  %d controls\n", tested);
        check (allOk, "every control on every page moves on its own and tells the host");
        check (tested == kNumParams - 1, "that is every parameter but the instrument itself");
        e->setParameter (e, kInst, 0.0f);
    }

    // ------------------------------------------------------------ particular controls
    std::printf ("\nControls\n");
    {
        // Minimoog cutoff: drag up, double-click back to default
        const Ctl* cut = 0;
        for (int i = 0; i < kNumCtls; ++i) if (kCtls[i].param == kMgCutoff) cut = &kCtls[i];
        const float c0 = e->getParameter (e, kMgCutoff);
        drag (win, panel, NSMakePoint (cut->x, cut->y), -40.0);
        const float c1 = e->getParameter (e, kMgCutoff);
        check (std::fabs (c1 - (c0 + 0.2f)) < 0.01f, "a knob takes 200 px for its whole turn");
        mouse (win, panel, NSLeftMouseDown, NSMakePoint (cut->x, cut->y), 2);
        mouse (win, panel, NSLeftMouseUp, NSMakePoint (cut->x, cut->y), 2);
        check (e->getParameter (e, kMgCutoff) == kParams[kMgCutoff].def, "double-click resets a knob");
        // a click on a marked selector position goes straight there
        const Ctl* rng = 0;
        for (int i = 0; i < kNumCtls; ++i) if (kCtls[i].param == kMgO1Rng) rng = &kCtls[i];
        const double a = (-125.0 + 250.0 * 5 / 5.0) * M_PI / 180.0;   // the 2' position
        click (win, panel, NSMakePoint (rng->x + (rng->r + 17.0) * std::sin (a), rng->y - (rng->r + 17.0) * std::cos (a)));
        check (paramStep (kMgO1Rng, e->getParameter (e, kMgO1Rng)) == 5, "clicking the 2' marking turns RANGE to 2'");
        e->setParameter (e, kMgO1Rng, kParams[kMgO1Rng].def);
        // right-click shows the value without changing it
        e->setParameter (e, kMgEmph, 0.3f);
        const Ctl* em = 0;
        for (int i = 0; i < kNumCtls; ++i) if (kCtls[i].param == kMgEmph) em = &kCtls[i];
        mouse (win, panel, NSRightMouseDown, NSMakePoint (em->x, em->y));
        check (e->getParameter (e, kMgEmph) == 0.3f, "right-click changes nothing");
        pump (0.1);
        renderPNG (panel, dir + "/panel_rightclick.png");
        e->setParameter (e, kMgEmph, kParams[kMgEmph].def);
    }
    {
        // Hammond: a registration, the Leslie on FAST, screenshot
        e->setParameter (e, kInst, stepValue (kHammond, kNumInstruments));
        const float reg[9] = { 8, 8, 8, 8, 0, 0, 0, 0, 0 };
        for (int b = 0; b < 9; ++b) e->setParameter (e, kB3Db1 + b, reg[b] / 8.0f);
        e->setParameter (e, kLsSpeed, 1.0f);
        e->setParameter (e, kB3PHarm, 0.0f);
        pump (0.1);
        renderPNG (panel, dir + "/panel_hammond_registration.png");
        for (int p = 0; p < kNumParams; ++p) if (kParams[p].inst == kHammond) e->setParameter (e, p, kParams[p].def);
        e->setParameter (e, kInst, 0.0f);
    }

    // ------------------------------------------------------------ presets
    std::printf ("\nPresets\n");
    e->setParameter (e, kInst, stepValue (kWurlitzer, kNumInstruments));
    e->setParameter (e, kWuVib, 0.8f);
    pump (0.1);
    click (win, panel, NSMakePoint (kSaveX + kSaveW * 0.5, kSaveY + kSaveH * 0.5));
    click (win, panel, slotPt (2));
    FILE* pf = std::fopen (presetFile.c_str(), "r");
    check (pf != 0, "SAVE then a key writes the preset file");
    if (pf) std::fclose (pf);
    NSTextField* field = nil;
    for (NSView* sv in [panel subviews]) if ([sv isKindOfClass: [NSTextField class]]) field = (NSTextField*) sv;
    check (field != nil, "saving opens a box to name the preset");
    renderPNG (panel, dir + "/panel_naming.png");
    if (field)
    {
        [field setStringValue: @"Wurli with the vibrato up"];
        [field sendAction: [field action] to: [field target]];
        pump (0.2);
    }
    bool named = false;
    pf = std::fopen (presetFile.c_str(), "r");
    if (pf) { char ln[1024]; while (std::fgets (ln, sizeof (ln), pf)) if (std::strstr (ln, "name 3: Wurli with the vibrato up")) named = true; std::fclose (pf); }
    check (named, "Return saves the name with the preset");
    bool gone = true;
    for (int tries = 0; tries < 10; ++tries)
    {
        gone = true;
        for (NSView* sv in [panel subviews]) if ([sv isKindOfClass: [NSTextField class]]) gone = false;
        if (gone) break;
        pump (0.1);
    }
    check (gone, "and the name box goes away");
    renderPNG (panel, dir + "/panel_preset_saved.png");
    // change the instrument and the vibrato, then recall
    e->setParameter (e, kInst, 0.0f);
    e->setParameter (e, kWuVib, 0.1f);
    pump (0.1);
    automateCalls = 0;
    click (win, panel, slotPt (2));
    check (paramStep (kInst, e->getParameter (e, kInst)) == kWurlitzer && std::fabs (e->getParameter (e, kWuVib) - 0.8f) < 1e-5f,
           "the key brings back the instrument and its settings");
    check (automateCalls >= kNumParams, "recall tells the host about every parameter");
    renderPNG (panel, dir + "/panel_preset_loaded.png");
    click (win, panel, NSMakePoint (kReadX + kReadW * 0.5, kReadY + kReadH * 0.5));
    field = nil;
    for (NSView* sv in [panel subviews]) if ([sv isKindOfClass: [NSTextField class]]) field = (NSTextField*) sv;
    check (field != nil && [[field stringValue] isEqualToString: @"Wurli with the vibrato up"], "clicking the readout opens the name to edit");
    if (field)
    {
        [field setStringValue: @"Something else"];
        [(id) panel control: field textView: nil doCommandBySelector: @selector (cancelOperation:)];
        pump (0.2);
    }
    const float before = e->getParameter (e, kWuVib);
    click (win, panel, slotPt (5));
    check (e->getParameter (e, kWuVib) == before, "an empty key changes nothing");
    for (int i = 0; i < kNumParams; ++i) e->setParameter (e, i, kParams[i].def);

    // ------------------------------------------------------------ host changes show
    std::printf ("\nHost automation shows on the panel\n");
    pump (0.1);
    NSData* pngBefore = renderPNG (panel, dir + "/panel_before_host.png");
    e->setParameter (e, kMgCutoff, 0.9f);
    e->setParameter (e, kMgO2Wave, 0.0f);
    pump (0.3);
    NSData* pngAfter = renderPNG (panel, dir + "/panel_changed.png");
    check (! [pngAfter isEqualToData: pngBefore], "panel redraws after host changes");
    e->setParameter (e, kInst, stepValue (kClavinet, kNumInstruments));
    pump (0.3);
    NSData* pngInst = renderPNG (panel, dir + "/panel_host_instrument.png");
    check (! [pngInst isEqualToData: pngAfter], "the page follows an instrument change from the host");
    for (int i = 0; i < kNumParams; ++i) e->setParameter (e, i, kParams[i].def);

    // ------------------------------------------------------------ the level LED
    std::printf ("\nLevel LED\n");
    {
        pump (0.2);
        NSData* quiet = renderPNG (panel, dir + "/panel_led_off.png");
        // play a note through the plugin
        VstMidiEvent ev;
        std::memset (&ev, 0, sizeof (ev));
        ev.type = kVstMidiType; ev.byteSize = sizeof (ev);
        ev.midiData[0] = (char) 0x90; ev.midiData[1] = 48; ev.midiData[2] = 110;
        struct { VstInt32 numEvents; VstIntPtr reserved; VstEvent* events[2]; } list;
        list.numEvents = 1; list.reserved = 0; list.events[0] = (VstEvent*) &ev; list.events[1] = 0;
        call (e, effProcessEvents, 0, 0, &list);
        std::vector<float> L (8192), R (8192);
        float* out[2] = { &L[0], &R[0] };
        e->processReplacing (e, 0, out, 8192);
        pump (0.2);
        NSData* lit = renderPNG (panel, dir + "/panel_showcase.png");
        check (! [lit isEqualToData: quiet], "the LEVEL LED lights while the instrument plays");
    }

    std::printf ("\nClose\n");
    call (e, effEditClose);
    check ([[parent subviews] count] == 0, "effEditClose removes the panel");
    check (call (e, effEditOpen, 0, 0, parent) == 1 && [[parent subviews] count] == 1, "reopens");
    call (e, effEditClose);
    call (e, effMainsChanged, 0, 0);
    call (e, effClose);
    [win release];
    [pool release];
    dlclose (lib);

    std::printf ("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL EDITOR CHECKS PASSED", failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
