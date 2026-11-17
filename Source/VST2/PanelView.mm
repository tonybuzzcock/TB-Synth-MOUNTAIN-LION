// PanelView.mm
// The custom panel for TB Synth, in the colours of the user's TASCAM M-50 and
// the TB M-50 strips: a warm grey face with faint grain and black lettering,
// charcoal knurled knobs (grey caps, red caps for levels, orange skirts for
// frequencies), switches that light amber in their engaged position, the red
// lip, a darker strip along the bottom with the TB electronic logo, the preset
// keys (the one in use glows red) and the tan readout, all in a lacquered
// mahogany frame with mitred corners.
//
// Along the top: "TB SYNTH" in heavy lettering and the INSTRUMENT menu (click
// for the list of seven, or step with the arrows). Each instrument has its own
// page laid out like the original's front panel (Layout.h). No manufacturer
// artwork or logos; model names in plain lettering only.
//
// Cocoa, manual retain/release, and only APIs that exist in Mac OS X 10.8.5.
// Everything is drawn in code; no image files.

#import <Cocoa/Cocoa.h>

#include "Editor.h"
#include "Layout.h"
#include "Format.h"
#include "Presets.h"
#include "TBLogo.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

using namespace tbsyn;
using namespace tbsyn::layout;

namespace
{

const CGFloat kTopBar = 64.0;

inline CGFloat valueAngle (CGFloat v) { return (CGFloat) ((-150.0 + 300.0 * v) * M_PI / 180.0); }
inline NSPoint polar (CGFloat cx, CGFloat cy, CGFloat r, CGFloat a) { return NSMakePoint (cx + r * std::sin (a), cy - r * std::cos (a)); }

NSColor* rgb (int r, int g, int b) { return [NSColor colorWithCalibratedRed: r / 255.0 green: g / 255.0 blue: b / 255.0 alpha: 1.0]; }
NSColor* rgba (int r, int g, int b, CGFloat a) { return [NSColor colorWithCalibratedRed: r / 255.0 green: g / 255.0 blue: b / 255.0 alpha: a]; }
NSColor* ink()     { return rgb (28, 28, 26); }
NSColor* dimInk()  { return rgb (128, 125, 118); }
NSColor* faceTop() { return rgb (204, 200, 191); }
NSColor* faceBot() { return rgb (182, 178, 169); }
NSColor* amber()   { return rgb (255, 168, 40); }
NSColor* cream()   { return rgb (234, 228, 208); }

NSFont* boldFont (CGFloat size) { NSFont* f = [NSFont fontWithName: @"Helvetica-Bold" size: size]; return f ? f : [NSFont boldSystemFontOfSize: size]; }
NSFont* plainFont (CGFloat size) { NSFont* f = [NSFont fontWithName: @"Helvetica" size: size]; return f ? f : [NSFont systemFontOfSize: size]; }

NSDictionary* textAttrs (NSFont* font, NSColor* color, CGFloat kern)
{
    return [NSDictionary dictionaryWithObjectsAndKeys: font, NSFontAttributeName, color, NSForegroundColorAttributeName,
              [NSNumber numberWithDouble: kern], NSKernAttributeName, nil];
}

void drawText (const char* s, NSFont* font, NSColor* color, CGFloat cx, CGFloat cy, CGFloat kern = 0.0)
{
    NSString* str = [NSString stringWithUTF8String: s];
    NSDictionary* attrs = textAttrs (font, color, kern);
    const NSSize sz = [str sizeWithAttributes: attrs];
    [str drawAtPoint: NSMakePoint (cx - (sz.width - kern) * 0.5, cy - sz.height * 0.5) withAttributes: attrs];
}

// Text that glows (an engaged switch's legend)
void drawGlowText (const char* s, NSFont* font, NSColor* color, NSColor* glow, CGFloat cx, CGFloat cy)
{
    [NSGraphicsContext saveGraphicsState];
    NSShadow* g = [[[NSShadow alloc] init] autorelease];
    [g setShadowOffset: NSMakeSize (0.0, 0.0)];
    [g setShadowBlurRadius: 7.0];
    [g setShadowColor: glow];
    [g set];
    drawText (s, font, color, cx, cy, 0.4);
    [NSGraphicsContext restoreGraphicsState];
}

// Up to two lines, centred on cy (the first line above, the second below)
void drawLabel (const char* s, NSFont* font, NSColor* color, CGFloat cx, CGFloat cy, CGFloat lineH = 10.0)
{
    const char* nl = std::strchr (s, '\n');
    if (! nl) { drawText (s, font, color, cx, cy, 0.5); return; }
    char a[64];
    const size_t n = std::min ((size_t) (nl - s), sizeof (a) - 1);
    std::memcpy (a, s, n); a[n] = 0;
    drawText (a, font, color, cx, cy - lineH * 0.5, 0.5);
    drawText (nl + 1, font, color, cx, cy + lineH * 0.5, 0.5);
}
bool twoLines (const char* s) { return std::strchr (s, '\n') != 0; }

void line (CGFloat x0, CGFloat y0, CGFloat x1, CGFloat y1, CGFloat w)
{
    NSBezierPath* p = [NSBezierPath bezierPath];
    [p moveToPoint: NSMakePoint (x0, y0)];
    [p lineToPoint: NSMakePoint (x1, y1)];
    [p setLineWidth: w];
    [p stroke];
}

void glowFill (NSBezierPath* p, NSColor* colour, CGFloat blur)
{
    [NSGraphicsContext saveGraphicsState];
    NSShadow* glow = [[[NSShadow alloc] init] autorelease];
    [glow setShadowOffset: NSMakeSize (0.0, 0.0)];
    [glow setShadowBlurRadius: blur];
    [glow setShadowColor: colour];
    [glow set];
    [colour set];
    [p fill];
    [p fill];
    [NSGraphicsContext restoreGraphicsState];
}

void shadowFill (NSBezierPath* p, NSColor* c, CGFloat dx, CGFloat dy, CGFloat blur, CGFloat alpha)
{
    [NSGraphicsContext saveGraphicsState];
    NSShadow* sh = [[[NSShadow alloc] init] autorelease];
    [sh setShadowOffset: NSMakeSize (dx, dy)];
    [sh setShadowBlurRadius: blur];
    [sh setShadowColor: [NSColor colorWithCalibratedWhite: 0.0 alpha: alpha]];
    [sh set];
    [c set];
    [p fill];
    [NSGraphicsContext restoreGraphicsState];
}

void drawTicks (CGFloat cx, CGFloat cy, CGFloat r, int n)
{
    [ink() set];
    for (int k = 0; k < n; ++k)
    {
        const CGFloat a = (CGFloat) ((-150.0 + 300.0 * k / (n - 1)) * M_PI / 180.0);
        const bool big = k == 0 || k == n - 1 || k == (n - 1) / 2;
        const NSPoint p0 = polar (cx, cy, r + 4.0, a), p1 = polar (cx, cy, r + (big ? 10.0 : 8.0), a);
        line (p0.x, p0.y, p1.x, p1.y, 1.2);
    }
}

void endLabels (const char* a, const char* b, CGFloat cx, CGFloat cy, CGFloat r)
{
    NSFont* f = boldFont (7.5);
    const NSPoint pa = polar (cx, cy, r + 14.0, (CGFloat) (-150.0 * M_PI / 180.0));
    const NSPoint pb = polar (cx, cy, r + 14.0, (CGFloat) (150.0 * M_PI / 180.0));
    drawText (a, f, ink(), pa.x - 5.0, pa.y + 4.0);
    drawText (b, f, ink(), pb.x + 5.0, pb.y + 4.0);
}

inline NSPoint viewToFace (NSPoint p) { return NSMakePoint (p.x - kPanelFrame, p.y - kPanelFrame); }
inline NSRect faceToView (NSRect r) { return NSOffsetRect (r, kPanelFrame, kPanelFrame); }

// Mahogany boards and frame (as on the TB M-50 strips)
void drawMahoganyBoard (NSRect r, bool vertical, unsigned seed)
{
    NSGradient* base = [[[NSGradient alloc] initWithColorsAndLocations:
                           rgb (122, 46, 26), 0.0, rgb (104, 36, 20), 0.5, rgb (88, 30, 16), 1.0, nil] autorelease];
    [base drawInRect: r angle: vertical ? 0.0 : 90.0];
    const CGFloat len = vertical ? r.size.height : r.size.width;
    const CGFloat across = vertical ? r.size.width : r.size.height;
    for (int k = 0; k < (int) (across * 3.0); ++k)
    {
        seed = seed * 1664525u + 1013904223u;
        const CGFloat pos = (seed >> 8) / 16777216.0 * across;
        seed = seed * 1664525u + 1013904223u;
        const double q = (seed >> 8) / 16777216.0;
        seed = seed * 1664525u + 1013904223u;
        const CGFloat phase = (seed >> 8) / 16777216.0 * 6.283;
        const CGFloat amp = 0.6 + 1.6 * q, wave = 0.004 + 0.01 * q;
        NSColor* c = q < 0.55 ? rgba (40, 10, 4, 0.10 + 0.16 * q) : rgba (196, 92, 52, 0.05 + 0.10 * (q - 0.55));
        NSBezierPath* g = [NSBezierPath bezierPath];
        for (int i = 0; i <= 60; ++i)
        {
            const CGFloat t = len * i / 60.0;
            const CGFloat off = pos + amp * std::sin (t * wave * 6.283 + phase) + 0.8 * std::sin (t * 0.031 + phase * 2.0);
            const NSPoint pt = vertical ? NSMakePoint (r.origin.x + off, r.origin.y + t) : NSMakePoint (r.origin.x + t, r.origin.y + off);
            if (i == 0) [g moveToPoint: pt]; else [g lineToPoint: pt];
        }
        [g setLineWidth: 0.5 + 1.4 * (1.0 - q)];
        [c set];
        [g stroke];
    }
    for (int k = 0; k < (int) (len * across / 60.0); ++k)
    {
        seed = seed * 1664525u + 1013904223u; const CGFloat a = (seed >> 8) / 16777216.0 * len;
        seed = seed * 1664525u + 1013904223u; const CGFloat b = (seed >> 8) / 16777216.0 * across;
        [rgba (30, 6, 2, 0.35) set];
        NSRectFillUsingOperation (vertical ? NSMakeRect (r.origin.x + b, r.origin.y + a, 0.8, 3.0)
                                           : NSMakeRect (r.origin.x + a, r.origin.y + b, 3.0, 0.8), NSCompositeSourceOver);
    }
}

void drawMahoganyFrame()
{
    const CGFloat W = kEditorWidth, H = kEditorHeight, F = kPanelFrame;
    for (int side = 0; side < 4; ++side)
    {
        NSBezierPath* clip = [NSBezierPath bezierPath];
        NSRect board;
        bool vertical;
        switch (side)
        {
            case 0:
                [clip moveToPoint: NSMakePoint (0, 0)]; [clip lineToPoint: NSMakePoint (W, 0)];
                [clip lineToPoint: NSMakePoint (W - F, F)]; [clip lineToPoint: NSMakePoint (F, F)];
                board = NSMakeRect (0, 0, W, F); vertical = false; break;
            case 1:
                [clip moveToPoint: NSMakePoint (0, H)]; [clip lineToPoint: NSMakePoint (W, H)];
                [clip lineToPoint: NSMakePoint (W - F, H - F)]; [clip lineToPoint: NSMakePoint (F, H - F)];
                board = NSMakeRect (0, H - F, W, F); vertical = false; break;
            case 2:
                [clip moveToPoint: NSMakePoint (0, 0)]; [clip lineToPoint: NSMakePoint (F, F)];
                [clip lineToPoint: NSMakePoint (F, H - F)]; [clip lineToPoint: NSMakePoint (0, H)];
                board = NSMakeRect (0, 0, F, H); vertical = true; break;
            default:
                [clip moveToPoint: NSMakePoint (W, 0)]; [clip lineToPoint: NSMakePoint (W - F, F)];
                [clip lineToPoint: NSMakePoint (W - F, H - F)]; [clip lineToPoint: NSMakePoint (W, H)];
                board = NSMakeRect (W - F, 0, F, H); vertical = true; break;
        }
        [clip closePath];
        [NSGraphicsContext saveGraphicsState];
        [clip addClip];
        drawMahoganyBoard (board, vertical, 3301u + 977u * (unsigned) side);
        NSGradient* sheen = [[[NSGradient alloc] initWithColorsAndLocations:
                                rgba (255, 236, 220, 0.20), 0.0, rgba (255, 236, 220, 0.04), 0.35,
                                rgba (0, 0, 0, 0.0), 0.6, rgba (0, 0, 0, 0.22), 1.0, nil] autorelease];
        const CGFloat ang = side == 0 ? 90.0 : (side == 1 ? -90.0 : (side == 2 ? 0.0 : 180.0));
        [sheen drawInRect: board angle: ang];
        [NSGraphicsContext restoreGraphicsState];
    }
    [rgba (30, 8, 2, 0.75) set];
    line (0, 0, F, F, 1.0);  line (W, 0, W - F, F, 1.0);
    line (0, H, F, H - F, 1.0);  line (W, H, W - F, H - F, 1.0);
    [rgba (255, 220, 200, 0.18) set];
    line (1, 0.5, W - 1, 0.5, 1.0);  line (0.5, 1, 0.5, H - 1, 1.0);
    [rgba (255, 240, 228, 0.10) set];
    NSRectFillUsingOperation (NSMakeRect (5, F, 3, H - 2 * F), NSCompositeSourceOver);
    NSRectFillUsingOperation (NSMakeRect (F, 5, W - 2 * F, 3), NSCompositeSourceOver);
    [rgba (20, 6, 2, 0.9) set];
    NSFrameRectWithWidth (NSMakeRect (F - 2, F - 2, W - 2 * F + 4, H - 2 * F + 4), 2.0);
    [rgba (0, 0, 0, 0.85) set];
    NSFrameRectWithWidth (NSMakeRect (0, 0, W, H), 1.0);
}

// Heavy wide lettering, in the spirit of the M-50's own model name
void drawHeavyName (const char* text, CGFloat cx, CGFloat cy, CGFloat size)
{
    const CGFloat stretch = 1.25;
    NSFont* f = [NSFont fontWithName: @"Arial-Black" size: size];
    const bool black = f != nil;
    if (! f) f = boldFont (size);
    NSString* s = [NSString stringWithUTF8String: text];
    NSMutableDictionary* at = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                 f, NSFontAttributeName, ink(), NSForegroundColorAttributeName,
                                 [NSNumber numberWithDouble: 0.5], NSKernAttributeName, nil];
    if (! black)
    {
        [at setObject: ink() forKey: NSStrokeColorAttributeName];
        [at setObject: [NSNumber numberWithDouble: -9.0] forKey: NSStrokeWidthAttributeName];
    }
    const NSSize sz = [s sizeWithAttributes: at];
    [NSGraphicsContext saveGraphicsState];
    NSAffineTransform* t = [NSAffineTransform transform];
    [t translateXBy: cx yBy: cy];
    [t scaleXBy: stretch yBy: 1.0];
    [t concat];
    [s drawAtPoint: NSMakePoint (-sz.width * 0.5, -sz.height * 0.5) withAttributes: at];
    [NSGraphicsContext restoreGraphicsState];
}

