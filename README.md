# TB Synth (Mountain Lion VST)

Nine classic keyboards in one VST 2.4 instrument, for **Mac OS X 10.8.5** (64-bit Intel), played in
Pro Tools 11 through Blue Cat PatchWork. Pick the instrument from the **INSTRUMENT** menu at the top of the
panel (click it for the list, or step with the arrows):

| Instrument | What's modelled |
|---|---|
| **Minimoog Model D** | 3 oscillators (LO / 32' - 2', six waveforms), mixer with noise (white / pink) and its overdrive, 24 dB ladder filter, filter and loudness contours, glide, modulation mix, low-note priority, single triggering |
| **Oberheim OB-X** | 8 voices, 2 oscillators each (saw / pulse, sync, X-MOD), 2-pole filter, two ADSRs, modulation LFO (sine / square / S&H) to oscillators, filter, pulse width; portamento |
| **Solina String Ensemble** | divide-down tone generator for all 49 keys, VIOLIN / VIOLA / TRUMPET / HORN / CELLO / CONTRABASS, CRESCENDO, SUSTAIN, the built-in ensemble chorus |
| **Wurlitzer 200A** | 64 struck reeds, electrostatic pickup (the bark when played hard), preamp, VIBRATO (the 200A's tremolo), amp and speakers, sustain pedal |
| **Hammond B-3 + Leslie 122** | 91 tonewheels on the real gear ratios, 9 drawbars, key click, percussion (2nd / 3rd, soft / normal, fast / slow, single trigger), scanner vibrato / chorus V1-C3, preamp drive, expression; Leslie 122 horn and drum with STOP / SLOW / FAST |
| **Hohner Cembalet** | 61 plucked reeds with capacitive pickup, VOLUME |
| **Hohner Clavinet D6** | 60 struck strings, two pickups (C/D, A/B), BRILLIANT / TREBLE / MEDIUM / SOFT, mute slider |
| **Crumar Multiman (1975)** | 61 keys split LOWER / UPPER; BRASS (transistor ladder with ATTACK, DECAY, EMPHASIS, CONTOUR, retriggered by every key), PIANO / CLAVICHORD, CELLO / VIOLIN through the ensemble, SUSTAIN LENGTH, VIBRATO, MASTER, BASS PEDALS on the lowest 13 keys |
| **ARP Omni (1975)** | 49 keys; STRINGS: VIOLINS 4' and VIOLAS 8' (polyphonic), CELLOS 8' and BASS 16' (monophonic, low-note priority, bottom 20 keys), ATTACK, RELEASE, the string phasers; SYNTHESIZER: 4' / 8' polyphonic and BASS 8' / 16', SAW or DYNAMIC PULSE, ENHANCEMENT, one 24 dB ladder VCF (FREQUENCY, RESONANCE, ADSR, LFO) and ADSR retriggered by every key, LFO 0.1 - 20 Hz, CHORUS PHASER; STRING / SYNTH MIX, BASS VOLUME, MASTER, sustain pedal |

**Mono**, like the originals: the same signal on both outputs, so it works on a mono or stereo
instrument track. Every control is a host parameter, so a session reopens exactly as it was left.

MIDI: notes (velocity on the Wurlitzer, Cembalet and Clavinet, as on the real keyboards), pitch bend
(Minimoog: a fifth either way, as its wheel; OB-X: 2 semitones), modulation wheel (Minimoog and OB-X:
modulation amount; B-3: Leslie to FAST while it's up), sustain pedal (Wurlitzer), all notes off.

## The panel

In the colours of the TASCAM M-50 and the TB M-50 strips: warm grey face, black lettering, charcoal
knurled knobs (red caps for levels, orange skirts for frequencies), switches that light amber in their
engaged position, the red lip, the lacquered mahogany frame. Each instrument has its own page laid out
like the original's front panel, with plain lettering and no manufacturer artwork. Along the bottom: the
TB electronic logo, DEFAULT, SAVE and the preset menu (each instrument has its own, up to 99 presets),
MASTER, and the tan readout. A LEVEL LED at top right lights amber while it plays, red near full scale.

- Knobs and sliders: drag up / down (Shift for fine), double-click for the default, scroll wheel.
- Rotary switches: click a marked position to go there, or click the knob to step round; drag works too.
- Drawbars: drag down to pull out, up to push in, a step at a time.
- Right-click (or Control-click) any control to see its exact value without moving it.
- Presets: click the preset menu to choose one (they're numbered in the list; the box shows the name,
  dimmed once you've moved a control). To keep a sound, press **SAVE**: the menu becomes a name field;
  type a name and press **SAVE** again (or Return) and it's added to the end of this instrument's menu.
  Esc or a click elsewhere cancels. It works the same on every instrument.
- **DEFAULT** puts every control of the current instrument back to its default (other instruments and
  MASTER are left alone), to start a new sound from scratch.

## Factory presets

| Instrument | No. | Name | For |
|---|---|---|---|
| Minimoog Model D | 1 | Carlos | Wendy Carlos, Switched-On Bach (a Moog modular): a harpsichord voice, 8' + 4' narrow pulses, plucked contours |
| Oberheim OB-X | 1 | Queen | Flash Gordon: big detuned brass, the filter opening on each stab |
| Oberheim OB-X | 2 | Japan | Tin Drum (Richard Barbieri): a dark pulse-wave pad, slow attack, slowly moving pulse width |
| Oberheim OB-X | 3 | Foxx | John Foxx, The Garden: a lush shimmering pad, saw and pulse, gentle vibrato |
| Oberheim OB-X | 4 | Tom Sawyer | Rush, "Tom Sawyer" (Moving Pictures, Geddy Lee's OB-X): the growl, two detuned saws through a resonant filter swept slowly by the LFO |
| Oberheim OB-X | 5 | Subdivisions | Rush, "Subdivisions" (Signals): lush, slowly opening chords with a little vibrato |
| Oberheim OB-X | 6 | 1999 | Prince, "1999" (first recorded on an OB-X): wide detuned saws, the LFO gently moving the oscillators, punchy |
| Oberheim OB-X | 7 | When You Were Mine | Prince, "When You Were Mine" (Dirty Mind, the riff on the OB-X): thin, organ-like pulse, an octave apart |
| Oberheim OB-X | 8 | Sweet Dreams | Eurythmics, "Sweet Dreams (Are Made of This)": the soft, reedy pad, pulse and saw |
| Oberheim OB-X | 9 | 99 Luftballons | Nena, "99 Luftballons": bright synth stabs that fall back quickly |
| Oberheim OB-X | 10 | Jarre Brass | Jean-Michel Jarre, who used the OB-X for its brass: a slow-blooming brass section |
| Wurlitzer 200A | 1 | Supertramp | "Bloody Well Right" intro: dry, no vibrato, amp pushed to grit |
| Hohner Clavinet D6 | 1 | Superstition | Stevie Wonder: BRILLIANT + TREBLE, both pickups in phase (D-A) |
| Hohner Clavinet D6 | 2 | Quatro | Suzi Quatro's keyboard player: TREBLE + MEDIUM, bridge pickup, a little mute, preamp pushed |
| ARP Omni | 1 | Joy Division | "Atmosphere" / Closer: violins, violas and cellos through the phasers, slow swell, long release |
| ARP Omni | 2-10 | Brass, Brass Strings Bass, Shimmer Sweep, Phased Sweep, Sweep Enh Strings, Poly Wah, Harpsichord Bass, Lower Strings, Lucy in the Sky | the nine patches in ARP's Omni-2 owner's manual (1978), set from their names and the Omni's controls (the manual's slider charts weren't readable here) |
| ARP Omni | 11 | Moving In Stereo | The Cars: Greg Hawkes said every keyboard on the track is the Omni; a dark hollow pad with a slow filter swell |
| Solina String Ensemble | 1 | Moroder | "I Feel Love": the record's pad is Moog modular; this is the Solina's take on it |
| Solina String Ensemble | 2 | Shine On | Pink Floyd, Wish You Were Here (Rick Wright): full, slow-swelling strings that ring on |
| Solina String Ensemble | 3 | Dream Weaver | Gary Wright, "Dream Weaver": soft upper strings, gentle swell |
| Solina String Ensemble | 4 | Video Killed | The Buggles, "Video Killed the Radio Star": bright, quick pop strings |
| Solina String Ensemble | 5 | Elton | Elton John, "Someone Saved My Life Tonight": violins, violas and cellos |
| Solina String Ensemble | 6 | Bee Gees | Bee Gees, "Nights on Broadway": the whole section, cellos and basses in |
| Solina String Ensemble | 7 | Chocolate City | Parliament (Bernie Worrell): the Solina played as a solo voice, brassy and quick |
| Solina String Ensemble | 8 | I'm In You | Peter Frampton, "I'm in You" (Stevie Wonder's string line): violins and violas, singing |
| Solina String Ensemble | 9 | Dogs | Pink Floyd, Animals, the middle of "Dogs": dark, low, very slow swell |

No published settings exist for these parts (the clavinet.com FAQ says of "Superstition" that it was a
Clavinet C, overdubbed); the presets are set by ear from the records. The factory presets always come
first in their instrument's menu and can't be saved over or renamed. Presets saved with earlier builds
(8 shared keys, or a bank of 8 keys per instrument) keep their places in the menus.

## Sources

| Instrument | Documents used |
|---|---|
| Minimoog Model D | Moog *Model D* owner's manual (specifications pp. 80-81: filter 10 Hz - 20 kHz at 24 dB/oct, contour amount 0-4 octaves, attack 1 ms - 10 s, decay 4 ms - 35 s, glide 1 ms - 10 s, wheel a fifth), R. J. Folkman's *Field Service Manual* (ladder filter; regeneration starts between EMPHASIS 7 and 8; three oscillators, saw with derived triangle and rectangles) |
| Hammond B-3 | B-3 / C-3 service manual (AO-28 preamp, percussion), Hammond generator gear ratios (85/104 ... 108/70 at 20 rev/s) |
| Wurlitzer 200A | Wurlitzer Series 200 / 200A service manual (64 notes A-C, input amplifier, modulator, output amplifier) |
| Hohner Clavinet D6 | D6 owner's manual (60 keys, contra F to e'''), D6 schematic and tone-switch notes (Chicago Electric Piano Co.: BRILLIANT / TREBLE high-pass, MEDIUM / SOFT low-pass, stacking; C-A / C-B / D-A / D-B pickup combinations; no tone switch = silence) |
| Hohner Cembalet | Hohner product descriptions (61 keys C2 - C7, plectrum and steel reed, capacitive pickup beside the reed) |
| Solina String Ensemble | Eminent / ARP descriptions (divide-down, 49 keys, six voices, crescendo and sustain, ensemble chorus) |
| Oberheim OB-X | OB-X owner's and service manual descriptions (8 voices, 2 oscillators, 2-pole filter, two envelopes) |
| Crumar Multiman | Crumar *Multiman* schematic diagram, DWG 1-12, 7/4/75 (`Docs/Crumar_Multiman_1975_schematic.pdf`): block diagram, tone generator and dividers, gates and key contacts (split buses P1/P2, 8A/8B, D1/D2), brass ladder TR1-TR10 with 47 nF stages and its A/D envelope, piano / clavichord, strings, sustain and vibrato, delay lines, bass pedals, panel legends |
| ARP Omni | ARP *Omni Model 2480 Service Manual* (Feb. 1977, doc. 90020000): product description and control functions, simplified block diagram (master top-octave generator, 49 divided lines, string 4' / 8' and bass voicing, 4075 24 dB VCF on the summed 4' / 8' polyphonic and 8' / 16' bass pitches, ADSR, LFO, phaser), trim procedures; patch names from the *Omni-2 Owner's Manual* (1978) |

The PDFs themselves couldn't be downloaded into the build environment, so the values above were read from
them online; no component-level schematic was transcribed. These are behavioural models built from
the documented behaviour and specifications, to be tuned by ear (as with the other TB plugins).

## Estimates (named constants, to tune by ear)

- **Minimoog**: LO range 9 octaves below 8'; osc 2 / 3 FREQUENCY +/- 7 semitones; TUNE +/- 2.5 semitones;
  release with DECAY off 8 ms; full-wheel modulation 12 semitones (oscillators), 2 octaves (filter);
  keyboard tracking reference C3; wide / narrow rectangles 30 % / 12 %; output amp starts to overdrive
  above VOLUME 6 (the manual's advice).
- **OB-X**: LFO 0.1 - 25 Hz; full DEPTH 12 semitones / 3 octaves / 40 % pulse width; X-MOD 18 semitones;
  RESONANCE Q 0.55 - 20; envelope times 1 ms - 10 s attack, 2 ms - 15 s decay / release; filter
  MODULATION 0 - 7 octaves; pitch lever 2 semitones; per-voice tuning tolerance +/- 4 cents.
- **Solina**: voice formant filters; ensemble 6 ms +/- 2.2 ms (0.62 Hz) +/- 0.28 ms (6.1 Hz), three
  taps 120 degrees apart; CRESCENDO 8 ms - 4 s; SUSTAIN 50 ms - 6 s; VIOLA voiced as a darker 8' with some
  16'; all voices play across the whole keyboard.
- **Wurlitzer 200A**: reed decays (4.5 s at A1 halving every 16 semitones), second mode at 6.27 x,
  pickup corner 300 Hz, VIBRATO rate 5.6 Hz and depth, damper times, speaker voicing.
- **Hammond**: drawbar steps 3 dB; percussion decay 0.5 s fast / 2.4 s slow (to -60 dB), soft -6 dB,
  normal takes 3 dB off the drawbars; vibrato depths V1 / V2 / V3 0.22 / 0.42 / 0.70 ms at 6.87 Hz;
  key contacts close over 3.5 ms. **Leslie 122**: horn 0.83 / 6.7 rev/s, drum 0.67 / 5.8 rev/s; horn
  spins up in about 0.75 s, the drum in about 4.5 s; 800 Hz crossover; one mic each, summed.
- **Cembalet**: reed decays (2.4 s at C2), third mode at 17.55 x, pickup curve.
- **Clavinet D6**: pickup positions (neck 17 %, bridge 7 % of the string), tone-switch filters
  (BRILLIANT 2 kHz and TREBLE 650 Hz high-pass, MEDIUM 2.2 kHz and SOFT 600 Hz low-pass), string
  decays, mute.

- **Crumar Multiman**: brass contour 4 ms - 3 s attack, 20 ms - 6 s decay, 4-octave sweep, CONTOUR 60 Hz -
  12 kHz; string attack 60 ms; SUSTAIN LENGTH 80 ms - 5 s; piano decay; vibrato 2.5 - 9 Hz, up to 0.6
  semitone; cello / violin / piano / clavichord filter corners; ensemble timings; split at C4. The panel
  is laid out in TB Synth's style rather than copying the Crumar's, with every control the schematic
  shows. The real bass pedals were a separate pedal board: here the lowest 13 keys (F1 - F2) play them,
  one note at a time, an octave down.

- **ARP Omni**: string attack 5 ms - 3 s, release 50 ms - 6 s; synthesizer key gate 3 ms; ADSR attack 2 ms -
  5 s, decay and release 10 ms - 8 s; VCF 20 Hz - 16 kHz, ADSR sweep 6 octaves, LFO sweep 2 octaves;
  DYNAMIC PULSE width following the ADSR; bass keying decay 2.5 s, cut 30 ms; phaser delays 0.12 - 0.72 ms
  at 0.63 / 0.81 / 1.07 Hz, CHORUS PHASER at 0.45 x those rates and deeper; voicing filters. The synth
  bass goes through the VCF with the polyphonic pitches, as the service manual describes. The LFO range and the 4075 ladder
  are from the service manual.

## Install (Mac Pro, OS X 10.8.5)

1. GitHub -> **Actions** -> the latest successful run -> **Artifacts** -> `TB-Synth-mountain-lion-vst`.
2. Unzip twice and copy **TB Synth.vst** to `/Library/Audio/Plug-Ins/VST/`.
3. In Pro Tools 11, put Blue Cat PatchWork on an instrument track (the synth version, so it passes MIDI),
   rescan, and load **TB Synth**.

GitHub has no 10.8 machines: the build is verified to declare 10.8.5 and to link only 10.8-era system
libraries, but the first load on the Mac Pro is the real-world test.

## Build and test

- `make -C Tests run vst-run` (Linux or Mac): DSP tests (tuning, behaviour, stability, 96 / 192 kHz),
  presets, and the plugin in a mini VST host; demo WAVs in `Tests/renders`.
- `make -C Tests asan`: the same under AddressSanitizer and UndefinedBehaviorSanitizer.
- `scripts/build_vst_mac.sh` (macOS): builds and verifies the bundle. GitHub Actions does this on every
  push, runs the panel test (every control on every page) and publishes screenshots to `gui-preview`.

Non-commercial, for personal use. Instrument names are used only to say what is modelled.
