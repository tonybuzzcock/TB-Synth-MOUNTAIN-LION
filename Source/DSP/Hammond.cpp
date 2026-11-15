// Hammond.cpp
// Hammond B-3 (upper manual, 61 keys C2 - C7) and Leslie 122.
//
// Tonewheels: 91 wheels on a synchronous motor shaft (20 rev/s), each pitch
// set by the generator's twelve gear ratios and the wheel's teeth (2 .. 256),
// so the tuning is the Hammond's own slightly-off equal temperament. Wheels
// run all the time; keys only connect them, so phases never restart.
// Nine drawbars per key (16', 5 1/3', 8', 4', 2 2/3', 2', 1 3/5', 1 1/3',
// 1'), eight steps of about 3 dB, the top octave folding back as on the
// real generator. Each key has nine contacts that close a little apart, the
// source of the key click.
// Percussion (B-3 / C-3): single-triggered (only when all keys were up),
// second or third harmonic, NORMAL / SOFT, FAST / SLOW decay; with NORMAL the
// drawbars drop a little, and the 1' drawbar is taken by the percussion, as on
// the real organ.
// Scanner vibrato and chorus V1 C1 V2 C2 V3 C3 (a swept delay at the
// scanner's 6.87 Hz; chorus mixes it with the dry signal), the AO-28 preamp
// with DRIVE and the EXPRESSION pedal before it, so playing louder drives it
// harder (like the Leslie amplifier).
// Leslie 122: an 800 Hz crossover; the treble horn and the bass drum rotor
// each spin up and down at their own rate (horn fast, drum slow), with
// Doppler and the changing loudness and brightness seen by one microphone on
// each, summed to mono. STOP brakes both rotors.
// Rotor speeds and accelerations, vibrato depths and percussion decays are
// estimates (see README); the wheel frequencies follow the gear ratios.

#include "Engines.h"

