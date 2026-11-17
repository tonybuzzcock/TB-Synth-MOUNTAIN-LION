// FactoryPresets.h
// Presets that come with TB Synth, always on the first keys of their
// instrument's bank (they can't be saved over or renamed). A factory preset
// starts from every default and changes only what it lists. Plain C++98 data.

#ifndef TBSYNTH_FACTORY_PRESETS_H
#define TBSYNTH_FACTORY_PRESETS_H

#include "../DSP/SynthParams.h"

namespace tbsyn
{

struct FactorySetting { int param; float value; };
struct FactoryPreset
{
    int instrument, key;          // key 0 = the first preset key
    const char* name;
    FactorySetting settings[32];  // ends at param -1
};

const FactoryPreset kFactoryPresets[] = {
    // Wendy Carlos, Switched-On Bach (a Moog modular): a harpsichord voice, two narrow
    // pulses an octave apart like a harpsichord's 8' and 4' registers, a quick
    // plucked filter contour, no sustain
    { kMinimoog, 0, "Carlos", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 1.0f }, { kMgO2Rng, 0.8f }, { kMgO2Wave, 0.8f }, { kMgO2Freq, 0.5f },
        { kMgO1Vol, 0.8f }, { kMgO2Vol, 0.55f }, { kMgO3On, 0.0f }, { kMgNzOn, 0.0f },
        { kMgCutoff, 0.38f }, { kMgEmph, 0.15f }, { kMgContAm, 0.7f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgFAtk, 0.0f }, { kMgFDec, 0.38f }, { kMgFSus, 0.0f },
        { kMgLAtk, 0.0f }, { kMgLDec, 0.52f }, { kMgLSus, 0.0f }, { kMgDecay, 1.0f }, { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },

    // Queen, Flash Gordon / "Flash": big detuned brass, the filter opening on each stab
    { kOBX, 0, "Queen", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxO2Det, 0.6f },
        { kOxCut, 0.42f }, { kOxRes, 0.2f }, { kOxEnvAm, 0.62f }, { kOxKbd, 1.0f },
        { kOxFA, 0.18f }, { kOxFD, 0.52f }, { kOxFS, 0.45f }, { kOxFR, 0.45f },
        { kOxLA, 0.05f }, { kOxLS, 1.0f }, { kOxLR, 0.45f },
        { -1, 0.0f } } },

    // Japan, Tin Drum (Richard Barbieri): a dark pulse-wave pad, slow attack, the
    // pulse width moving slowly
    { kOBX, 1, "Japan", {
        { kOxO1Saw, 0.0f }, { kOxO1Pls, 1.0f }, { kOxO2Saw, 0.0f }, { kOxO2Pls, 1.0f }, { kOxO2Det, 0.55f }, { kOxPW, 0.35f },
        { kOxLfoRt, 0.3f }, { kOxModDp, 0.35f }, { kOxModO1, 0.0f }, { kOxModO2, 0.0f }, { kOxModPw, 1.0f },
        { kOxCut, 0.35f }, { kOxRes, 0.3f }, { kOxEnvAm, 0.25f },
        { kOxFA, 0.6f }, { kOxFS, 0.6f }, { kOxFR, 0.65f },
        { kOxLA, 0.55f }, { kOxLS, 0.9f }, { kOxLR, 0.7f },
        { -1, 0.0f } } },

    // John Foxx, The Garden: a lush shimmering pad, saw and pulse, a gentle vibrato
    { kOBX, 2, "Foxx", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 0.0f }, { kOxO2Pls, 1.0f }, { kOxPW, 0.25f }, { kOxO2Det, 0.64f },
        { kOxLfoRt, 0.7f }, { kOxModDp, 0.04f }, { kOxModO1, 1.0f }, { kOxModO2, 1.0f },
        { kOxCut, 0.5f }, { kOxRes, 0.15f }, { kOxEnvAm, 0.3f },
        { kOxFA, 0.45f }, { kOxFS, 0.7f }, { kOxFR, 0.6f },
        { kOxLA, 0.4f }, { kOxLS, 1.0f }, { kOxLR, 0.6f },
        { -1, 0.0f } } },

    // Supertramp, "Bloody Well Right" (Rick Davies' intro): dry, no vibrato,
    // played hard into an amp that's starting to grit
    { kWurlitzer, 0, "Supertramp", {
        { kWuVol, 0.85f }, { kWuVib, 0.0f },
        { -1, 0.0f } } },

    // Stevie Wonder, "Superstition": bright and punchy, both pickups in phase
    // for the body of the low riff, BRILLIANT + TREBLE for the snap
    { kClavinet, 0, "Superstition", {
        { kClBrill, 1.0f }, { kClTreb, 1.0f }, { kClMed, 0.0f }, { kClSoft, 0.0f },
        { kClCD, 1.0f }, { kClAB, 0.0f }, { kClMute, 0.0f }, { kClVol, 0.56f },
        { -1, 0.0f } } },

    // Suzi Quatro's keyboard player: a glam-rock clav, TREBLE + MEDIUM for a gritty
    // middle, the bridge pickup, a little mute for chunky staccato, the preamp pushed
    { kClavinet, 1, "Quatro", {
        { kClBrill, 0.0f }, { kClTreb, 1.0f }, { kClMed, 1.0f }, { kClSoft, 0.0f },
        { kClCD, 0.0f }, { kClAB, 1.0f }, { kClMute, 0.15f }, { kClVol, 0.82f },
        { -1, 0.0f } } },

    // Donna Summer, "I Feel Love" (Giorgio Moroder): the record's pad is the Moog
    // modular; this is the Solina's take on it, bright strings with a soft start
    { kSolina, 0, "Moroder", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f },
        { kSoCello, 1.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.48f }, { kSoSust, 0.55f }, { kSoVol, 0.7f },
        { -1, 0.0f } } },
    // The classic Solina sounds: settings worked out from the records (no published
    // settings exist for a preset-only machine like this; the tabs and slider positions are estimates)
    // Pink Floyd, Wish You Were Here (Rick Wright): full, slow-swelling strings that ring on
    { kSolina, 1, "Shine On", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 1.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.62f }, { kSoSust, 0.8f }, { kSoVol, 0.68f },
        { -1, 0.0f } } },
    // Gary Wright, "Dream Weaver": soft upper strings, gentle swell
    { kSolina, 2, "Dream Weaver", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 0.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.5f }, { kSoSust, 0.62f }, { kSoVol, 0.7f },
        { -1, 0.0f } } },
    // The Buggles, "Video Killed the Radio Star": bright, quick pop strings
    { kSolina, 3, "Video Killed", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 0.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.18f }, { kSoSust, 0.38f }, { kSoVol, 0.68f },
        { -1, 0.0f } } },
    // Elton John, "Someone Saved My Life Tonight": violins, violas and cellos
    { kSolina, 4, "Elton", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 1.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.42f }, { kSoSust, 0.58f }, { kSoVol, 0.68f },
        { -1, 0.0f } } },
    // Bee Gees, "Nights on Broadway": the whole section, cellos and basses in
    { kSolina, 5, "Bee Gees", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 1.0f }, { kSoBass, 1.0f }, { kSoCresc, 0.3f }, { kSoSust, 0.5f }, { kSoVol, 0.64f },
        { -1, 0.0f } } },
    // Parliament (Bernie Worrell): the Solina played as a solo voice, brassy and quick
    { kSolina, 6, "Chocolate City", {
        { kSoViolin, 0.0f }, { kSoViola, 1.0f }, { kSoTrump, 1.0f }, { kSoHorn, 0.0f }, { kSoCello, 0.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.12f }, { kSoSust, 0.32f }, { kSoVol, 0.7f },
        { -1, 0.0f } } },
    // Peter Frampton, "I'm in You" (Stevie Wonder's string line): violins and violas, singing
    { kSolina, 7, "I'm In You", {
        { kSoViolin, 1.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 0.0f }, { kSoCello, 0.0f }, { kSoBass, 0.0f }, { kSoCresc, 0.36f }, { kSoSust, 0.55f }, { kSoVol, 0.7f },
        { -1, 0.0f } } },
    // Pink Floyd, Animals, the middle of "Dogs": dark, low, very slow swell
    { kSolina, 8, "Dogs", {
        { kSoViolin, 0.0f }, { kSoViola, 1.0f }, { kSoTrump, 0.0f }, { kSoHorn, 1.0f }, { kSoCello, 1.0f }, { kSoBass, 1.0f }, { kSoCresc, 0.72f }, { kSoSust, 0.86f }, { kSoVol, 0.64f },
        { -1, 0.0f } } },
    // ---- ARP Omni. Names 2-10 are the patches in ARP's Omni-2 owner's manual (1978);
    // the settings are worked out from the patch names and the Omni's controls, not
    // copied from the manual's charts (estimates). "Joy Division" and "Moving In
    // Stereo" follow records the Omni is documented on.
    // Joy Division, "Atmosphere" / Closer: violins and violas through the phasers,
    // slow swell, long release
    { kOmni, 0, "Joy Division", {
        { kOmViolin, 1.0f }, { kOmViola, 1.0f }, { kOmCello, 1.0f }, { kOmBass, 0.0f },
        { kOmSAtk, 0.62f }, { kOmSRel, 0.82f }, { kOmMix, 0.0f }, { kOmBassV, 0.5f },
        { -1, 0.0f } } },
    { kOmni, 1, "Brass", {
        { kOmViolin, 0.0f }, { kOmViola, 0.0f }, { kOmSy8, 1.0f }, { kOmSy4, 0.0f }, { kOmWave, 0.0f },
        { kOmVcfF, 0.38f }, { kOmVcfR, 0.15f }, { kOmVcfEnv, 0.62f }, { kOmVcfLfo, 0.0f },
        { kOmA, 0.5f }, { kOmD, 0.55f }, { kOmS, 0.65f }, { kOmR, 0.45f }, { kOmSRel, 0.35f }, { kOmMix, 1.0f },
        { -1, 0.0f } } },
    { kOmni, 2, "Brass Strings Bass", {
        { kOmViolin, 1.0f }, { kOmViola, 1.0f }, { kOmCello, 1.0f }, { kOmBass, 1.0f }, { kOmSAtk, 0.4f }, { kOmSRel, 0.55f },
        { kOmSy8, 1.0f }, { kOmWave, 0.0f },
        { kOmVcfF, 0.4f }, { kOmVcfR, 0.15f }, { kOmVcfEnv, 0.6f }, { kOmA, 0.48f }, { kOmD, 0.55f }, { kOmS, 0.65f }, { kOmR, 0.45f },
        { kOmMix, 0.55f }, { kOmBassV, 0.7f }, { kOmMaster, 0.6f },
        { -1, 0.0f } } },
    { kOmni, 3, "Shimmer Sweep", {
        { kOmViolin, 0.0f }, { kOmViola, 0.0f }, { kOmSy8, 1.0f }, { kOmSy4, 1.0f }, { kOmWave, 0.0f },
        { kOmVcfF, 0.42f }, { kOmVcfR, 0.62f }, { kOmVcfEnv, 0.0f }, { kOmVcfLfo, 0.7f }, { kOmLfo, 0.3f },
        { kOmA, 0.3f }, { kOmD, 0.5f }, { kOmS, 1.0f }, { kOmR, 0.6f }, { kOmSRel, 0.6f }, { kOmMix, 1.0f },
        { -1, 0.0f } } },
    { kOmni, 4, "Phased Sweep", {
        { kOmViolin, 0.0f }, { kOmViola, 0.0f }, { kOmSy8, 1.0f }, { kOmSy4, 0.0f }, { kOmWave, 0.0f }, { kOmChorus, 1.0f },
        { kOmVcfF, 0.3f }, { kOmVcfR, 0.5f }, { kOmVcfEnv, 0.75f }, { kOmVcfLfo, 0.0f },
        { kOmA, 0.72f }, { kOmD, 0.72f }, { kOmS, 0.35f }, { kOmR, 0.65f }, { kOmSRel, 0.65f }, { kOmMix, 1.0f },
        { -1, 0.0f } } },
    { kOmni, 5, "Sweep Enh Strings", {
        { kOmViolin, 1.0f }, { kOmViola, 1.0f }, { kOmSAtk, 0.5f }, { kOmSRel, 0.7f },
        { kOmSy8, 1.0f }, { kOmWave, 0.0f }, { kOmEnh, 1.0f },
        { kOmVcfF, 0.35f }, { kOmVcfR, 0.45f }, { kOmVcfEnv, 0.0f }, { kOmVcfLfo, 0.6f }, { kOmLfo, 0.22f },
        { kOmA, 0.55f }, { kOmD, 0.5f }, { kOmS, 1.0f }, { kOmR, 0.65f }, { kOmMix, 0.5f },
        { -1, 0.0f } } },
    { kOmni, 6, "Poly Wah", {
        { kOmViolin, 0.0f }, { kOmViola, 0.0f }, { kOmSy8, 1.0f }, { kOmSy4, 0.0f }, { kOmWave, 1.0f },
        { kOmVcfF, 0.28f }, { kOmVcfR, 0.7f }, { kOmVcfEnv, 0.7f }, { kOmVcfLfo, 0.0f },
        { kOmA, 0.38f }, { kOmD, 0.48f }, { kOmS, 0.15f }, { kOmR, 0.35f }, { kOmSRel, 0.3f }, { kOmMix, 1.0f },
        { -1, 0.0f } } },
    { kOmni, 7, "Harpsichord Bass", {
        { kOmViolin, 0.0f }, { kOmViola, 1.0f }, { kOmCello, 1.0f }, { kOmBass, 0.0f }, { kOmSAtk, 0.35f }, { kOmSRel, 0.5f },
        { kOmSy8, 1.0f }, { kOmSy4, 1.0f }, { kOmWave, 1.0f },
        { kOmVcfF, 0.55f }, { kOmVcfR, 0.1f }, { kOmVcfEnv, 0.45f },
        { kOmA, 0.0f }, { kOmD, 0.55f }, { kOmS, 0.0f }, { kOmR, 0.5f }, { kOmMix, 0.62f }, { kOmBassV, 0.6f },
        { -1, 0.0f } } },
    { kOmni, 8, "Lower Strings", {
        { kOmViolin, 0.0f }, { kOmViola, 1.0f }, { kOmCello, 1.0f }, { kOmBass, 1.0f },
        { kOmSAtk, 0.55f }, { kOmSRel, 0.72f }, { kOmMix, 0.0f }, { kOmBassV, 0.72f },
        { -1, 0.0f } } },
    { kOmni, 9, "Lucy in the Sky", {
        { kOmViolin, 0.0f }, { kOmViola, 0.0f }, { kOmSy4, 1.0f }, { kOmSy8, 0.0f }, { kOmWave, 1.0f }, { kOmEnh, 1.0f },
        { kOmChorus, 1.0f }, { kOmVcfF, 0.62f }, { kOmVcfR, 0.1f }, { kOmVcfEnv, 0.15f },
        { kOmA, 0.05f }, { kOmD, 0.65f }, { kOmS, 0.55f }, { kOmR, 0.45f }, { kOmSRel, 0.4f }, { kOmMix, 1.0f },
        { -1, 0.0f } } },
    // The Cars, "Moving In Stereo" (Greg Hawkes: every keyboard on it is the Omni):
    // the dark, hollow synthesizer pad with a slow filter swell
    { kOmni, 10, "Moving In Stereo", {
        { kOmViolin, 1.0f }, { kOmViola, 0.0f }, { kOmSAtk, 0.6f }, { kOmSRel, 0.7f },
        { kOmSy8, 1.0f }, { kOmSy4, 0.0f }, { kOmSyB16, 1.0f }, { kOmWave, 1.0f }, { kOmEnh, 1.0f },
        { kOmVcfF, 0.33f }, { kOmVcfR, 0.35f }, { kOmVcfEnv, 0.55f }, { kOmVcfLfo, 0.1f }, { kOmLfo, 0.25f },
        { kOmA, 0.7f }, { kOmD, 0.7f }, { kOmS, 0.6f }, { kOmR, 0.65f }, { kOmMix, 0.7f }, { kOmBassV, 0.6f },
        { -1, 0.0f } } },
};
const int kNumFactoryPresets = (int) (sizeof (kFactoryPresets) / sizeof (kFactoryPresets[0]));

} // namespace tbsyn

#endif
