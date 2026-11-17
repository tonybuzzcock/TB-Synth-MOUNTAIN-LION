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
};
const int kNumFactoryPresets = (int) (sizeof (kFactoryPresets) / sizeof (kFactoryPresets[0]));

} // namespace tbsyn

#endif