namespace tbsyn
{

namespace
{
// Gear ratios C .. B (driving / driven teeth on the Hammond generator)
const double kRatio[12] = { 85.0 / 104.0, 71.0 / 82.0, 67.0 / 73.0, 105.0 / 108.0, 103.0 / 100.0, 84.0 / 77.0,
                            74.0 / 64.0, 98.0 / 80.0, 96.0 / 74.0, 88.0 / 64.0, 67.0 / 46.0, 108.0 / 70.0 };
const double kShaftHz = 20.0;
const int kBarOffset[9] = { -12, 7, 0, 12, 19, 24, 28, 31, 36 };   // semitones from the key's 8' wheel
const double kScannerHz = 6.87;                                    // 412 rpm scanner
const double kVibMs[3] = { 0.22, 0.42, 0.70 };                     // V1, V2, V3 depth (estimates)
const double kPercFast = 0.5, kPercSlow = 2.4;                     // decay to -60 dB (estimates)
const double kHornSlow = 0.83, kHornFast = 6.7, kDrumSlow = 0.67, kDrumFast = 5.8;   // rev/s (estimates)
const double kHornUp = 0.75, kHornDown = 0.95, kDrumUp = 4.5, kDrumDown = 5.5;       // seconds (estimates)
}

HammondEngine::HammondEngine()
    : sr (44100.0), keysHeld (0), numPending (0), sampleClock (0), percEnv (0.0), percK (0.0), smoothK (1.0),
      scanPh (0.0), hornSpeed (kHornSlow), drumSpeed (kDrumSlow), hornAngle (0.0), drumAngle (1.3), wheel (0.0)
{
    for (int i = 0; i < kNumParams; ++i) pv[i] = kParams[i].def;
    noise.seed (9119u);
    for (int w = 0; w <= kWheels; ++w)
    {
        wheelPh[w] = 0.0; wheelInc[w] = 0.0; wheelAmp[w] = 0.0; target[w] = level[w] = percWheel[w] = 0.0;
    }
    for (int w = 1; w <= kWheels; ++w) wheelPh[w] = noise.uniform();
    std::memset (keyDown, 0, sizeof (keyDown));
    std::memset (contact, 0, sizeof (contact));
    for (int b = 0; b < kBars; ++b) barGain[b] = 0.0;
}

void HammondEngine::prepare (double s)
{
    sr = s;
    for (int w = 1; w <= kWheels; ++w)
    {
        const int sem = (w - 1) % 12;
        const int oct = (w - 1) / 12;
        const double teeth = 2.0 * (double) (1 << oct);
        const double hz = kShaftHz * kRatio[sem] * teeth;
        wheelInc[w] = hz < 0.45 * sr ? hz / sr : 0.0;
        // the top wheels come out a little weaker; the lowest a little stronger
        wheelAmp[w] = w < 13 ? 1.1 : (w > 72 ? 1.0 - 0.012 * (w - 72) : 1.0);
    }
    smoothK = 1.0 - std::exp (-1.0 / (0.00008 * sr));    // contacts close in about 0.1 ms
    preDc.set (20.0, sr);
    preLp.set (9000.0, sr);
    xoLo1.lowpass (800.0, 0.7071, sr); xoLo2.lowpass (800.0, 0.7071, sr);
    xoHi1.highpass (800.0, 0.7071, sr); xoHi2.highpass (800.0, 0.7071, sr);
    hornLp.lowpass (7500.0, 0.7, sr);
    drumLp.lowpass (2500.0, 0.7, sr);
    dcBlock.set (10.0, sr);
    volS.setTime (0.02, sr); volS.v = pv[kB3Vol];
    silence();
}

int HammondEngine::wheelFor (int key, int bar) const
{
    int w = 13 + key + kBarOffset[bar];      // key 0 (C2) 8' = wheel 13 (C, 65.4 Hz)
    while (w > kWheels) w -= 12;             // foldback in the top octave
    while (w < 1) w += 12;
    return w;
}

void HammondEngine::rebuildTargets()
{
    const bool perc = isOn (pv[kB3Perc]);
    const bool normal = paramStep (kB3PVol, pv[kB3PVol]) == 0;
    for (int b = 0; b < kBars; ++b) barGain[b] = map::drawbarGain (paramStep (kB3Db1 + b, pv[kB3Db1 + b]));
    if (perc) barGain[8] = 0.0;                                  // the 1' drawbar goes to the percussion
    const double drop = (perc && normal) ? 0.708 : 1.0;          // about -3 dB with NORMAL percussion (estimate)
    for (int w = 0; w <= kWheels; ++w) { target[w] = 0.0; percWheel[w] = 0.0; }
    const int harm = paramStep (kB3PHarm, pv[kB3PHarm]) == 0 ? 3 : 4;    // 4' (second) or 2 2/3' (third)
    for (int k = 0; k < kKeys; ++k)
    {
        for (int b = 0; b < kBars; ++b)
            if (contact[k][b]) target[wheelFor (k, b)] += barGain[b] * drop;
        if (keyDown[k]) percWheel[wheelFor (k, harm)] += 1.0;
    }
}

void HammondEngine::setParam (int p, float v)
{
    if (p < 0 || p >= kNumParams) return;
    pv[p] = v;
    if ((p >= kB3Db1 && p <= kB3Db9) || p == kB3Perc || p == kB3PVol || p == kB3PHarm) rebuildTargets();
}

void HammondEngine::noteOn (int note, int)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys || keyDown[k]) return;
    if (keysHeld == 0 && isOn (pv[kB3Perc]))
    {
        percEnv = 1.0;      // single trigger: only from all keys up
        const double t = paramStep (kB3PDec, pv[kB3PDec]) == 1 ? kPercFast : kPercSlow;
        percK = std::exp (-std::log (1000.0) / (t * sr));
    }
    keyDown[k] = true;
    ++keysHeld;
    // nine contacts close over the first few milliseconds, in no fixed order
    for (int b = 0; b < kBars && numPending < kPending; ++b)
    {
        Contact& c = pending[numPending++];
        c.key = k; c.bar = b; c.close = true;
        c.when = sampleClock + (int) (noise.uniform() * 0.0035 * sr);
    }
    rebuildTargets();
}

void HammondEngine::noteOff (int note)
{
    const int k = note - kLowKey;
    if (k < 0 || k >= kKeys || ! keyDown[k]) return;
    keyDown[k] = false;
    --keysHeld;
    for (int b = 0; b < kBars && numPending < kPending; ++b)
    {
        Contact& c = pending[numPending++];
        c.key = k; c.bar = b; c.close = false;
        c.when = sampleClock + (int) (noise.uniform() * 0.002 * sr);
    }
    rebuildTargets();
}

void HammondEngine::allNotesOff()
{
    for (int k = 0; k < kKeys; ++k) { keyDown[k] = false; for (int b = 0; b < kBars; ++b) contact[k][b] = false; }
    keysHeld = 0;
    numPending = 0;
    rebuildTargets();
}

void HammondEngine::silence()
{
    allNotesOff();
    for (int w = 0; w <= kWheels; ++w) level[w] = 0.0;
    percEnv = 0.0;
    scanner.clear(); hornLine.clear(); drumLine.clear();
    xoLo1.reset(); xoLo2.reset(); xoHi1.reset(); xoHi2.reset(); hornLp.reset(); drumLp.reset();
    preDc.reset(); preLp.reset(); dcBlock.reset();
}