// A tiny waveform picture for a selector position (Minimoog / OB-X)
//   kinds: 0 tri, 1 tri/saw, 2 saw, 3 square, 4 wide pulse, 5 narrow pulse, 6 reverse saw, 7 sine, 8 sample & hold
void drawWaveGlyph (int kind, CGFloat cx, CGFloat cy, NSColor* c)
{
    const CGFloat w = 14.0, h = 7.0, x0 = cx - w * 0.5, yT = cy - h * 0.5, yB = cy + h * 0.5;
    NSBezierPath* p = [NSBezierPath bezierPath];
    switch (kind)
    {
        case 0: [p moveToPoint: NSMakePoint (x0, yB)]; [p lineToPoint: NSMakePoint (x0 + w * 0.25, yT)]; [p lineToPoint: NSMakePoint (x0 + w * 0.75, yB)];
                [p lineToPoint: NSMakePoint (x0 + w, yT)]; break;
        case 1: [p moveToPoint: NSMakePoint (x0, yB)]; [p curveToPoint: NSMakePoint (x0 + w * 0.45, yT) controlPoint1: NSMakePoint (x0 + w * 0.2, yB)
                                                         controlPoint2: NSMakePoint (x0 + w * 0.4, yT)];
                [p lineToPoint: NSMakePoint (x0 + w * 0.5, yB)]; [p curveToPoint: NSMakePoint (x0 + w * 0.95, yT) controlPoint1: NSMakePoint (x0 + w * 0.7, yB)
                                                                     controlPoint2: NSMakePoint (x0 + w * 0.9, yT)];
                [p lineToPoint: NSMakePoint (x0 + w, yB)]; break;
        case 2: [p moveToPoint: NSMakePoint (x0, yB)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5, yT)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5, yB)];
                [p lineToPoint: NSMakePoint (x0 + w, yT)]; [p lineToPoint: NSMakePoint (x0 + w, yB)]; break;
        case 6: [p moveToPoint: NSMakePoint (x0, yT)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5, yB)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5, yT)];
                [p lineToPoint: NSMakePoint (x0 + w, yB)]; [p lineToPoint: NSMakePoint (x0 + w, yT)]; break;
        case 3: case 4: case 5:
        {
            const CGFloat d = kind == 3 ? 0.5 : (kind == 4 ? 0.3 : 0.12);
            [p moveToPoint: NSMakePoint (x0, yB)]; [p lineToPoint: NSMakePoint (x0, yT)];
            [p lineToPoint: NSMakePoint (x0 + w * 0.5 * d, yT)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5 * d, yB)];
            [p lineToPoint: NSMakePoint (x0 + w * 0.5, yB)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5, yT)];
            [p lineToPoint: NSMakePoint (x0 + w * 0.5 * (1 + d), yT)]; [p lineToPoint: NSMakePoint (x0 + w * 0.5 * (1 + d), yB)];
            [p lineToPoint: NSMakePoint (x0 + w, yB)];
            break;
        }
        case 7:
            for (int i = 0; i <= 20; ++i)
            {
                const NSPoint pt = NSMakePoint (x0 + w * i / 20.0, cy - h * 0.5 * std::sin (2.0 * M_PI * i / 20.0));
                if (i == 0) [p moveToPoint: pt]; else [p lineToPoint: pt];
            }
            break;
        default:
        {
            const CGFloat lv[4] = { 0.2, 0.9, 0.0, 0.6 };
            [p moveToPoint: NSMakePoint (x0, yT + h * lv[0])];
            for (int i = 0; i < 4; ++i)
            {
                [p lineToPoint: NSMakePoint (x0 + w * i / 4.0, yT + h * lv[i])];
                [p lineToPoint: NSMakePoint (x0 + w * (i + 1) / 4.0, yT + h * lv[i])];
            }
            break;
        }
    }
    [p setLineWidth: 1.3];
    [p setLineJoinStyle: NSRoundLineJoinStyle];
    [c set];
    [p stroke];
}

int waveKind (int style, int pos)
{
    static const int moog[6] = { 0, 1, 2, 3, 4, 5 };
    static const int moog3[6] = { 0, 6, 2, 3, 4, 5 };
    static const int obx[3] = { 7, 3, 8 };
    if (style == kWave) return moog[pos % 6];
    if (style == kWave + 1) return moog3[pos % 6];
    return obx[pos % 3];
}

int stepsOf (int param) { return kParams[param].steps; }
int instOf (const Ctl& c) { return kParams[c.param].inst; }

// Selector geometry: the angle of each position
CGFloat selectorAngle (int n, int k)
{
    const double span = n >= 6 ? 125.0 : (n == 3 ? 60.0 : 90.0);
    return (CGFloat) ((-span + 2.0 * span * k / (n - 1)) * M_PI / 180.0);
}

CGFloat slideSpacing (int n) { return n == 2 ? 44.0 : 34.0; }
CGFloat slidePosX (const Ctl& c, int k, int n) { return (CGFloat) (c.x + (k - (n - 1) * 0.5) * slideSpacing (n)); }
CGFloat vSliderY (const Ctl& c, CGFloat v) { return (CGFloat) (c.y + c.r - v * 2.0 * c.r); }

