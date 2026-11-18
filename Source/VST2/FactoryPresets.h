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

    // Rush, "Tom Sawyer" (Moving Pictures, Geddy Lee's OB-X): the growl, two detuned saws through a resonant filter swept slowly by the LFO
    { kOBX, 3, "Tom Sawyer", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.55f },
        { kOxModO1, 0.0f }, { kOxModO2, 0.0f }, { kOxModFl, 1.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.63f },
        { kOxCut, 0.3f }, { kOxRes, 0.72f }, { kOxEnvAm, 0.25f }, { kOxLfoRt, 0.15f }, { kOxLfoWv, 0.0f },
        { kOxFA, 0.6f }, { kOxFD, 0.7f }, { kOxFS, 0.6f }, { kOxLA, 0.2f }, { kOxLS, 1.0f },
        { kOxLR, 0.6f },
        { -1, 0.0f } } },

    // Rush, "Subdivisions" (Signals): lush, slowly opening chords with a little vibrato
    { kOBX, 4, "Subdivisions", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.06f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.64f },
        { kOxCut, 0.48f }, { kOxRes, 0.15f }, { kOxEnvAm, 0.35f }, { kOxLfoRt, 0.62f }, { kOxFA, 0.55f },
        { kOxFD, 0.65f }, { kOxFS, 0.6f }, { kOxFR, 0.6f }, { kOxLA, 0.45f }, { kOxLS, 1.0f },
        { kOxLR, 0.6f },
        { -1, 0.0f } } },

    // Prince, "1999" (first recorded on an OB-X): wide detuned saws, the LFO gently moving the oscillators, punchy
    { kOBX, 5, "1999", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.1f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.67f },
        { kOxCut, 0.52f }, { kOxRes, 0.2f }, { kOxEnvAm, 0.45f }, { kOxLfoRt, 0.6f }, { kOxFA, 0.2f },
        { kOxFD, 0.55f }, { kOxFS, 0.5f }, { kOxFR, 0.45f }, { kOxLA, 0.05f }, { kOxLS, 0.9f },
        { kOxLR, 0.4f },
        { -1, 0.0f } } },

    // Prince, "When You Were Mine" (Dirty Mind, the riff on the OB-X): thin, organ-like pulse, an octave apart
    { kOBX, 6, "When You Were Mine", {
        { kOxO1Saw, 0.0f }, { kOxO1Pls, 1.0f }, { kOxO2Saw, 0.0f }, { kOxO2Pls, 1.0f }, { kOxModDp, 0.0f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxPW, 0.35f },
        { kOxO2Frq, 0.75f }, { kOxO2Det, 0.55f }, { kOxCut, 0.62f }, { kOxRes, 0.1f }, { kOxEnvAm, 0.15f },
        { kOxFA, 0.0f }, { kOxFD, 0.5f }, { kOxFS, 0.8f }, { kOxLA, 0.0f }, { kOxLS, 1.0f },
        { kOxLR, 0.25f },
        { -1, 0.0f } } },

    // Eurythmics, "Sweet Dreams (Are Made of This)": the soft, reedy pad, pulse and saw
    { kOBX, 7, "Sweet Dreams", {
        { kOxO1Saw, 0.0f }, { kOxO1Pls, 1.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.0f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxPW, 0.2f },
        { kOxO2Det, 0.6f }, { kOxCut, 0.45f }, { kOxRes, 0.3f }, { kOxEnvAm, 0.3f }, { kOxFA, 0.4f },
        { kOxFD, 0.6f }, { kOxFS, 0.5f }, { kOxFR, 0.55f }, { kOxLA, 0.3f }, { kOxLS, 0.9f },
        { kOxLR, 0.55f },
        { -1, 0.0f } } },

    // Nena, "99 Luftballons": bright synth stabs that fall back quickly
    { kOBX, 8, "99 Luftballons", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.0f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.6f },
        { kOxCut, 0.45f }, { kOxRes, 0.25f }, { kOxEnvAm, 0.55f }, { kOxFA, 0.1f }, { kOxFD, 0.5f },
        { kOxFS, 0.3f }, { kOxFR, 0.4f }, { kOxLA, 0.02f }, { kOxLD, 0.6f }, { kOxLS, 0.6f },
        { kOxLR, 0.35f },
        { -1, 0.0f } } },

    // Jean-Michel Jarre, who used the OB-X for its brass: a slow-blooming brass section
    { kOBX, 9, "Jarre Brass", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.05f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.62f },
        { kOxCut, 0.36f }, { kOxRes, 0.18f }, { kOxEnvAm, 0.62f }, { kOxLfoRt, 0.6f }, { kOxFA, 0.5f },
        { kOxFD, 0.6f }, { kOxFS, 0.55f }, { kOxFR, 0.5f }, { kOxLA, 0.35f }, { kOxLS, 0.9f },
        { kOxLR, 0.5f },
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
    // ---- Yamaha SK-30: no published patch settings were readable (the manual's
    // SOUND MEMO pages are blank charts), so these are built from the manual's
    // advice and the 1981 review's descriptions (estimates).
    // Hammond-style 16' / 5 1/3' / 8' with the 3rd percussion, slow tremolo
    { kSK30, 0, "Jazz Organ", {
        { kSkOrgSel, 1.0f }, { kSkO16, 1.0f }, { kSkO8, 1.0f }, { kSkO513, 1.0f }, { kSkO4, 0.0f },
        { kSkO2, 0.0f }, { kSkPerc3, 0.7f }, { kSkPercDec, 0.35f }, { kSkOrgTrem, 1.0f }, { kSkTrFast, 0.0f },
        { kSkOrgV, 0.75f },
        { -1, 0.0f } } },
    // every tone lever out, bright, fast tremolo
    { kSK30, 1, "Gospel Organ", {
        { kSkOrgSel, 1.0f }, { kSkO16, 1.0f }, { kSkO8, 1.0f }, { kSkO513, 1.0f }, { kSkO4, 1.0f },
        { kSkO223, 0.8f }, { kSkO2, 0.8f }, { kSkO135, 0.5f }, { kSkO113, 0.6f }, { kSkO1, 0.9f },
        { kSkOrgBril, 0.65f }, { kSkOrgTrem, 1.0f }, { kSkTrFast, 1.0f }, { kSkOrgV, 0.62f },
        { -1, 0.0f } } },
    // ORGAN 3 with the ensemble (the 1981 review's "flat-out" home-organ sound)
    { kSK30, 2, "Home Organ", {
        { kSkOrgSel, 0.75f }, { kSkOrgEns, 1.0f }, { kSkOrgV, 0.7f },
        { -1, 0.0f } } },
    // POLY-SYNTH 2, the square-wave voice the manual recommends for clarinet, delayed vibrato
    { kSK30, 3, "Clarinet", {
        { kSkOrgV, 0.0f }, { kSkPolySel, 0.5f }, { kSkPolyV, 0.75f }, { kSkVibDp, 0.35f }, { kSkVibDl, 0.3f },
        { kSkVibSp, 0.45f },
        { -1, 0.0f } } },
    // MANUAL poly-synth: MODE II detuned saws, the filter following the EG for brass
    { kSK30, 4, "Brass Section", {
        { kSkOrgV, 0.0f }, { kSkPolySel, 1.0f }, { kSkWave, 0.0f }, { kSkMode, 1.0f }, { kSkDetune, 0.62f },
        { kSkCut, 0.32f }, { kSkRes, 0.2f }, { kSkEgDp, 0.7f }, { kSkEgA, 0.45f }, { kSkEgD, 0.6f },
        { kSkPBril, 0.55f }, { kSkPolyV, 0.75f },
        { -1, 0.0f } } },
    // STRING 2 (violas), MODE II, sustain and delayed vibrato
    { kSK30, 5, "Symphonic Strings", {
        { kSkOrgV, 0.0f }, { kSkStrSel, 0.6667f }, { kSkStrV, 0.75f }, { kSkMode, 1.0f }, { kSkDetune, 0.6f },
        { kSkVibDp, 0.3f }, { kSkVibDl, 0.35f }, { kSkPSus, 1.0f }, { kSkPSusT, 0.6f },
        { -1, 0.0f } } },
    // STRING 1 (cellos), long sustain
    { kSK30, 6, "Cello Section", {
        { kSkOrgV, 0.0f }, { kSkStrSel, 0.3333f }, { kSkStrV, 0.8f }, { kSkPSus, 1.0f }, { kSkPSusT, 0.55f },
        { kSkVibDp, 0.25f },
        { -1, 0.0f } } },
    // the solo synthesizer: 8' saw, a little resonance, portamento
    { kSK30, 7, "Solo Lead", {
        { kSkOrgV, 0.0f }, { kSkSoloV, 0.75f }, { kSkSoFeet, 0.6f }, { kSkSoWave, 0.2f }, { kSkSoCut, 0.55f },
        { kSkSoRes, 0.35f }, { kSkSoEgDp, 0.4f }, { kSkSoVca, 1.0f }, { kSkSoA, 0.05f }, { kSkSoD, 0.45f },
        { kSkSoS, 0.7f }, { kSkSoR, 0.35f }, { kSkSoPorta, 0.3f },
        { -1, 0.0f } } },
    // MANUAL BASS: a 16' solo bass on the lowest 19 keys under STRING 3 (violins)
    { kSK30, 8, "Bass and Strings", {
        { kSkOrgV, 0.0f }, { kSkSoMode, 1.0f }, { kSkSoFeet, 0.4f }, { kSkSoWave, 0.6f }, { kSkSoPW, 0.3f },
        { kSkSoCut, 0.35f }, { kSkSoRes, 0.3f }, { kSkSoEgDp, 0.5f }, { kSkSoVca, 1.0f }, { kSkSoA, 0.0f },
        { kSkSoD, 0.5f }, { kSkSoS, 0.4f }, { kSkSoR, 0.2f }, { kSkSoloV, 0.8f }, { kSkStrSel, 1.0f },
        { kSkStrV, 0.6f }, { kSkPSus, 1.0f }, { kSkPSusT, 0.5f },
        { -1, 0.0f } } },
    // violas with a 4' poly-synth layer through the ensemble (an owner's SS-30-style stack)
    { kSK30, 9, "Big Strings", {
        { kSkOrgV, 0.0f }, { kSkStrSel, 0.6667f }, { kSkStrV, 0.7f }, { kSkPolySel, 1.0f }, { kSkFeet, 1.0f },
        { kSkWave, 0.0f }, { kSkCut, 0.55f }, { kSkRes, 0.1f }, { kSkEgDp, 0.1f }, { kSkEgA, 0.5f },
        { kSkPolyV, 0.45f }, { kSkSlow, 1.0f }, { kSkPEns, 1.0f }, { kSkMode, 1.0f }, { kSkDetune, 0.62f },
        { kSkPSus, 1.0f }, { kSkPSusT, 0.65f }, { kSkVibDp, 0.2f },
        { -1, 0.0f } } },
    // Vangelis, Blade Runner "Main Titles" (a Yamaha CS-80, the SK-30's big brother): two detuned
    // saws into a resonant low-pass, slow attack and long release, delayed vibrato; add a big reverb
    { kSK30, 10, "Vangel Blade", {
        { kSkOrgV, 0.0f }, { kSkPolySel, 1.0f }, { kSkWave, 0.0f }, { kSkFeet, 0.5f }, { kSkMode, 1.0f },
        { kSkDetune, 0.64f }, { kSkCut, 0.3f }, { kSkRes, 0.45f }, { kSkEgDp, 0.55f }, { kSkEgA, 0.68f },
        { kSkEgD, 0.82f }, { kSkPBril, 0.55f }, { kSkSlow, 1.0f }, { kSkPSus, 1.0f }, { kSkPSusT, 0.85f },
        { kSkPEns, 1.0f }, { kSkVibDl, 0.45f }, { kSkVibSp, 0.4f }, { kSkVibDp, 0.25f }, { kSkPolyV, 0.8f },
        { -1, 0.0f } } },
    // ---- Minimoog Model D: leads from 1977-82 (Numan, Ultravox, Bowie, New Romantic); settings
    // worked out from the records (estimates)
    // Tubeway Army / Gary Numan, "Are 'Friends' Electric?" (1979): the lead line, two slightly detuned 8' saws, a touch of emphasis
    { kMinimoog, 1, "Friends Electric", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.52f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.85f },
        { kMgO2Vol, 0.8f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.55f }, { kMgEmph, 0.3f },
        { kMgContAm, 0.4f }, { kMgFAtk, 0.1f }, { kMgFDec, 0.5f }, { kMgFSus, 0.6f }, { kMgLAtk, 0.05f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },
    // Gary Numan, "Cars" (1979): the riff, saw and square an octave apart, punchy filter
    { kMinimoog, 2, "Cars Lead", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.4f }, { kMgO2Wave, 0.6f }, { kMgO2Freq, 0.5f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.85f },
        { kMgO2Vol, 0.65f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.5f }, { kMgEmph, 0.25f },
        { kMgContAm, 0.45f }, { kMgFAtk, 0.03f }, { kMgFDec, 0.45f }, { kMgFSus, 0.5f }, { kMgLAtk, 0.02f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },
    // Gary Numan, "Metal" (The Pleasure Principle, 1979): a hard, buzzing pulse lead with resonance
    { kMinimoog, 3, "Metal", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.8f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 1.0f }, { kMgO2Freq, 0.51f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.5f }, { kMgEmph, 0.55f },
        { kMgContAm, 0.35f }, { kMgFAtk, 0.02f }, { kMgFDec, 0.4f }, { kMgFSus, 0.55f }, { kMgLAtk, 0.02f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },
    // Ultravox, "Vienna" (1980, one of Moog's Model D anthems): a smooth, singing lead with slow glide
    { kMinimoog, 4, "Vienna", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.2f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.515f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.6f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.5f }, { kMgEmph, 0.2f },
        { kMgContAm, 0.35f }, { kMgFAtk, 0.12f }, { kMgFDec, 0.55f }, { kMgFSus, 0.7f }, { kMgLAtk, 0.08f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.36f },
        { -1, 0.0f } } },
    // David Bowie, "Warszawa" (Low, 1977): a mournful, hollow square lead with long glide
    { kMinimoog, 5, "Warszawa", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.6f }, { kMgO2Rng, 0.4f }, { kMgO2Wave, 0.0f }, { kMgO2Freq, 0.5f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.45f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.45f }, { kMgEmph, 0.15f },
        { kMgContAm, 0.3f }, { kMgFAtk, 0.2f }, { kMgFDec, 0.6f }, { kMgFSus, 0.7f }, { kMgLAtk, 0.12f },
        { kMgLDec, 0.55f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.42f },
        { -1, 0.0f } } },
    // The Buggles, "Video Killed the Radio Star" (1979): the bright, chirpy lead
    { kMinimoog, 6, "Video Killed", {
        { kMgO1Rng, 0.8f }, { kMgO1Wave, 0.6f }, { kMgO2Rng, 0.8f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.506f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.6f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.6f }, { kMgEmph, 0.3f },
        { kMgContAm, 0.4f }, { kMgFAtk, 0.02f }, { kMgFDec, 0.4f }, { kMgFSus, 0.55f }, { kMgLAtk, 0.02f },
        { kMgLDec, 0.45f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },
    // Steve Winwood, "While You See a Chance" (1980): the fat, brassy saw lead
    { kMinimoog, 7, "While You See a Chance", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.515f },
        { kMgO3On, 1.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.4f }, { kMgO3Freq, 0.49f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.5f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.47f }, { kMgEmph, 0.25f },
        { kMgContAm, 0.55f }, { kMgFAtk, 0.2f }, { kMgFDec, 0.55f }, { kMgFSus, 0.6f }, { kMgLAtk, 0.08f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.2f },
        { -1, 0.0f } } },
    // Giorgio Moroder, "Chase" (1978): the soaring lead over the sequence
    { kMinimoog, 8, "Chase", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.8f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.53f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.85f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.58f }, { kMgEmph, 0.4f },
        { kMgContAm, 0.45f }, { kMgFAtk, 0.05f }, { kMgFDec, 0.5f }, { kMgFSus, 0.65f }, { kMgLAtk, 0.03f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.22f },
        { -1, 0.0f } } },
    // Rush, "Tom Sawyer" (1981): Geddy Lee's searing Minimoog lead
    { kMinimoog, 9, "Tom Sawyer Lead", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.8f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.506f },
        { kMgO3On, 1.0f }, { kMgO3Rng, 0.6f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.85f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.5f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.6f }, { kMgEmph, 0.45f },
        { kMgContAm, 0.35f }, { kMgFAtk, 0.05f }, { kMgFDec, 0.5f }, { kMgFSus, 0.75f }, { kMgLAtk, 0.03f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.25f },
        { -1, 0.0f } } },
    // a 1980-81 New Romantic lead: thin narrow pulse, a little glide, osc 3 as a vibrato LFO on the mod wheel
    { kMinimoog, 10, "New Romantic Lead", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 1.0f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 0.8f }, { kMgO2Freq, 0.508f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.0f }, { kMgO3Wave, 0.0f }, { kMgO3Freq, 0.62f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.55f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.55f }, { kMgEmph, 0.25f },
        { kMgContAm, 0.3f }, { kMgFAtk, 0.05f }, { kMgFDec, 0.5f }, { kMgFSus, 0.7f }, { kMgLAtk, 0.05f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.18f }, { kMgO3Ctrl, 0.0f }, { kMgModMix, 0.0f }, { kMgOscMod, 1.0f },
        { -1, 0.0f } } },
    // ---- Crumar Multiman: the instrument's main voices and combinations (no published
    // settings exist for the records it's on; estimates)
    // cellos and violins on both halves, vibrato, a medium sustain
    { kMultiman, 0, "Full Strings", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.8f }, { kCrCeHi, 0.5f },
        { kCrViLo, 0.5f }, { kCrViHi, 0.8f }, { kCrSust, 0.55f }, { kCrVibDp, 0.2f }, { kCrVibSp, 0.5f },
        { -1, 0.0f } } },
    // violins alone, brighter up top
    { kMultiman, 1, "Violins", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrViLo, 0.6f }, { kCrViHi, 0.85f },
        { kCrSust, 0.5f },
        { -1, 0.0f } } },
    // cellos alone, fuller down low
    { kMultiman, 2, "Cellos", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrCeLo, 0.85f }, { kCrCeHi, 0.6f },
        { kCrSust, 0.5f },
        { -1, 0.0f } } },
    // the brass ladder opening slowly, both halves
    { kMultiman, 3, "Brass Swell", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.6f }, { kCrDec, 0.75f },
        { kCrEmph, 0.3f }, { kCrCont, 0.3f }, { kCrBrLo, 0.95f }, { kCrBrHi, 0.95f }, { kCrMaster, 0.9f },
        { -1, 0.0f } } },
    // short brass hits, every key retriggering the contour
    { kMultiman, 4, "Brass Stabs", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.05f }, { kCrDec, 0.35f },
        { kCrEmph, 0.45f }, { kCrCont, 0.25f }, { kCrBrLo, 0.95f }, { kCrBrHi, 0.95f }, { kCrMaster, 0.9f },
        { -1, 0.0f } } },
    // the piano voice, both halves
    { kMultiman, 5, "Piano", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 1.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrPnLo, 0.75f }, { kCrPnHi, 0.75f },
        { kCrSust, 0.35f },
        { -1, 0.0f } } },
    // the clavichord voice
    { kMultiman, 6, "Clavichord", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 1.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrPnLo, 0.7f }, { kCrPnHi, 0.75f },
        { kCrSust, 0.3f },
        { -1, 0.0f } } },
    // piano with violins over the upper half
    { kMultiman, 7, "Piano and Strings", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 1.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrPnLo, 0.7f }, { kCrPnHi, 0.65f },
        { kCrViLo, 0.0f }, { kCrViHi, 0.6f }, { kCrSust, 0.45f },
        { -1, 0.0f } } },
    // the bass pedals on the lowest 13 keys under full strings
    { kMultiman, 8, "Bass and Strings", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 1.0f }, { kCrVibOn, 0.0f }, { kCrBassV, 0.45f }, { kCrCeLo, 0.6f },
        { kCrCeHi, 0.5f }, { kCrViLo, 0.4f }, { kCrViHi, 0.75f }, { kCrSust, 0.5f }, { kCrMaster, 0.6f },
        { -1, 0.0f } } },
    // strings and a slow brass swell with vibrato and long sustain, in the spirit of Klaus Schulze and Tangerine Dream, who both used the Multiman
    { kMultiman, 9, "Berlin School", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.7f }, { kCrCeHi, 0.5f },
        { kCrViLo, 0.5f }, { kCrViHi, 0.7f }, { kCrAtk, 0.7f }, { kCrDec, 0.8f }, { kCrEmph, 0.35f },
        { kCrCont, 0.28f }, { kCrBrLo, 0.5f }, { kCrBrHi, 0.5f }, { kCrSust, 0.75f }, { kCrVibDp, 0.3f },
        { kCrVibSp, 0.35f }, { kCrMaster, 0.62f },
        { -1, 0.0f } } },
    // more Berlin School sounds (estimates, in the spirit of the records named)
    // Klaus Schulze, Moondawn (1976, Crumar keyboards on the credits): warm cellos and violins under a slow brass swell, long sustain
    { kMultiman, 10, "Moondawn", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.8f }, { kCrCeHi, 0.55f },
        { kCrViLo, 0.45f }, { kCrViHi, 0.7f }, { kCrAtk, 0.75f }, { kCrDec, 0.85f }, { kCrEmph, 0.3f },
        { kCrCont, 0.32f }, { kCrBrLo, 0.55f }, { kCrBrHi, 0.45f }, { kCrSust, 0.8f }, { kCrVibDp, 0.25f },
        { kCrVibSp, 0.3f }, { kCrMaster, 0.6f },
        { -1, 0.0f } } },
    // in the spirit of Schulze's Timewind (1975): a slow, rising string wash that rings for seconds
    { kMultiman, 11, "Timewind", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.75f }, { kCrCeHi, 0.6f },
        { kCrViLo, 0.55f }, { kCrViHi, 0.75f }, { kCrSust, 0.9f }, { kCrVibDp, 0.35f }, { kCrVibSp, 0.25f },
        { kCrMaster, 0.65f },
        { -1, 0.0f } } },
    // in the spirit of Schulze's Mirage (1977): stark, glassy violins, no vibrato, very long sustain
    { kMultiman, 12, "Mirage", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrViLo, 0.5f }, { kCrViHi, 0.8f },
        { kCrSust, 0.95f }, { kCrMaster, 0.7f },
        { -1, 0.0f } } },
    // in the spirit of Schulze's X (1978): a full symphonic section, low cellos leading, vibrato
    { kMultiman, 13, "Symphonic X", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.9f }, { kCrCeHi, 0.7f },
        { kCrViLo, 0.4f }, { kCrViHi, 0.7f }, { kCrSust, 0.7f }, { kCrVibDp, 0.3f }, { kCrVibSp, 0.45f },
        { kCrMaster, 0.62f },
        { -1, 0.0f } } },
    // in the spirit of Tangerine Dream's Phaedra era: a dark, resonant brass drone that opens slowly
    { kMultiman, 14, "Phaedra Drone", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.85f }, { kCrDec, 0.9f },
        { kCrEmph, 0.65f }, { kCrCont, 0.2f }, { kCrBrLo, 0.95f }, { kCrBrHi, 0.8f }, { kCrSust, 0.8f },
        { kCrMaster, 0.85f },
        { -1, 0.0f } } },
    // in the spirit of Tangerine Dream's Rubycon: low brass with emphasis, swelling under cellos
    { kMultiman, 15, "Rubycon Brass", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.6f }, { kCrDec, 0.8f },
        { kCrEmph, 0.55f }, { kCrCont, 0.28f }, { kCrBrLo, 0.95f }, { kCrCeLo, 0.6f }, { kCrCeHi, 0.4f },
        { kCrSust, 0.75f }, { kCrMaster, 0.75f },
        { -1, 0.0f } } },
    // in the spirit of Tangerine Dream's Stratosfear: bright brass over violins, the contour opening on every chord
    { kMultiman, 16, "Stratosfear", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrAtk, 0.35f }, { kCrDec, 0.65f },
        { kCrEmph, 0.35f }, { kCrCont, 0.4f }, { kCrBrLo, 0.6f }, { kCrBrHi, 0.7f }, { kCrViLo, 0.4f },
        { kCrViHi, 0.65f }, { kCrSust, 0.6f }, { kCrVibDp, 0.2f }, { kCrVibSp, 0.5f }, { kCrMaster, 0.65f },
        { -1, 0.0f } } },
    // short, resonant brass blips for repeating Berlin School patterns: every key retriggers the filter
    { kMultiman, 17, "Ostinato Brass", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.0f }, { kCrDec, 0.28f },
        { kCrEmph, 0.6f }, { kCrCont, 0.22f }, { kCrBrLo, 0.95f }, { kCrBrHi, 0.95f }, { kCrSust, 0.3f },
        { kCrMaster, 0.9f },
        { -1, 0.0f } } },
    // cellos and violins split low and high with deep, slow vibrato: a choir-like pad
    { kMultiman, 18, "Cosmic Choir", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrCeLo, 0.85f }, { kCrCeHi, 0.0f },
        { kCrViLo, 0.0f }, { kCrViHi, 0.8f }, { kCrSust, 0.85f }, { kCrVibDp, 0.6f }, { kCrVibSp, 0.2f },
        { kCrMaster, 0.65f },
        { -1, 0.0f } } },
    // the bass pedals as a low drone under a slow brass and string pad
    { kMultiman, 19, "Bass Drone Pad", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 1.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 1.0f }, { kCrVibOn, 0.0f }, { kCrBassV, 0.45f }, { kCrCeLo, 0.5f },
        { kCrCeHi, 0.5f }, { kCrViLo, 0.4f }, { kCrViHi, 0.65f }, { kCrAtk, 0.7f }, { kCrDec, 0.85f },
        { kCrEmph, 0.3f }, { kCrCont, 0.3f }, { kCrBrHi, 0.5f }, { kCrSust, 0.85f }, { kCrMaster, 0.58f },
        { -1, 0.0f } } },
    // ---- 1970s / early-80s Italian horror (Goblin, Fabio Frizzi): settings worked out from
    // the records (estimates)
    // Goblin, Profondo Rosso (1975, Minimoog among the gear): the driving, biting Moog bass line
    { kMinimoog, 11, "Profondo Rosso", {
        { kMgO1Rng, 0.4f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.4f }, { kMgO2Wave, 0.6f }, { kMgO2Freq, 0.506f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.9f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.4f }, { kMgEmph, 0.42f },
        { kMgContAm, 0.5f }, { kMgFAtk, 0.02f }, { kMgFDec, 0.4f }, { kMgFSus, 0.35f }, { kMgLAtk, 0.01f },
        { kMgLDec, 0.45f }, { kMgLSus, 0.85f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 0.0f },
        { -1, 0.0f } } },
    // Goblin, Suspiria (1977, Minimoog and a big Moog): a dark, detuned, resonant growl with a breath of noise
    { kMinimoog, 12, "Suspiria Moog", {
        { kMgO1Rng, 0.2f }, { kMgO1Wave, 0.4f }, { kMgO2Rng, 0.4f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.53f },
        { kMgO3On, 1.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.47f }, { kMgO1Vol, 0.85f },
        { kMgO2Vol, 0.7f }, { kMgO3Vol, 0.6f }, { kMgNzOn, 1.0f }, { kMgCutoff, 0.28f }, { kMgEmph, 0.65f },
        { kMgContAm, 0.45f }, { kMgFAtk, 0.55f }, { kMgFDec, 0.7f }, { kMgFSus, 0.4f }, { kMgLAtk, 0.3f },
        { kMgLDec, 0.6f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.3f }, { kMgNzVol, 0.25f }, { kMgNzCol, 1.0f },
        { -1, 0.0f } } },
    // a late-70s giallo lead: a wailing square with emphasis, slow glide
    { kMinimoog, 13, "Giallo Lead", {
        { kMgO1Rng, 0.6f }, { kMgO1Wave, 0.6f }, { kMgO2Rng, 0.6f }, { kMgO2Wave, 0.4f }, { kMgO2Freq, 0.512f },
        { kMgO3On, 0.0f }, { kMgO3Rng, 0.4f }, { kMgO3Wave, 0.6f }, { kMgO3Freq, 0.494f }, { kMgO1Vol, 0.8f },
        { kMgO2Vol, 0.55f }, { kMgO3Vol, 0.0f }, { kMgNzOn, 0.0f }, { kMgCutoff, 0.5f }, { kMgEmph, 0.5f },
        { kMgContAm, 0.35f }, { kMgFAtk, 0.15f }, { kMgFDec, 0.55f }, { kMgFSus, 0.65f }, { kMgLAtk, 0.1f },
        { kMgLDec, 0.5f }, { kMgLSus, 1.0f }, { kMgDecay, 1.0f }, { kMgKbd1, 1.0f }, { kMgKbd2, 1.0f },
        { kMgGlideOn, 1.0f }, { kMgGlide, 0.4f },
        { -1, 0.0f } } },
    // Fabio Frizzi, Zombi 2 (1979, polysynths of the Prophet / Jupiter / CS-80 kind): the heavy, dark synth theme
    { kOBX, 10, "Zombi 2", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.0f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.6f },
        { kOxO2Frq, 0.25f }, { kOxCut, 0.33f }, { kOxRes, 0.35f }, { kOxEnvAm, 0.45f }, { kOxFA, 0.25f },
        { kOxFD, 0.6f }, { kOxFS, 0.45f }, { kOxFR, 0.5f }, { kOxLA, 0.1f }, { kOxLS, 0.9f },
        { kOxLR, 0.5f },
        { -1, 0.0f } } },
    // Fabio Frizzi, The Beyond (1981): a ghostly, vocal-like pad that slowly breathes
    { kOBX, 11, "The Beyond", {
        { kOxO1Saw, 0.0f }, { kOxO1Pls, 1.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.12f },
        { kOxModO1, 0.0f }, { kOxModO2, 0.0f }, { kOxModFl, 1.0f }, { kOxModPw, 0.0f }, { kOxPW, 0.25f },
        { kOxO2Det, 0.62f }, { kOxCut, 0.4f }, { kOxRes, 0.45f }, { kOxEnvAm, 0.2f }, { kOxLfoRt, 0.2f },
        { kOxFA, 0.7f }, { kOxFD, 0.7f }, { kOxFS, 0.7f }, { kOxFR, 0.7f }, { kOxLA, 0.65f },
        { kOxLS, 1.0f }, { kOxLR, 0.7f }, { kOxVol, 0.45f },
        { -1, 0.0f } } },
    // Goblin / Simonetti, Tenebre (1982; Simonetti is credited with an OB-X): the bright, driving arpeggio voice
    { kOBX, 12, "Tenebre", {
        { kOxO1Saw, 1.0f }, { kOxO1Pls, 0.0f }, { kOxO2Saw, 1.0f }, { kOxO2Pls, 0.0f }, { kOxModDp, 0.0f },
        { kOxModO1, 1.0f }, { kOxModO2, 1.0f }, { kOxModFl, 0.0f }, { kOxModPw, 0.0f }, { kOxO2Det, 0.57f },
        { kOxO2Frq, 0.75f }, { kOxCut, 0.5f }, { kOxRes, 0.3f }, { kOxEnvAm, 0.5f }, { kOxFA, 0.0f },
        { kOxFD, 0.42f }, { kOxFS, 0.3f }, { kOxFR, 0.35f }, { kOxLA, 0.0f }, { kOxLD, 0.5f },
        { kOxLS, 0.6f }, { kOxLR, 0.35f },
        { -1, 0.0f } } },
    // Goblin, Profondo Rosso: a full, gothic organ, chorus vibrato and a slow Leslie
    { kHammond, 0, "Profondo Organ", {
        { kB3Db1, 1.000f }, { kB3Db2, 0.000f }, { kB3Db3, 1.000f }, { kB3Db4, 1.000f }, { kB3Db5, 0.500f },
        { kB3Db6, 0.750f }, { kB3Db7, 0.000f }, { kB3Db8, 0.500f }, { kB3Db9, 0.750f }, { kB3Perc, 0.0f },
        { kB3VibOn, 1.0f }, { kB3VibSel, 1.0f }, { kLsOn, 1.0f }, { kLsSpeed, 0.5f }, { kB3Drive, 0.3f },
        { kB3Vol, 0.75f },
        { -1, 0.0f } } },
    // Goblin, Suspiria (Hammond B-3 among the gear): a dark, low organ, deep vibrato, Leslie stopped and driven
    { kHammond, 1, "Suspiria Organ", {
        { kB3Db1, 1.000f }, { kB3Db2, 0.750f }, { kB3Db3, 0.500f }, { kB3Db4, 0.000f }, { kB3Db5, 0.000f },
        { kB3Db6, 0.000f }, { kB3Db7, 0.000f }, { kB3Db8, 0.000f }, { kB3Db9, 0.000f }, { kB3Perc, 0.0f },
        { kB3VibOn, 1.0f }, { kB3VibSel, 0.8f }, { kLsOn, 1.0f }, { kLsSpeed, 0.0f }, { kB3Drive, 0.55f },
        { kB3Vol, 0.75f },
        { -1, 0.0f } } },
    // Goblin, Suspiria (Hohner Clavinet among the gear): a dark, muted, plucked clav
    { kClavinet, 2, "Suspiria Clav", {
        { kClBrill, 0.0f }, { kClTreb, 0.0f }, { kClMed, 1.0f }, { kClSoft, 1.0f }, { kClCD, 1.0f },
        { kClAB, 0.0f }, { kClMute, 0.4f }, { kClVol, 0.85f },
        { -1, 0.0f } } },
    // eerie, thin violins with slow, wide vibrato, for 70s Italian thrillers
    { kMultiman, 20, "Giallo Strings", {
        { kCrBrOnLo, 0.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 0.0f },
        { kCrViolin, 1.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 1.0f }, { kCrViLo, 0.55f }, { kCrViHi, 0.8f },
        { kCrSust, 0.7f }, { kCrVibDp, 0.55f }, { kCrVibSp, 0.25f }, { kCrMaster, 0.65f },
        { -1, 0.0f } } },
    // an ominous low brass swell with emphasis under cellos, for Lucio Fulci-style horror
    { kMultiman, 21, "Fulci Brass", {
        { kCrBrOnLo, 1.0f }, { kCrBrOnHi, 0.0f }, { kCrPiano, 0.0f }, { kCrClav, 0.0f }, { kCrCello, 1.0f },
        { kCrViolin, 0.0f }, { kCrBassOn, 0.0f }, { kCrVibOn, 0.0f }, { kCrAtk, 0.55f }, { kCrDec, 0.8f },
        { kCrEmph, 0.6f }, { kCrCont, 0.22f }, { kCrBrLo, 0.95f }, { kCrCeLo, 0.7f }, { kCrCeHi, 0.3f },
        { kCrSust, 0.7f }, { kCrMaster, 0.75f },
        { -1, 0.0f } } },
    // Goblin used an Elka organ on Profondo Rosso and Suspiria: a reedy, eerie combo organ with the ensemble
    { kSK30, 11, "Elka Organ", {
        { kSkOrgSel, 1.0f }, { kSkO16, 0.0f }, { kSkO8, 1.0f }, { kSkO513, 0.0f }, { kSkO4, 0.9f },
        { kSkO223, 0.6f }, { kSkO2, 0.8f }, { kSkO135, 0.4f }, { kSkO113, 0.0f }, { kSkO1, 0.5f },
        { kSkOrgBril, 0.75f }, { kSkOrgEns, 1.0f }, { kSkOrgV, 0.65f }, { kSkPolyV, 0.0f }, { kSkStrV, 0.0f },
        { kSkSoloV, 0.0f },
        { -1, 0.0f } } },
    // ---- Elka Rhapsody 610: Billy Currie (Ultravox), Vangelis, Klaus Schulze; settings worked
    // out from the records (estimates)
    // Ultravox (Billy Currie), "Vienna" era: strings and violoncello together, a long, sweeping release
    { kRhapsody, 0, "Vienna Strings", {
        { kRhCello, 1.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.0f },
        { kRhDecCello, 0.8f }, { kRhDecStr, 0.8f }, { kRhVolStrLo, 0.7f }, { kRhVolStrHi, 0.75f }, { kRhMaster, 0.68f },
        { -1, 0.0f } } },
    // Ultravox!, "Hiroshima Mon Amour" (1977): a cold, plain string line through the chorus
    { kRhapsody, 1, "Hiroshima Mon Amour", {
        { kRhCello, 0.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.0f },
        { kRhDecStr, 0.72f }, { kRhVolStrHi, 0.75f }, { kRhMaster, 0.7f },
        { -1, 0.0f } } },
    // Ultravox, "Slow Motion" (1978): split at 2 octaves, piano below, strings above
    { kRhapsody, 2, "Slow Motion", {
        { kRhCello, 0.0f }, { kRhStrings, 1.0f }, { kRhPiano, 1.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.5f },
        { kRhVolStrLo, 0.0f }, { kRhVolStrHi, 0.8f }, { kRhVolPnLo, 0.8f }, { kRhVolPnHi, 0.0f }, { kRhDecStr, 0.65f },
        { kRhDecPno, 0.55f }, { kRhMaster, 0.7f },
        { -1, 0.0f } } },
    // Ultravox, Rage in Eden (1981): dark violoncello under quieter strings, long release
    { kRhapsody, 3, "Rage in Eden", {
        { kRhCello, 1.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.0f },
        { kRhVolStrLo, 0.8f }, { kRhVolStrHi, 0.6f }, { kRhDecCello, 0.88f }, { kRhDecStr, 0.85f }, { kRhMaster, 0.66f },
        { -1, 0.0f } } },
    // Vangelis (a Rhapsody 610 player), Heaven and Hell (1975): the full section, ringing on for seconds
    { kRhapsody, 4, "Heaven and Hell", {
        { kRhCello, 1.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.0f },
        { kRhVolStrLo, 0.8f }, { kRhVolStrHi, 0.8f }, { kRhDecCello, 0.92f }, { kRhDecStr, 0.92f }, { kRhMaster, 0.62f },
        { -1, 0.0f } } },
    // Vangelis, Albedo 0.39 (1976): clavichord with a long decay over soft strings
    { kRhapsody, 5, "Albedo", {
        { kRhCello, 0.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 1.0f }, { kRhSplit, 0.0f },
        { kRhVolStrHi, 0.5f }, { kRhVolStrLo, 0.5f }, { kRhVolPnLo, 0.8f }, { kRhVolPnHi, 0.8f }, { kRhDecStr, 0.8f },
        { kRhDecPno, 0.7f }, { kRhMaster, 0.7f },
        { -1, 0.0f } } },
    // Vangelis, Spiral (1977): short, bright clavichord for running patterns, a little string behind
    { kRhapsody, 6, "Spiral", {
        { kRhCello, 0.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 1.0f }, { kRhSplit, 0.0f },
        { kRhVolStrHi, 0.4f }, { kRhVolStrLo, 0.4f }, { kRhVolPnLo, 0.85f }, { kRhVolPnHi, 0.85f }, { kRhDecStr, 0.6f },
        { kRhDecPno, 0.3f }, { kRhMaster, 0.72f },
        { -1, 0.0f } } },
    // Klaus Schulze, Body Love (1977): strings and violoncello with a very long release, for slow chords
    { kRhapsody, 7, "Body Love", {
        { kRhCello, 1.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.0f },
        { kRhVolStrLo, 0.75f }, { kRhVolStrHi, 0.7f }, { kRhDecCello, 0.95f }, { kRhDecStr, 0.95f }, { kRhMaster, 0.62f },
        { -1, 0.0f } } },
    // Klaus Schulze, Mirage (1977): thin, icy strings high up, very long release
    { kRhapsody, 8, "Mirage Ice", {
        { kRhCello, 0.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 0.5f },
        { kRhVolStrLo, 0.35f }, { kRhVolStrHi, 0.8f }, { kRhDecStr, 0.9f }, { kRhMaster, 0.7f },
        { -1, 0.0f } } },
    // Klaus Schulze, Dune (1979): split at 3 octaves, heavy violoncello and strings below, lighter strings above
    { kRhapsody, 9, "Dune", {
        { kRhCello, 1.0f }, { kRhStrings, 1.0f }, { kRhPiano, 0.0f }, { kRhClav, 0.0f }, { kRhSplit, 1.0f },
        { kRhVolStrLo, 0.85f }, { kRhVolStrHi, 0.5f }, { kRhDecCello, 0.85f }, { kRhDecStr, 0.85f }, { kRhMaster, 0.64f },
        { -1, 0.0f } } },
};
const int kNumFactoryPresets = (int) (sizeof (kFactoryPresets) / sizeof (kFactoryPresets[0]));

} // namespace tbsyn

#endif