void HammondEngine::render (float* out, int n)
{
    const SineTable& sine = sineTable();
    const bool vib = isOn (pv[kB3VibOn]);
    const int vsel = paramStep (kB3VibSel, pv[kB3VibSel]);
    const double vibMs = kVibMs[vsel / 2];
    const bool chorus = (vsel % 2) == 1;
    const double drive = 1.0 + 11.0 * pv[kB3Drive] * pv[kB3Drive];
    const bool leslie = isOn (pv[kLsOn]);
    int speed = paramStep (kLsSpeed, pv[kLsSpeed]);
    if (wheel > 0.5 && speed > 0) speed = 2;                   // the modulation wheel kicks it to FAST
    const double hornT = speed == 0 ? 0.0 : (speed == 1 ? kHornSlow : kHornFast);
    const double drumT = speed == 0 ? 0.0 : (speed == 1 ? kDrumSlow : kDrumFast);
    const double hornK = 1.0 - std::exp (-1.0 / ((hornT > hornSpeed ? kHornUp : kHornDown) * sr / 3.0));
    const double drumK = 1.0 - std::exp (-1.0 / ((drumT > drumSpeed ? kDrumUp : kDrumDown) * sr / 3.0));
    const double percGain = isOn (pv[kB3Perc]) ? (paramStep (kB3PVol, pv[kB3PVol]) == 0 ? 1.0 : 0.5) : 0.0;
    const double msToS = sr / 1000.0;

    for (int i = 0; i < n; ++i)
    {
        // contacts due now
        if (numPending)
        {
            bool changed = false;
            for (int c = 0; c < numPending; )
            {
                if (pending[c].when <= sampleClock)
                {
                    // a closing contact only matters while its key is still down (and vice versa)
                    const Contact& pc = pending[c];
                    if (pc.close == keyDown[pc.key]) { contact[pc.key][pc.bar] = pc.close; changed = true; }
                    pending[c] = pending[--numPending];
                }
                else ++c;
            }
            if (changed) rebuildTargets();
        }
        ++sampleClock;
        if (sampleClock > 0x40000000) { for (int c = 0; c < numPending; ++c) pending[c].when -= sampleClock; sampleClock = 0; }

        // the generator
        double x = 0.0, perc = 0.0;
        for (int w = 1; w <= kWheels; ++w)
        {
            wheelPh[w] += wheelInc[w];
            if (wheelPh[w] >= 1.0) wheelPh[w] -= 1.0;
            level[w] += (target[w] - level[w]) * smoothK;
            const bool hasPerc = percWheel[w] > 0.0 && percEnv > 1e-5;
            if (level[w] < 1e-7 && ! hasPerc) continue;
            const double s = sine (wheelPh[w]) * wheelAmp[w];
            x += s * level[w];
            if (hasPerc) perc += s * percWheel[w];
        }
        x += perc * percEnv * percGain * 1.8;
        percEnv *= percK;
        x *= 0.11;

        // scanner vibrato / chorus
        scanner.push (x);
        if (vib)
        {
            scanPh = wrap01 (scanPh + kScannerHz / sr);
            const double d = (0.9 + vibMs * (0.5 + 0.5 * std::sin (kTwoPi * scanPh))) * msToS;
            const double wet = scanner.read (d);
            x = chorus ? 0.5 * (x + wet) * 1.2 : wet;
        }

        // expression, then the preamp's drive (a tube stage rounding one side first)
        const double ex = volS.tick (pv[kB3Vol]);
        double y = x * (ex / 0.8) * drive;
        y = softClip (y + 0.12 * y * y);
        y = preLp.lp (preDc.hp (y)) / std::sqrt (drive);

        // Leslie 122
        if (leslie)
        {
            hornSpeed += (hornT - hornSpeed) * hornK;
            drumSpeed += (drumT - drumSpeed) * drumK;
            hornAngle = wrap01 (hornAngle + hornSpeed / sr);
            drumAngle = wrap01 (drumAngle + drumSpeed / sr);
            const double lo = xoLo2.tick (xoLo1.tick (y));
            const double hi = xoHi2.tick (xoHi1.tick (y));
            hornLine.push (hi);
            drumLine.push (lo);
            const double ch = std::cos (kTwoPi * hornAngle), sh = std::sin (kTwoPi * hornAngle);
            const double cd = std::cos (kTwoPi * drumAngle), sd = std::sin (kTwoPi * drumAngle);
            double h = hornLine.read ((0.8 + 0.36 * sh) * msToS);       // horn mouth about 18 cm from the shaft
            h = hornLp.tick (h) * (0.6 + 0.4 * ch) + h * 0.25 * (0.5 + 0.5 * ch);
            double d = drumLine.read ((0.5 + 0.18 * sd) * msToS);
            d = drumLp.tick (d) * (0.72 + 0.28 * cd);
            y = 0.85 * softClip ((h * 1.1 + d) * 0.9 / 0.85);
        }
        out[i] = (float) dcBlock.hp (y * 0.7);
    }
}

} // namespace tbsyn