NSRect menuRect() { return NSMakeRect (kMenuX, kMenuY, kMenuW, kMenuH); }
NSRect menuLeft() { return NSMakeRect (kMenuX, kMenuY, kArrowW, kMenuH); }
NSRect menuRight() { return NSMakeRect (kMenuX + kMenuW - kArrowW, kMenuY, kArrowW, kMenuH); }
NSRect saveRect() { return NSMakeRect (kSaveX, kSaveY, kSaveW, kSaveH); }
NSRect defaultRect() { return NSMakeRect (kDefaultX, kDefaultY, kDefaultW, kDefaultH); }
NSRect presetRect() { return NSMakeRect (kPresetX, kPresetY, kPresetW, kPresetH); }
NSRect readRect() { return NSMakeRect (kReadX, kReadY, kReadW, kReadH); }

} // namespace

//==============================================================================
@interface TBSynthPanelView : NSView <NSTextFieldDelegate>
{
    EditorHost* host;
    float shown[kNumParams];
    float meter[kNumMeters];
    NSTimer* timer;
    int dragCtl;
    CGFloat dragStartY, dragStartValue;
    bool dragMoved;
    int readoutParam;
    bool saveMode;
    int activePreset;         // slot of the preset last chosen or saved, -1 = none
    bool presetEdited;        // a control moved since: its name shows dimmed
    int presetBank;           // the instrument presetUsed[] was read for
    bool presetUsed[kSlotsPerBank];
    char status[96];
    NSImage* backdrop[kNumInstruments];
    NSTextField* nameField;
    NSTextField* retiredField;
}
- (id) initWithFrame: (NSRect) frame host: (EditorHost*) h;
- (void) shutdown;
- (void) finishNaming: (BOOL) keep;
- (void) refreshBank;
- (void) startNaming;
- (NSMenu*) presetMenu;
- (void) presetPicked: (id) sender;
- (void) defaultSound;
@end

@implementation TBSynthPanelView

- (id) initWithFrame: (NSRect) frame host: (EditorHost*) h
{
    self = [super initWithFrame: frame];
    if (self)
    {
        host = h;
        for (int i = 0; i < kNumParams; ++i) shown[i] = host->getParam (i);
        for (int i = 0; i < kNumMeters; ++i) meter[i] = host->getMeter (i);
        dragCtl = -1;
        dragMoved = false;
        readoutParam = -1;
        saveMode = false;
        activePreset = -1;
        presetEdited = false;
        status[0] = 0;
        for (int i = 0; i < kNumInstruments; ++i) backdrop[i] = nil;
        nameField = nil;
        retiredField = nil;
        host->refreshPresets();
        presetBank = instIndex (shown[kInst]);
        for (int k = 0; k < kSlotsPerBank; ++k) presetUsed[k] = host->presetExists (bankSlot (presetBank, k));
        timer = [[NSTimer timerWithTimeInterval: 1.0 / 20.0 target: self selector: @selector (tick:) userInfo: nil repeats: YES] retain];
        [[NSRunLoop currentRunLoop] addTimer: timer forMode: NSDefaultRunLoopMode];
        [[NSRunLoop currentRunLoop] addTimer: timer forMode: NSEventTrackingRunLoopMode];
    }
    return self;
}

- (void) shutdown
{
    [NSObject cancelPreviousPerformRequestsWithTarget: self];
    [self dropRetired];
    if (nameField)
    {
        [nameField setDelegate: nil];
        [nameField setTarget: nil];
        [nameField removeFromSuperview];
        [nameField release];
        nameField = nil;
    }
    if (timer) { [timer invalidate]; [timer release]; timer = nil; }
    for (int i = 0; i < kNumInstruments; ++i) if (backdrop[i]) { [backdrop[i] release]; backdrop[i] = nil; }
    host = 0;
}

- (void) dealloc
{
    [self shutdown];
    [super dealloc];
}

- (BOOL) isFlipped { return YES; }
- (BOOL) isOpaque { return YES; }
- (BOOL) acceptsFirstMouse: (NSEvent*) e { (void) e; return YES; }

- (int) inst { return instIndex (shown[kInst]); }

// The preset keys show the current instrument's bank
- (void) refreshBank
{
    if (! host) return;
    host->refreshPresets();
    presetBank = [self inst];
    for (int k = 0; k < kSlotsPerBank; ++k) presetUsed[k] = host->presetExists (bankSlot (presetBank, k));
}

- (void) tick: (NSTimer*) t
{
    (void) t;
    if (! host) return;
    bool changed = false;
    for (int i = 0; i < kNumParams; ++i)
    {
        const float v = host->getParam (i);
        if (v != shown[i]) { shown[i] = v; changed = true; }
    }
    const float pk = host->getMeter (kMeterPeak);
    const bool litNow = pk > 0.02f, litBefore = meter[kMeterPeak] > 0.02f;
    const bool hotNow = pk > 0.9f, hotBefore = meter[kMeterPeak] > 0.9f;
    meter[kMeterPeak] = pk;
    if (changed)
    {
        if (presetBank != [self inst]) { activePreset = -1; [self refreshBank]; }
        [self setNeedsDisplay: YES];
        return;
    }
    if (litNow != litBefore || hotNow != hotBefore)
        [self setNeedsDisplayInRect: faceToView (NSMakeRect (kLedX - 24, kLedY - 24, 48, 48))];
}

//------------------------------------------------------------------------------
// The fixed parts of a page, drawn once per instrument

