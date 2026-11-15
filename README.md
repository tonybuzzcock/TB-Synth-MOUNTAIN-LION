# TB Synth (Mountain Lion VST)

Seven classic keyboards in one VST 2.4 instrument, for **Mac OS X 10.8.5** (64-bit Intel), played in
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
TB electronic logo, SAVE and 8 named preset keys (each instrument has its own bank of 8), MASTER,
and the tan readout. A LEVEL LED at top right lights amber while it plays, red near full scale.

- Knobs and sliders: drag up / down (Shift for fine), double-click for the default, scroll wheel.
- Rotary switches: click a marked position to go there, or click the knob to step round; drag works too.
- Drawbars: drag down to pull out, up to push in, a step at a time.
- Right-click (or Control-click) any control to see its exact value without moving it.
- Presets: SAVE, then a key; type a name and press Return. Click the readout to rename.

## Factory presets

| Instrument | Key | Name | For |
|---|---|---|---|
| Minimoog Model D | 1 | Carlos | Wendy Carlos, Switched-On Bach (a Moog modular): a harpsichord voice, 8' + 4' narrow pulses, plucked contours |
| Oberheim OB-X | 1 | Queen | Flash Gordon: big detuned brass, the filter opening on each stab |
| Oberheim OB-X | 2 | Japan | Tin Drum (Richard Barbieri): a dark pulse-wave pad, slow attack, slowly moving pulse width |
| Oberheim OB-X | 3 | Foxx | John Foxx, The Garden: a lush shimmering pad, saw and pulse, gentle vibrato |
| Wurlitzer 200A | 1 | Supertramp | "Bloody Well Right" intro: dry, no vibrato, amp pushed to grit |
| Hohner Clavinet D6 | 1 | Superstition | Stevie Wonder: BRILLIANT + TREBLE, both pickups in phase (D-A) |
| Hohner Clavinet D6 | 2 | Quatro | Suzi Quatro's keyboard player: TREBLE + MEDIUM, bridge pickup, a little mute, preamp pushed |
| Solina String Ensemble | 1 | Moroder | "I Feel Love": the record's pad is Moog modular; this is the Solina's take on it |

No published settings exist for these parts (the clavinet.com FAQ says of "Superstition" that it was a
Clavinet C, overdubbed); the presets are set by ear from the records. The factory presets always
hold the first keys of their bank: they can't be saved over or renamed (SAVE stays armed for another
key). Any of your presets found on one of those keys, or saved with the first release (8 shared
keys), moves to the next free key of its instrument's bank.

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