- (NSImage*) backdropFor: (int) inst
{
    if (backdrop[inst]) return backdrop[inst];
    NSImage* img = [[NSImage alloc] initWithSize: NSMakeSize (kEditorWidth, kEditorHeight)];
    backdrop[inst] = img;
    [img lockFocusFlipped: YES];
    drawMahoganyFrame();
    const CGFloat w = kFaceWidth, h = kFaceHeight;
    [NSGraphicsContext saveGraphicsState];
    {
        NSAffineTransform* t = [NSAffineTransform transform];
        [t translateXBy: kPanelFrame yBy: kPanelFrame];
        [t concat];
        NSRectClip (NSMakeRect (0, 0, w, h));
    }

    // the face: warm grey with a faint grain
    NSGradient* fg = [[[NSGradient alloc] initWithStartingColor: faceTop() endingColor: faceBot()] autorelease];
    [fg drawInRect: NSMakeRect (0, 0, w, kStripTop) angle: 90.0];
    unsigned seed = 50u;
    for (int k = 0; k < 24000; ++k)
    {
        seed = seed * 1664525u + 1013904223u; const CGFloat x = (seed >> 8) / 16777216.0 * w;
        seed = seed * 1664525u + 1013904223u; const CGFloat y = (seed >> 8) / 16777216.0 * kStripTop;
        seed = seed * 1664525u + 1013904223u; const double q = (seed >> 8) / 16777216.0;
        [[NSColor colorWithCalibratedWhite: q < 0.5 ? 1.0 : 0.0 alpha: 0.035 + 0.04 * q] set];
        NSRectFillUsingOperation (NSMakeRect (x, y, 3.0, 1.0), NSCompositeSourceOver);
    }

    // top bar: a rule under it, the title, the menu's well, the level LED's name
    [ink() set];
    NSRectFill (NSMakeRect (0, kTopBar, w, 1.2));
    drawHeavyName ("TB SYNTH", 142.0, 32.0, 24.0);
    drawText ("INSTRUMENT", boldFont (7.5), ink(), kMenuX + kMenuW * 0.5, kMenuY + kMenuH + 7.0, 1.0);
    drawText ("LEVEL", boldFont (8.0), ink(), kLedX - 32.0, kLedY, 0.6);

    // sections: thin dark boxes with bold caps titles
    for (int s = 0; s < kNumSections; ++s)
    {
        const Section& sc = kSections[s];
        if (sc.inst != inst) continue;
        NSBezierPath* b = [NSBezierPath bezierPathWithRoundedRect: NSMakeRect (sc.x + 0.5, sc.y + 0.5, sc.w, sc.h) xRadius: 4.0 yRadius: 4.0];
        [rgba (0, 0, 0, 0.05) set];
        [b fill];
        [ink() set];
        [b setLineWidth: 1.0];
        [b stroke];
        NSString* title = [NSString stringWithUTF8String: sc.title];
        NSDictionary* at = textAttrs (boldFont (10.0), ink(), 1.0);
        const NSSize sz = [title sizeWithAttributes: at];
        const CGFloat tx = sc.x + sc.w * 0.5;
        [faceTop() set];
        NSRectFill (NSMakeRect (tx - sz.width * 0.5 - 6.0, sc.y - 6.0, sz.width + 12.0, 12.0));
        drawText (sc.title, boldFont (10.0), ink(), tx, sc.y + 0.5, 1.0);
    }
    for (int c = 0; c < kNumCaptions; ++c)
        if (kCaptions[c].inst == inst) drawText (kCaptions[c].text, boldFont (8.5), ink(), kCaptions[c].x, kCaptions[c].y, 0.8);

    // printed markings round each control of this page
    for (int i = 0; i < kNumCtls; ++i)
    {
        const Ctl& c = kCtls[i];
        if (instOf (c) != inst && instOf (c) != -1) continue;
        if (c.param == kMaster) continue;    // drawn with the strip
        const CGFloat x = c.x, y = c.y;
        switch (c.type)
        {
            case kKnob:
                drawTicks (x, y, c.r, 11);
                endLabels (c.lo, c.hi, x, y, c.r);
                if (c.label[0]) drawLabel (c.label, boldFont (8.5), ink(), x, y - c.r - (twoLines (c.label) ? 29.0 : 24.0));
                break;
            case kSelector:
            {
                const int n = stepsOf (c.param);
                for (int k = 0; k < n; ++k)
                {
                    const CGFloat a = selectorAngle (n, k);
                    const NSPoint p0 = polar (x, y, c.r + 3.0, a), p1 = polar (x, y, c.r + 7.0, a);
                    [ink() set];
                    line (p0.x, p0.y, p1.x, p1.y, 1.2);
                    const NSPoint pl = polar (x, y, c.r + 17.0, a);
                    if (c.style >= kWave) drawWaveGlyph (waveKind (c.style, k), pl.x, pl.y, ink());
                    else
                    {
                        char nm[24];
                        choiceName (kParams[c.param].choices, k, nm, sizeof (nm));
                        drawText (nm, boldFont (7.5), ink(), pl.x, pl.y);
                    }
                }
                if (c.label[0]) drawLabel (c.label, boldFont (8.5), ink(), x, y - c.r - (twoLines (c.label) ? 38.0 : 32.0));
                break;
            }
            case kRocker:
                if (c.label[0]) drawLabel (c.label, boldFont (7.5), ink(), x, y + 30.0 + (twoLines (c.label) ? 5.0 : 0.0), 9.0);
                drawText ("ON", boldFont (6.5), ink(), x, y - 22.0, 0.3);
                break;
            case kTab:
                if (c.label[0]) drawLabel (c.label, boldFont (8.0), ink(), x, y - tabH (c) * 0.5 - (twoLines (c.label) ? 17.0 : 12.0), 9.5);
                break;
            case kDrawbar:
            {
                drawText (c.label, boldFont (9.0), ink(), x, kBarTop - 10.0, 0.3);
                // the slot the bar comes out of
                NSBezierPath* slot = [NSBezierPath bezierPathWithRoundedRect: NSMakeRect (x - kBarW * 0.5 - 3.0, kBarTop - 2.0, kBarW + 6.0, 8.0) xRadius: 2.0 yRadius: 2.0];
                [rgb (24, 23, 22) set];
                [slot fill];
                break;
            }
            case kSlide:
                if (c.label[0]) drawText (c.label, boldFont (8.5), ink(), x, y - 42.0, 0.8);
                break;
            case kVSlider:
            if (c.style == kLever)
            {
                // a compact tone lever (SK-30): slot, ticks at 0 / 5 / 10 and between, no numbers
                NSBezierPath* slot = [NSBezierPath bezierPathWithRoundedRect: NSMakeRect (x - 2.5, y - c.r - 8.0, 5.0, 2.0 * c.r + 16.0) xRadius: 2.5 yRadius: 2.5];
                [rgb (24, 23, 22) set];
                [slot fill];
                for (int k = 0; k <= 10; ++k)
                {
                    const CGFloat ty = vSliderY (c, k / 10.0f);
                    const bool major = k == 0 || k == 5 || k == 10;
                    [ink() set];
                    line (x - (major ? 14.0 : 12.0), ty, x - 8.0, ty, major ? 1.2 : 0.7);
                    line (x + 8.0, ty, x + (major ? 14.0 : 12.0), ty, major ? 1.2 : 0.7);
                }
                drawLabel (c.label, boldFont (7.0), ink(), x, y + c.r + (twoLines (c.label) ? 21.0 : 17.0), 8.5);
                break;
            }
            else
            {
                NSBezierPath* slot = [NSBezierPath bezierPathWithRoundedRect: NSMakeRect (x - 3.0, y - c.r - 12.0, 6.0, 2.0 * c.r + 24.0) xRadius: 3.0 yRadius: 3.0];
                [rgb (24, 23, 22) set];
                [slot fill];
                for (int k = 0; k <= 10; ++k)
                {
                    const CGFloat ty = vSliderY (c, k / 10.0f);
                    [ink() set];
                    line (x - 24.0, ty, x - 13.0, ty, k == 0 || k == 10 || k == 5 ? 1.4 : 0.9);
                    line (x + 13.0, ty, x + 24.0, ty, k == 0 || k == 10 || k == 5 ? 1.4 : 0.9);
                    if (k % 2 == 0)
                    {
                        char n[4];
                        std::snprintf (n, sizeof (n), "%d", k);
                        drawText (n, boldFont (7.5), ink(), x - 33.0, ty + 0.5);
                    }
                }
                drawLabel (c.label, boldFont (8.5), ink(), x, y + c.r + 32.0);
                break;
            }
            default: break;
        }
    }

    // the red lip along the front
    {
        const NSRect lip = NSMakeRect (0, kStripTop - 14.0, w, 14.0);
        NSGradient* lg = [[[NSGradient alloc] initWithColorsAndLocations:
                             rgb (40, 38, 36), 0.0, rgb (40, 38, 36), 0.25, rgb (214, 52, 30), 0.3,
                             rgb (186, 40, 24), 0.7, rgb (40, 38, 36), 0.75, rgb (30, 29, 27), 1.0, nil] autorelease];
        [lg drawInRect: lip angle: 90.0];
    }
    // the plugin strip
    {
        const NSRect strip = NSMakeRect (0, kStripTop, w, h - kStripTop);
        NSGradient* sg = [[[NSGradient alloc] initWithStartingColor: rgb (168, 164, 156) endingColor: rgb (150, 146, 139)] autorelease];
        [sg drawInRect: strip angle: 90.0];
        [rgb (30, 29, 27) set];
        NSRectFill (NSMakeRect (0, kStripTop, w, 1.5));
        for (int i = 0; i < kNumCtls; ++i)
            if (kCtls[i].param == kMaster)
            {
                const Ctl& c = kCtls[i];
                drawTicks (c.x, c.y, c.r, 11);
                drawText ("MASTER", boldFont (8.0), ink(), c.x, c.y - c.r - 18.0, 0.8);
            }
        tblogo::drawTBLogo (NSMakeRect (kLogoX, kLogoY, kLogoW, kLogoH));
    }
    [rgba (0, 0, 0, 0.6) set];
    NSFrameRectWithWidth (NSMakeRect (0, 0, w, h), 1.0);
    [NSGraphicsContext restoreGraphicsState];
    for (int k = 0; k < 6; ++k)
    {
        [rgba (0, 0, 0, 0.22 - 0.035 * k) set];
        NSFrameRectWithWidth (NSMakeRect (kPanelFrame + k, kPanelFrame + k, w - 2 * k, h - 2 * k), 1.0);
    }
    [img unlockFocus];
    return img;
}

//------------------------------------------------------------------------------
// Controls

- (void) drawKnobAt: (NSPoint) c radius: (CGFloat) r style: (int) style value: (float) v
{
    NSColor* skirt = style == kOrange ? rgb (250, 176, 90) : rgb (96, 94, 90);
    NSColor* skirtDark = style == kOrange ? rgb (184, 88, 14) : rgb (34, 33, 31);
    NSColor* capCol = style == kRed ? rgb (206, 38, 34) : rgb (92, 90, 86);
    NSColor* capDark = style == kRed ? rgb (120, 18, 14) : rgb (30, 29, 28);
    const CGFloat sa = valueAngle (v);
    NSBezierPath* sk = [NSBezierPath bezierPath];
    const int ridges = 40;
    for (int k = 0; k <= ridges * 4; ++k)
    {
        const CGFloat t = (CGFloat) (2.0 * M_PI * k / (ridges * 4));
        const CGFloat rr = r * (1.0 - 0.04 * (0.5 - 0.5 * std::cos (ridges * t)));
        const NSPoint p = polar (c.x, c.y, rr, t + sa);
        if (k == 0) [sk moveToPoint: p]; else [sk lineToPoint: p];
    }
    [sk closePath];
    shadowFill (sk, skirtDark, 1.5, -3.0, 4.0, 0.55);
    NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: skirt, 0.0, skirtDark, 1.0, nil] autorelease];
    [g drawInBezierPath: sk relativeCenterPosition: NSMakePoint (-0.35, 0.5)];
    [[NSColor colorWithCalibratedWhite: 0.0 alpha: 0.35] set];
    for (int k = 0; k < ridges; ++k)
    {
        const CGFloat t = (CGFloat) (2.0 * M_PI * k / ridges) + sa;
        const NSPoint p0 = polar (c.x, c.y, r * 0.82, t), p1 = polar (c.x, c.y, r * 0.97, t);
        line (p0.x, p0.y, p1.x, p1.y, 0.7);
    }
    const CGFloat rc = r * (style == kOrange ? 0.55 : 0.66);
    NSBezierPath* cap = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (c.x - rc, c.y - rc, 2 * rc, 2 * rc)];
    shadowFill (cap, capDark, 0.8, -1.5, 2.0, 0.6);
    NSGradient* cg = [[[NSGradient alloc] initWithColorsAndLocations:
                         [capCol blendedColorWithFraction: 0.35 ofColor: [NSColor whiteColor]], 0.0, capCol, 0.5, capDark, 1.0, nil] autorelease];
    [cg drawInBezierPath: cap relativeCenterPosition: NSMakePoint (-0.3, 0.45)];
    [[NSColor colorWithCalibratedWhite: 0.0 alpha: 0.5] set];
    [cap setLineWidth: 0.8];
    [cap stroke];
    [[NSColor whiteColor] set];
    NSBezierPath* p = [NSBezierPath bezierPath];
    [p moveToPoint: polar (c.x, c.y, style == kOrange ? rc + 1.0 : rc * 0.25, sa)];
    [p lineToPoint: polar (c.x, c.y, style == kOrange ? r * 0.97 : rc * 0.92, sa)];
    [p setLineWidth: 2.4];
    [p setLineCapStyle: NSRoundLineCapStyle];
    [p stroke];
}

// Rotary switch: a smaller charcoal knob with a bar pointer at its position
- (void) drawSelector: (const Ctl&) c value: (float) v
{
    const int n = stepsOf (c.param);
    const int k = stepOf (v, n);
    const CGFloat a = selectorAngle (n, k);
    const NSPoint ctr = NSMakePoint (c.x, c.y);
    const CGFloat r = c.r;
    NSBezierPath* body = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (ctr.x - r, ctr.y - r, 2 * r, 2 * r)];
    shadowFill (body, rgb (34, 33, 31), 1.5, -3.0, 4.0, 0.55);
    NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: rgb (110, 108, 104), 0.0, rgb (34, 33, 31), 1.0, nil] autorelease];
    [g drawInBezierPath: body relativeCenterPosition: NSMakePoint (-0.35, 0.5)];
    // the bar across the knob, pointing at the position
    NSBezierPath* bar = [NSBezierPath bezierPath];
    const NSPoint tip = polar (ctr.x, ctr.y, r * 0.95, a), tail = polar (ctr.x, ctr.y, r * 0.95, a + (CGFloat) M_PI);
    [bar moveToPoint: tail];
    [bar lineToPoint: tip];
    [bar setLineWidth: r * 0.55];
    [bar setLineCapStyle: NSRoundLineCapStyle];
    [rgb (52, 51, 48) set];
    [bar stroke];
    [[NSColor whiteColor] set];
    const NSPoint p0 = polar (ctr.x, ctr.y, r * 0.3, a);
    line (p0.x, p0.y, tip.x, tip.y, 2.2);
}

// Rocker: the upper half is ON; the engaged half is raised and lit amber
- (void) drawRocker: (const Ctl&) c on: (bool) on
{
    const NSRect r = NSMakeRect (c.x - 9.0, c.y - 15.0, 18.0, 30.0);
    NSBezierPath* bezel = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (r, -2.5, -2.5) xRadius: 3.0 yRadius: 3.0];
    [rgb (30, 29, 28) set];
    [bezel fill];
    const NSRect top = NSMakeRect (r.origin.x, r.origin.y, r.size.width, r.size.height * 0.5);
    const NSRect bot = NSMakeRect (r.origin.x, r.origin.y + r.size.height * 0.5, r.size.width, r.size.height * 0.5);
    NSBezierPath* lit = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (on ? top : bot, 0.5, 0.5) xRadius: 2.0 yRadius: 2.0];
    NSBezierPath* dark = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (on ? bot : top, 1.5, 1.5) xRadius: 1.5 yRadius: 1.5];
    NSGradient* dg = [[[NSGradient alloc] initWithStartingColor: rgb (70, 68, 64) endingColor: rgb (44, 43, 41)] autorelease];
    [dg drawInBezierPath: dark angle: -90.0];
    if (on)
    {
        glowFill (lit, amber(), 12.0);
        NSGradient* lg = [[[NSGradient alloc] initWithColorsAndLocations:
                             rgb (255, 246, 214), 0.0, rgb (255, 196, 90), 0.45, rgb (230, 140, 20), 1.0, nil] autorelease];
        [lg drawInBezierPath: lit relativeCenterPosition: NSMakePoint (0.0, 0.0)];
    }
    else
    {
        NSGradient* lg = [[[NSGradient alloc] initWithStartingColor: rgb (120, 117, 110) endingColor: rgb (78, 76, 72)] autorelease];
        [lg drawInBezierPath: lit angle: -90.0];
    }
    [rgba (0, 0, 0, 0.6) set];
    [lit setLineWidth: 0.8];
    [lit stroke];
}

// Tab: a cream tablet; down = lit amber from inside, with its LED
- (void) drawTab: (const Ctl&) c on: (bool) on
{
    const double tw = tabW (c), th = tabH (c);
    const NSRect r = NSMakeRect (c.x - tw * 0.5, c.y - th * 0.5 + (on ? 2.0 : 0.0), tw, th);
    NSBezierPath* well = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (NSMakeRect (c.x - tw * 0.5, c.y - th * 0.5, tw, th), -3.0, -3.0) xRadius: 3.0 yRadius: 3.0];
    [rgb (30, 29, 28) set];
    [well fill];
    NSBezierPath* tp = [NSBezierPath bezierPathWithRoundedRect: r xRadius: 3.0 yRadius: 3.0];
    if (on)
    {
        glowFill (tp, amber(), 14.0);
        NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations:
                            rgb (255, 248, 222), 0.0, rgb (255, 210, 120), 0.5, rgb (236, 160, 40), 1.0, nil] autorelease];
        [g drawInBezierPath: tp angle: -90.0];
    }
    else
    {
        shadowFill (tp, rgb (170, 164, 146), 1.0, -3.0, 3.0, 0.5);
        NSGradient* g = [[[NSGradient alloc] initWithStartingColor: rgb (240, 234, 216) endingColor: rgb (196, 190, 170)] autorelease];
        [g drawInBezierPath: tp angle: -90.0];
        [rgba (255, 255, 255, 0.6) set];
        NSRectFillUsingOperation (NSMakeRect (NSMinX (r) + 3.0, NSMinY (r) + 2.0, r.size.width - 6.0, 1.0), NSCompositeSourceOver);
    }
    [rgba (0, 0, 0, 0.55) set];
    [tp setLineWidth: 0.8];
    [tp stroke];
    if (c.lo[0]) drawText (c.lo, boldFont (8.5), on ? rgb (90, 40, 0) : dimInk(), c.x, NSMidY (r) + 6.0, 0.4);
    // LED at the top of the tab
    const NSPoint led = NSMakePoint (c.x, NSMinY (r) + 10.0);
    NSBezierPath* lens = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (led.x - 3.5, led.y - 3.5, 7.0, 7.0)];
    if (on)
    {
        glowFill (lens, rgb (255, 40, 24), 8.0);
        NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: rgb (255, 236, 220), 0.0, rgb (255, 70, 46), 0.5, rgb (214, 20, 12), 1.0, nil] autorelease];
        [g drawInBezierPath: lens relativeCenterPosition: NSMakePoint (-0.2, 0.3)];
    }
    else
    {
        NSGradient* g = [[[NSGradient alloc] initWithStartingColor: rgb (130, 30, 26) endingColor: rgb (60, 10, 8)] autorelease];
        [g drawInBezierPath: lens relativeCenterPosition: NSMakePoint (-0.3, 0.4)];
    }
}

// Hammond drawbar: the bar is pulled down out of its slot; markings 1..8 show
// as it comes out; the handle's colour by footage (brown, white, black)
- (void) drawDrawbar: (const Ctl&) c value: (float) v
{
    const int step = stepOf (v, 9);
    const CGFloat hy = (CGFloat) drawbarY (step);
    const CGFloat x = c.x;
    // the shaft
    const NSRect shaft = NSMakeRect (x - 7.0, kBarTop, 14.0, hy - kBarTop);
    NSGradient* sg = [[[NSGradient alloc] initWithColorsAndLocations:
                         rgb (210, 208, 200), 0.0, rgb (250, 248, 240), 0.4, rgb (160, 158, 150), 1.0, nil] autorelease];
    [sg drawInRect: shaft angle: 0.0];
    for (int k = 1; k <= step; ++k)
    {
        const CGFloat my = (CGFloat) (kBarTop + k * kBarStep - 6.0);
        char n[4];
        std::snprintf (n, sizeof (n), "%d", k);
        drawText (n, boldFont (8.5), ink(), x, my);
        [ink() set];
        line (x - 7.0, my + 9.0, x + 7.0, my + 9.0, 0.6);
    }
    // the handle
    NSColor* top; NSColor* botc; NSColor* text;
    if (c.style == 0)      { top = rgb (150, 88, 52); botc = rgb (88, 44, 22); text = rgb (250, 240, 220); }
    else if (c.style == 1) { top = rgb (252, 250, 244); botc = rgb (200, 196, 186); text = ink(); }
    else                   { top = rgb (76, 74, 70); botc = rgb (20, 20, 18); text = rgb (240, 236, 226); }
    const NSRect hr = NSMakeRect (x - kBarW * 0.5, hy - kBarHandleH * 0.5, kBarW, kBarHandleH);
    NSBezierPath* hp = [NSBezierPath bezierPathWithRoundedRect: hr xRadius: 7.0 yRadius: 7.0];
    shadowFill (hp, botc, 1.5, -3.5, 4.0, 0.55);
    NSGradient* hg = [[[NSGradient alloc] initWithColorsAndLocations: top, 0.0, botc, 1.0, nil] autorelease];
    [hg drawInBezierPath: hp angle: -90.0];
    [rgba (0, 0, 0, 0.6) set];
    [hp setLineWidth: 0.8];
    [hp stroke];
    drawText (c.label, boldFont (8.0), text, x, hy + 0.5, 0.2);
}

// Slide switch (2 or 3 positions): the lever glows amber where it is; that
// legend lights, the others dim
- (void) drawSlide: (const Ctl&) c value: (float) v
{
    const int n = stepsOf (c.param);
    const int idx = stepOf (v, n);
    const CGFloat sp = slideSpacing (n);
    const NSRect slot = NSMakeRect (c.x - sp * (n - 1) * 0.5 - 16.0, c.y - 8.0, sp * (n - 1) + 32.0, 16.0);
    NSBezierPath* p = [NSBezierPath bezierPathWithRoundedRect: slot xRadius: 3.0 yRadius: 3.0];
    [rgb (36, 35, 33) set];
    [p fill];
    for (int k = 0; k < n; ++k)
    {
        const CGFloat x = slidePosX (c, k, n);
        char nm[24];
        choiceName (kParams[c.param].choices, k, nm, sizeof (nm));
        if (k == idx) drawGlowText (nm, boldFont (9.0), ink(), amber(), x, c.y - 22.0);
        else drawText (nm, boldFont (9.0), dimInk(), x, c.y - 22.0, 0.4);
        [rgb (90, 88, 84) set];
        NSRectFill (NSMakeRect (x - 1.0, c.y - 1.0, 2.0, 2.0));
    }
    const CGFloat lx = slidePosX (c, idx, n);
    NSBezierPath* lever = [NSBezierPath bezierPathWithRoundedRect: NSMakeRect (lx - 10.0, c.y - 6.5, 20.0, 13.0) xRadius: 2.5 yRadius: 2.5];
    glowFill (lever, amber(), 12.0);
    NSGradient* lg = [[[NSGradient alloc] initWithColorsAndLocations:
                         rgb (255, 246, 214), 0.0, rgb (255, 196, 90), 0.45, rgb (230, 140, 20), 1.0, nil] autorelease];
    [lg drawInBezierPath: lever relativeCenterPosition: NSMakePoint (0.0, 0.0)];
    [rgb (60, 40, 10) set];
    [lever setLineWidth: 0.8];
    [lever stroke];
}

- (void) drawVSlider: (const Ctl&) c value: (float) v
{
    const CGFloat y = vSliderY (c, v);
    const CGFloat w = c.style == kLever ? 22.0 : 38.0, h = c.style == kLever ? 12.0 : 20.0;
    const NSRect r = NSMakeRect (c.x - w * 0.5, y - h * 0.5, w, h);
    NSBezierPath* cp = [NSBezierPath bezierPathWithRoundedRect: r xRadius: 2.5 yRadius: 2.5];
    shadowFill (cp, rgb (150, 146, 132), 2.0, -4.0, 5.0, 0.6);
    NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations:
                        rgb (250, 246, 232), 0.0, cream(), 0.45, rgb (190, 184, 164), 0.55, rgb (226, 220, 200), 1.0, nil] autorelease];
    [g drawInBezierPath: cp angle: -90.0];
    [rgb (60, 58, 52) set];
    [cp setLineWidth: 0.8];
    [cp stroke];
    [rgb (20, 20, 20) set];
    line (c.x - w * 0.5 + 3.0, y, c.x + w * 0.5 - 3.0, y, 2.0);
}

- (void) drawMenu
{
    const NSRect r = menuRect();
    NSBezierPath* well = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (r, -2.0, -2.0) xRadius: 5.0 yRadius: 5.0];
    [rgb (30, 29, 28) set];
    [well fill];
    NSBezierPath* face = [NSBezierPath bezierPathWithRoundedRect: r xRadius: 4.0 yRadius: 4.0];
    NSGradient* g = [[[NSGradient alloc] initWithStartingColor: rgb (232, 212, 170) endingColor: rgb (204, 178, 130)] autorelease];   // tan, as the readout
    [g drawInBezierPath: face angle: -90.0];
    // the arrows' keys
    const NSRect keys[2] = { menuLeft(), menuRight() };
    for (int k = 0; k < 2; ++k)
    {
        NSBezierPath* kp = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (keys[k], 3.0, 3.0) xRadius: 3.0 yRadius: 3.0];
        NSGradient* kg = [[[NSGradient alloc] initWithStartingColor: rgb (84, 82, 78) endingColor: rgb (44, 43, 41)] autorelease];
        [kg drawInBezierPath: kp angle: -90.0];
        NSBezierPath* tri = [NSBezierPath bezierPath];
        const CGFloat cx = NSMidX (keys[k]), cy = NSMidY (keys[k]);
        if (k == 0) { [tri moveToPoint: NSMakePoint (cx + 5, cy - 7)]; [tri lineToPoint: NSMakePoint (cx - 6, cy)]; [tri lineToPoint: NSMakePoint (cx + 5, cy + 7)]; }
        else        { [tri moveToPoint: NSMakePoint (cx - 5, cy - 7)]; [tri lineToPoint: NSMakePoint (cx + 6, cy)]; [tri lineToPoint: NSMakePoint (cx - 5, cy + 7)]; }
        [tri closePath];
        [[NSColor whiteColor] set];
        [tri fill];
    }
    const int inst = [self inst];
    drawText (instrumentName (inst), boldFont (16.0), [NSColor blackColor], NSMidX (r) - 6.0, NSMidY (r) + 0.5, 0.8);
    // a small drop-down mark
    NSBezierPath* dm = [NSBezierPath bezierPath];
    const CGFloat dx = NSMaxX (r) - kArrowW - 16.0, dy = NSMidY (r);
    [dm moveToPoint: NSMakePoint (dx - 5, dy - 3)]; [dm lineToPoint: NSMakePoint (dx + 5, dy - 3)]; [dm lineToPoint: NSMakePoint (dx, dy + 4)];
    [dm closePath];
    [ink() set];
    [dm fill];
    [rgb (40, 42, 38) set];
    [face setLineWidth: 1.0];
    [face stroke];
}

- (void) drawLevelLed
{
    const NSPoint c = NSMakePoint (kLedX, kLedY);
    const CGFloat r = 6.0;
    const float pk = meter[kMeterPeak];
    const bool lit = pk > 0.02f, hot = pk > 0.9f;
    NSBezierPath* bezel = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (c.x - r - 3.0, c.y - r - 3.0, 2 * r + 6.0, 2 * r + 6.0)];
    NSGradient* bg = [[[NSGradient alloc] initWithStartingColor: rgb (236, 234, 228) endingColor: rgb (110, 108, 102)] autorelease];
    [bg drawInBezierPath: bezel angle: -60.0];
    NSBezierPath* lens = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (c.x - r, c.y - r, 2 * r, 2 * r)];
    if (lit)
    {
        NSColor* col = hot ? rgb (255, 40, 24) : amber();
        const CGFloat hr = 20.0;
        NSGradient* halo = [[[NSGradient alloc] initWithColorsAndLocations:
                               [col colorWithAlphaComponent: 0.8], 0.0, [col colorWithAlphaComponent: 0.25], 0.45, [col colorWithAlphaComponent: 0.0], 1.0, nil] autorelease];
        [halo drawInBezierPath: [NSBezierPath bezierPathWithOvalInRect: NSMakeRect (c.x - hr, c.y - hr, 2 * hr, 2 * hr)] relativeCenterPosition: NSMakePoint (0.0, 0.0)];
        NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: rgb (255, 244, 230), 0.0, col, 0.55, [col blendedColorWithFraction: 0.4 ofColor: [NSColor blackColor]], 1.0, nil] autorelease];
        [g drawInBezierPath: lens relativeCenterPosition: NSMakePoint (-0.2, 0.3)];
    }
    else
    {
        NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: rgb (150, 100, 40), 0.0, rgb (90, 56, 14), 0.55, rgb (40, 24, 6), 1.0, nil] autorelease];
        [g drawInBezierPath: lens relativeCenterPosition: NSMakePoint (-0.3, 0.4)];
    }
    [rgb (30, 20, 18) set];
    [lens setLineWidth: 0.8];
    [lens stroke];
}

- (void) drawKey: (NSRect) r state: (int) state
{
    NSBezierPath* bp = [NSBezierPath bezierPathWithRoundedRect: r xRadius: 2.0 yRadius: 2.0];
    if (state == 3)
    {
        glowFill (bp, rgb (255, 40, 24), 12.0);
        NSGradient* g = [[[NSGradient alloc] initWithColorsAndLocations: rgb (255, 210, 196), 0.0, rgb (255, 70, 46), 0.5, rgb (214, 20, 12), 1.0, nil] autorelease];
        [g drawInBezierPath: bp relativeCenterPosition: NSMakePoint (0.0, 0.0)];
    }
    else
    {
        shadowFill (bp, rgb (80, 78, 74), 1.0, -2.0, 2.5, 0.5);
        NSGradient* g = state == 1
            ? [[[NSGradient alloc] initWithStartingColor: rgb (84, 82, 78) endingColor: rgb (44, 43, 41)] autorelease]
            : [[[NSGradient alloc] initWithStartingColor: rgb (226, 223, 216) endingColor: rgb (184, 180, 172)] autorelease];
        [g drawInBezierPath: bp angle: -90.0];
    }
    [rgb (30, 29, 28) set];
    [bp setLineWidth: 1.0];
    [bp stroke];
}

- (void) drawPresets
{
    if (presetBank != [self inst]) [self refreshBank];
    [self drawKey: defaultRect() state: 1];
    drawText ("DEFAULT", boldFont (13.0), [NSColor whiteColor], NSMidX (defaultRect()), NSMidY (defaultRect()), 0.5);
    [self drawKey: saveRect() state: saveMode ? 3 : 1];
    drawText ("SAVE", boldFont (14.0), saveMode ? rgb (60, 10, 6) : [NSColor whiteColor], NSMidX (saveRect()), NSMidY (saveRect()));

    // the preset menu: tan like the INSTRUMENT menu; numbers only inside the list
    const NSRect r = presetRect();
    NSBezierPath* well = [NSBezierPath bezierPathWithRoundedRect: NSInsetRect (r, -2.0, -2.0) xRadius: 5.0 yRadius: 5.0];
    [rgb (30, 29, 28) set];
    [well fill];
    NSBezierPath* face = [NSBezierPath bezierPathWithRoundedRect: r xRadius: 4.0 yRadius: 4.0];
    NSGradient* g = saveMode
        ? [[[NSGradient alloc] initWithStartingColor: rgb (246, 232, 196) endingColor: rgb (226, 204, 160)] autorelease]
        : [[[NSGradient alloc] initWithStartingColor: rgb (232, 212, 170) endingColor: rgb (204, 178, 130)] autorelease];
    [g drawInBezierPath: face angle: -90.0];
    [rgb (40, 42, 38) set];
    [face setLineWidth: saveMode ? 2.0 : 1.0];
    if (saveMode) [rgb (214, 40, 24) set];
    [face stroke];
    if (! nameField)
    {
        char name[kMaxPresetName + 8] = { 0 };
        if (activePreset >= 0 && host) host->presetName (activePreset, name, (int) sizeof (name));
        if (activePreset >= 0 && name[0])
            drawText (name, boldFont (15.0), presetEdited ? rgb (120, 104, 76) : [NSColor blackColor], NSMidX (r) - 10.0, NSMidY (r) + 0.5, 0.4);
        else
            drawText ("PRESETS", boldFont (13.0), rgb (120, 104, 76), NSMidX (r) - 10.0, NSMidY (r) + 0.5, 1.5);
        NSBezierPath* dm = [NSBezierPath bezierPath];
        const CGFloat dx = NSMaxX (r) - 18.0, dy = NSMidY (r);
        [dm moveToPoint: NSMakePoint (dx - 6, dy - 3)]; [dm lineToPoint: NSMakePoint (dx + 6, dy - 3)]; [dm lineToPoint: NSMakePoint (dx, dy + 5)];
        [dm closePath];
        [ink() set];
        [dm fill];
    }
}

- (void) drawFitted: (const char*) text in: (NSRect) box
{
    NSString* s = [NSString stringWithUTF8String: text];
    CGFloat size = 18.0;
    while (size > 9.0 && [s sizeWithAttributes: textAttrs (boldFont (size), ink(), 0.0)].width > box.size.width - 14.0) size -= 0.5;
    drawText (text, boldFont (size), [NSColor blackColor], NSMidX (box), NSMidY (box));
}

- (void) drawReadout
{
    const NSRect box = readRect();
    NSBezierPath* p = [NSBezierPath bezierPathWithRect: box];
    NSGradient* glow = [[[NSGradient alloc] initWithStartingColor: rgb (232, 212, 170) endingColor: rgb (204, 178, 130)] autorelease];
    [glow drawInBezierPath: p relativeCenterPosition: NSMakePoint (0.0, 0.0)];
    [rgb (40, 42, 38) set];
    [p setLineWidth: 1.0];
    [p stroke];
    if (status[0]) [self drawFitted: status in: box];
    else if (readoutParam >= 0 && host)
    {
        // a control from another instrument's page: show the instrument instead
        const int pi = kParams[readoutParam].inst;
        if (pi != -1 && pi != [self inst]) readoutParam = kInst;
        char v[64], text[160];
        host->getParamText (readoutParam, v, sizeof (v));
        std::snprintf (text, sizeof (text), "%s   %s", kParams[readoutParam].label, v);
        [self drawFitted: text in: box];
    }
}

- (void) drawRect: (NSRect) dirty
{
    (void) dirty;
    const int inst = [self inst];
    [[self backdropFor: inst] drawInRect: [self bounds] fromRect: NSZeroRect operation: NSCompositeSourceOver fraction: 1.0 respectFlipped: YES hints: nil];
    [NSGraphicsContext saveGraphicsState];
    NSAffineTransform* t = [NSAffineTransform transform];
    [t translateXBy: kPanelFrame yBy: kPanelFrame];
    [t concat];
    for (int i = 0; i < kNumCtls; ++i)
    {
        const Ctl& c = kCtls[i];
        if (instOf (c) != inst && instOf (c) != -1) continue;
        const float v = shown[c.param];
        switch (c.type)
        {
            case kKnob:     [self drawKnobAt: NSMakePoint (c.x, c.y) radius: c.r style: c.style value: v]; break;
            case kSelector: [self drawSelector: c value: v]; break;
            case kRocker:   [self drawRocker: c on: isOn (v)]; break;
            case kTab:      [self drawTab: c on: isOn (v)]; break;
            case kDrawbar:  [self drawDrawbar: c value: v]; break;
            case kSlide:    [self drawSlide: c value: v]; break;
            case kVSlider:  [self drawVSlider: c value: v]; break;
            default: break;
        }
    }
    [self drawMenu];
    [self drawLevelLed];
    [self drawPresets];
    [self drawReadout];
    [NSGraphicsContext restoreGraphicsState];
}

//------------------------------------------------------------------------------
// Mouse

- (int) ctlAtPoint: (NSPoint) p
{
    const int inst = [self inst];
    for (int i = 0; i < kNumCtls; ++i)
    {
        const Ctl& c = kCtls[i];
        if (instOf (c) != inst && instOf (c) != -1) continue;
        const CGFloat dx = p.x - c.x, dy = p.y - c.y;
        switch (c.type)
        {
            case kKnob:     if (dx * dx + dy * dy < (c.r + 6.0) * (c.r + 6.0)) return i; break;
            case kSelector: if (dx * dx + dy * dy < (c.r + 24.0) * (c.r + 24.0)) return i; break;
            case kRocker:   if (std::fabs (dx) <= 13.0 && std::fabs (dy) <= 19.0) return i; break;
            case kTab:      if (std::fabs (dx) <= tabW (c) * 0.5 + 3.0 && std::fabs (dy) <= tabH (c) * 0.5 + 3.0) return i; break;
            case kDrawbar:  if (std::fabs (dx) <= kBarW * 0.5 + 2.0 && p.y >= kBarTop - 4.0 && p.y <= drawbarY (8) + kBarHandleH) return i; break;
            case kSlide:
            {
                const int n = stepsOf (c.param);
                if (std::fabs (dx) <= slideSpacing (n) * (n - 1) * 0.5 + 18.0 && std::fabs (dy) <= 14.0) return i;
                break;
            }
            case kVSlider:  if (std::fabs (dx) <= (c.style == kLever ? 15.0 : 22.0) && dy >= -c.r - 14.0 && dy <= c.r + 14.0) return i; break;
            default: break;
        }
    }
    return -1;
}

- (void) setParam: (int) param value: (float) v
{
    if (! host) return;
    v = v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
    host->guiSetParam (param, v);
    shown[param] = host->getParam (param);
    readoutParam = param;
    if (param != kInst) presetEdited = true;
    [self setNeedsDisplay: YES];
}

- (void) clickParam: (int) param value: (float) v
{
    host->guiBeginEdit (param);
    [self setParam: param value: v];
    host->guiEndEdit (param);
}

- (void) chooseInstrument: (int) k
{
    if (k < 0) k = kNumInstruments - 1;
    if (k >= kNumInstruments) k = 0;
    status[0] = 0;
    activePreset = -1;
    presetEdited = false;
    [self clickParam: kInst value: instValue (k)];
}

- (void) menuPicked: (id) sender
{
    [self chooseInstrument: (int) [sender tag]];
}

- (void) popUpInstrumentMenu
{
    NSMenu* m = [[[NSMenu alloc] initWithTitle: @"Instrument"] autorelease];
    [m setAutoenablesItems: NO];
    const int cur = [self inst];
    for (int k = 0; k < kNumInstruments; ++k)
    {
        NSMenuItem* it = [[[NSMenuItem alloc] initWithTitle: [NSString stringWithUTF8String: instrumentName (k)]
                                                     action: @selector (menuPicked:) keyEquivalent: @""] autorelease];
        [it setTarget: self];
        [it setTag: k];
        [it setState: k == cur ? NSOnState : NSOffState];
        [m addItem: it];
    }
    const NSRect r = faceToView (menuRect());
    [m popUpMenuPositioningItem: [m itemAtIndex: cur] atLocation: NSMakePoint (NSMinX (r) + kArrowW, NSMinY (r) + 4.0) inView: self];
}

- (void) dropRetired
{
    if (! retiredField) return;
    [retiredField setDelegate: nil];
    [retiredField setTarget: nil];
    [retiredField removeFromSuperview];
    [retiredField release];
    retiredField = nil;
    if (! nameField && [self window]) [[self window] makeFirstResponder: self];
    [self setNeedsDisplay: YES];
}

// SAVE, first press: the menu box becomes a name field
- (void) startNaming
{
    if (! host) return;
    if (nameField) return;
    saveMode = true;
    nameField = [[NSTextField alloc] initWithFrame: faceToView (NSInsetRect (presetRect(), 8.0, 8.0))];
    [nameField setBezeled: NO];
    [nameField setBordered: NO];
    [nameField setDrawsBackground: NO];
    [nameField setTextColor: [NSColor blackColor]];
    [nameField setFont: boldFont (15.0)];
    [nameField setAlignment: NSCenterTextAlignment];
    [nameField setFocusRingType: NSFocusRingTypeNone];
    [nameField setStringValue: @""];
    [[nameField cell] setPlaceholderString: @"TYPE A NAME, THEN SAVE"];
    [[nameField cell] setScrollable: YES];
    [nameField setTarget: self];
    [nameField setAction: @selector (nameEntered:)];
    [nameField setDelegate: self];
    [self addSubview: nameField];
    std::snprintf (status, sizeof (status), "NAME THE SOUND, THEN PRESS SAVE AGAIN");
    readoutParam = -1;
    [[self window] makeFirstResponder: nameField];
    [self setNeedsDisplay: YES];
}

// SAVE, second press (or Return): add it to the end of this instrument's menu
- (void) finishNaming: (BOOL) keep
{
    if (! nameField) return;
    std::string name;
    if (keep)
    {
        const char* utf8 = [[nameField stringValue] UTF8String];
        name = utf8 ? utf8 : "";
    }
    saveMode = false;
    [self dropRetired];
    retiredField = nameField;
    nameField = nil;
    [retiredField setDelegate: nil];
    [retiredField setTarget: nil];
    [self performSelector: @selector (dropRetired) withObject: nil afterDelay: 0.0];
    if (! keep || ! host) { status[0] = 0; [self setNeedsDisplay: YES]; return; }

    const int inst = [self inst];
    [self refreshBank];
    int slot = -1, count = 0;
    for (int k = 0; k < kSlotsPerBank; ++k)
    {
        if (presetUsed[k]) { ++count; continue; }
        if (slot < 0 && ! host->presetIsFactory (bankSlot (inst, k))) slot = bankSlot (inst, k);
    }
    if (slot < 0)
    {
        std::snprintf (status, sizeof (status), "THE MENU IS FULL (%d PRESETS)", (int) kSlotsPerBank);
        [self setNeedsDisplay: YES];
        return;
    }
    char fallback[32];
    std::snprintf (fallback, sizeof (fallback), "PRESET %d", count + 1);
    size_t a = name.find_first_not_of (" \t");
    if (a == std::string::npos) name = fallback;
    if (host->savePreset (slot) && host->renamePreset (slot, name.c_str()))
    {
        [self refreshBank];
        activePreset = slot;
        presetEdited = false;
        char shown2[kMaxPresetName + 8] = { 0 };
        host->presetName (slot, shown2, (int) sizeof (shown2));
        std::snprintf (status, sizeof (status), "SAVED: %s", shown2);
    }
    else std::snprintf (status, sizeof (status), "COULD NOT SAVE THE PRESET");
    [self setNeedsDisplay: YES];
}

- (void) nameEntered: (id) sender { (void) sender; [self finishNaming: YES]; }
- (BOOL) control: (NSControl*) c textView: (NSTextView*) tv doCommandBySelector: (SEL) sel
{
    (void) c; (void) tv;
    if (sel == @selector (cancelOperation:)) { [self finishNaming: NO]; return YES; }
    return NO;
}

// The menu: this instrument's presets, numbered in the list only
- (NSMenu*) presetMenu
{
    NSMenu* m = [[[NSMenu alloc] initWithTitle: @"Presets"] autorelease];
    [m setAutoenablesItems: NO];
    [self refreshBank];
    const int inst = [self inst];
    int number = 0;
    for (int k = 0; k < kSlotsPerBank; ++k)
    {
        if (! presetUsed[k]) continue;
        const int slot = bankSlot (inst, k);
        char name[kMaxPresetName + 8] = { 0 }, title[kMaxPresetName + 16];
        if (host) host->presetName (slot, name, (int) sizeof (name));
        std::snprintf (title, sizeof (title), "%d   %s", ++number, name[0] ? name : "(no name)");
        NSMenuItem* it = [[[NSMenuItem alloc] initWithTitle: [NSString stringWithUTF8String: title]
                                                     action: @selector (presetPicked:) keyEquivalent: @""] autorelease];
        [it setTarget: self];
        [it setTag: slot];
        [it setState: slot == activePreset ? NSOnState : NSOffState];
        [m addItem: it];
    }
    if (number == 0)
    {
        NSMenuItem* it = [[[NSMenuItem alloc] initWithTitle: @"No presets yet: press SAVE to add one" action: nil keyEquivalent: @""] autorelease];
        [it setEnabled: NO];
        [m addItem: it];
    }
    return m;
}

- (void) presetPicked: (id) sender
{
    if (! host) return;
    const int slot = (int) [sender tag];
    host->refreshPresets();
    if (host->presetExists (slot) && host->loadPreset (slot))
    {
        for (int j = 0; j < kNumParams; ++j) shown[j] = host->getParam (j);
        activePreset = slot;
        presetEdited = false;
        char name[kMaxPresetName + 8] = { 0 };
        host->presetName (slot, name, (int) sizeof (name));
        std::snprintf (status, sizeof (status), "%s", name);
        [self refreshBank];
    }
    [self setNeedsDisplay: YES];
}

// DEFAULT: every control of this instrument back to its default (other
// instruments and MASTER are left alone), to start a new sound
- (void) defaultSound
{
    if (! host) return;
    if (nameField) [self finishNaming: NO];
    const int inst = [self inst];
    for (int p = 0; p < kNumParams; ++p)
    {
        if (kParams[p].inst != inst) continue;
        host->guiBeginEdit (p);
        host->guiSetParam (p, kParams[p].def);
        host->guiEndEdit (p);
        shown[p] = host->getParam (p);
    }
    activePreset = -1;
    presetEdited = false;
    readoutParam = -1;
    std::snprintf (status, sizeof (status), "%s: DEFAULT", instrumentName (inst));
    [self setNeedsDisplay: YES];
}

- (BOOL) presetClick: (NSPoint) p
{
    if (NSPointInRect (p, defaultRect())) { [self defaultSound]; return YES; }
    if (NSPointInRect (p, saveRect()))
    {
        if (nameField) [self finishNaming: YES];
        else [self startNaming];
        return YES;
    }
    if (NSPointInRect (p, presetRect()))
    {
        if (nameField) return YES;          // typing the name: the field has the click
        NSMenu* m = [self presetMenu];
        const NSRect r = faceToView (presetRect());
        NSMenuItem* cur = nil;
        for (NSMenuItem* it in [m itemArray]) if ([it tag] == activePreset && [it action]) cur = it;
        [m popUpMenuPositioningItem: cur atLocation: NSMakePoint (NSMinX (r) + 4.0, cur ? NSMinY (r) + 4.0 : NSMaxY (r) + 2.0) inView: self];
        return YES;
    }
    return NO;
}

- (void) mouseDown: (NSEvent*) e
{
    const NSPoint p = viewToFace ([self convertPoint: [e locationInWindow] fromView: nil]);
    if (! host) return;
    // while naming, SAVE adds the preset; a click anywhere else cancels
    if (nameField && ! NSPointInRect (p, saveRect()) && ! NSPointInRect (p, presetRect()) && ! NSPointInRect (p, defaultRect()))
        [self finishNaming: NO];
    if (([e modifierFlags] & NSControlKeyMask) != 0) { [self showValueAt: p]; return; }
    if ([self presetClick: p]) return;

    // the INSTRUMENT menu
    if (NSPointInRect (p, menuLeft()))  { [self chooseInstrument: [self inst] - 1]; return; }
    if (NSPointInRect (p, menuRight())) { [self chooseInstrument: [self inst] + 1]; return; }
    if (NSPointInRect (p, menuRect()))  { readoutParam = kInst; [self popUpInstrumentMenu]; return; }

    const int i = [self ctlAtPoint: p];
    if (i < 0) return;
    const Ctl& c = kCtls[i];
    shown[c.param] = host->getParam (c.param);
    readoutParam = c.param;
    status[0] = 0;
    const int n = stepsOf (c.param);

    switch (c.type)
    {
        case kRocker:
        case kTab:
            [self clickParam: c.param value: isOn (shown[c.param]) ? 0.0f : 1.0f];
            return;
        case kSlide:
        {
            const CGFloat sp = slideSpacing (n);
            int k = (int) std::floor ((p.x - c.x) / sp + (n - 1) * 0.5 + 0.5);
            k = k < 0 ? 0 : (k > n - 1 ? n - 1 : k);
            if (k == stepOf (shown[c.param], n)) k = (k + 1) % n;     // a click on the lever steps it on
            [self clickParam: c.param value: stepValue (k, n)];
            return;
        }
        case kSelector:
        {
            // a click on a marked position goes there; on the knob it steps round
            const CGFloat dx = p.x - c.x, dy = p.y - c.y;
            int k = stepOf (shown[c.param], n);
            if (dx * dx + dy * dy > (c.r + 6.0) * (c.r + 6.0))
            {
                CGFloat best = 1e9;
                for (int j = 0; j < n; ++j)
                {
                    const NSPoint pl = polar (c.x, c.y, c.r + 17.0, selectorAngle (n, j));
                    const CGFloat d = (pl.x - p.x) * (pl.x - p.x) + (pl.y - p.y) * (pl.y - p.y);
                    if (d < best) { best = d; k = j; }
                }
                [self clickParam: c.param value: stepValue (k, n)];
                return;
            }
            host->guiBeginEdit (c.param);
            dragCtl = i;
            dragStartY = p.y;
            dragStartValue = shown[c.param];
            dragMoved = false;
            return;
        }
        default:
            break;
    }
    host->guiBeginEdit (c.param);
    if ([e clickCount] == 2) [self setParam: c.param value: kParams[c.param].def];
    dragCtl = i;
    dragStartY = p.y;
    dragStartValue = shown[c.param];
    dragMoved = false;
    [self setNeedsDisplay: YES];
}

- (void) showValueAt: (NSPoint) p
{
    if (! host) return;
    const int i = [self ctlAtPoint: p];
    if (i < 0) return;
    for (int j = 0; j < kNumParams; ++j) shown[j] = host->getParam (j);
    readoutParam = kCtls[i].param;
    status[0] = 0;
    [self setNeedsDisplay: YES];
}

- (void) rightMouseDown: (NSEvent*) e
{
    [self showValueAt: viewToFace ([self convertPoint: [e locationInWindow] fromView: nil])];
}

- (void) mouseDragged: (NSEvent*) e
{
    if (dragCtl < 0 || ! host) return;
    const Ctl& c = kCtls[dragCtl];
    const NSPoint p = viewToFace ([self convertPoint: [e locationInWindow] fromView: nil]);
    const bool fine = ([e modifierFlags] & NSShiftKeyMask) != 0;
    const int n = stepsOf (c.param);
    dragMoved = dragMoved || std::fabs (p.y - dragStartY) > 2.0;
    if (c.type == kDrawbar)
    {
        // the handle follows the hand down and up, a step at a time
        const int start = stepOf (dragStartValue, 9);
        int k = start + (int) std::floor ((p.y - dragStartY) / kBarStep + 0.5);
        k = k < 0 ? 0 : (k > 8 ? 8 : k);
        [self setParam: c.param value: stepValue (k, 9)];
        return;
    }
    if (c.type == kSelector)
    {
        int k = stepOf (dragStartValue, n) + (int) std::floor ((dragStartY - p.y) / 24.0 + 0.5);
        k = k < 0 ? 0 : (k > n - 1 ? n - 1 : k);
        [self setParam: c.param value: stepValue (k, n)];
        return;
    }
    const CGFloat span = c.type == kVSlider ? 2.0 * c.r : 200.0;
    const CGFloat perPixel = (fine ? 0.2 : 1.0) / span;
    [self setParam: c.param value: (float) (dragStartValue + (dragStartY - p.y) * perPixel)];
}

- (void) mouseUp: (NSEvent*) e
{
    (void) e;
    if (dragCtl >= 0 && host)
    {
        const Ctl& c = kCtls[dragCtl];
        if (c.type == kSelector && ! dragMoved)
        {
            const int n = stepsOf (c.param);
            [self setParam: c.param value: stepValue ((stepOf (shown[c.param], n) + 1) % n, n)];   // a click steps round
        }
        host->guiEndEdit (c.param);
    }
    dragCtl = -1;
}

- (void) scrollWheel: (NSEvent*) e
{
    const NSPoint p = viewToFace ([self convertPoint: [e locationInWindow] fromView: nil]);
    const int i = [self ctlAtPoint: p];
    if (i < 0 || ! host) return;
    const Ctl& c = kCtls[i];
    if (c.type == kRocker || c.type == kTab || c.type == kSlide) return;
    const CGFloat d = [e deltaY];
    if (d == 0.0) return;
    status[0] = 0;
    const int n = stepsOf (c.param);
    float v = shown[c.param];
    if (n >= 2)
    {
        const int dir = c.type == kDrawbar ? (d > 0 ? -1 : 1) : (d > 0 ? 1 : -1);
        int k = stepOf (v, n) + dir;
        k = k < 0 ? 0 : (k > n - 1 ? n - 1 : k);
        v = stepValue (k, n);
    }
    else v = (float) (v + d * 0.01);
    [self clickParam: c.param value: v];
}

@end

//==============================================================================
namespace tbsyn
{

void* editorOpen (void* parentView, EditorHost* host)
{
    NSView* parent = (NSView*) parentView;
    if (! parent || ! host) return 0;
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    TBSynthPanelView* v = [[TBSynthPanelView alloc] initWithFrame: NSMakeRect (0, 0, kEditorWidth, kEditorHeight) host: host];
    if (v) [parent addSubview: v];
    [pool release];
    return (void*) v;
}

void editorClose (void* handle)
{
    TBSynthPanelView* v = (TBSynthPanelView*) handle;
    if (! v) return;
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [v shutdown];
    [v removeFromSuperview];
    [v release];
    [pool release];
}

} // namespace tbsyn
