# kaminim00g

A Minimoog-inspired monophonic synthesizer plugin built with JUCE. Outputs VST3 and AU formats, compatible with Ableton Live and Logic Pro.

![kam00g](https://img.shields.io/badge/format-VST3%20%7C%20AU-orange) ![platform](https://img.shields.io/badge/platform-macOS-lightgrey)

---

## Features

### Oscillator Bank
Three VCOs each with:
- **7 waveforms** — Sine, Triangle, Reverse Saw, Sawtooth, Square, Wide Pulse, Narrow Pulse
- **6 range settings** — Lo, 32', 16', 8', 4', 2'
- **Fine tune** — ±7 semitones per oscillator
- **PolyBLEP anti-aliasing** on all discontinuous waveforms

### Mixer
Individual level controls for OSC 1, OSC 2, OSC 3, and white noise.

### Filter
Huovilainen Moog ladder filter — 24dB/oct resonant lowpass.
- Cutoff, Resonance (self-oscillates near maximum)
- Filter envelope amount
- Keyboard tracking

### Envelopes
Separate ADSR envelopes for filter contour and loudness contour.

### Modulation
- **LFO** — Sine, Triangle, Saw, Square with rate 0.1–20Hz
- **Mod Mix** — blend LFO and noise as modulation source
- **Routing** — independently route to filter cutoff and/or oscillator pitch
- **MIDI mod wheel (CC1)** scales modulation depth live

### Controllers
- **Master Tune** — global pitch shift ±12 semitones
- **Glide** — smooth portamento between notes (0–2 seconds)
- **Bend Range** — MIDI pitch wheel range 1–12 semitones

### FX
- **Drive** — soft tanh saturation for warmth and grit
- **Stereo Chorus** — quadrature LFO delay lines with Rate, Depth, and Mix controls

### Presets
6 factory presets: Init, Fat Bass, Bright Lead, Brass, Strings, Filter Sweep.

---

## Building

### Requirements
- macOS with Xcode 14+
- CMake 3.22+
- [JUCE](https://github.com/juce-framework/JUCE) cloned to `~/JUCE`

### Steps

```bash
git clone https://github.com/manoukpaglayan/kaminim00g
cd kaminim00g
cmake -B build -G Xcode
cmake --build build --config Release
```

Built plugins appear in `build/kam00g_artefacts/Release/`.

### Install

```bash
cp -R build/kam00g_artefacts/Release/AU/kam00g.component ~/Library/Audio/Plug-Ins/Components/
cp -R build/kam00g_artefacts/Release/VST3/kam00g.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

---

## Signal Flow

```
[OSC 1] ─┐
[OSC 2] ─┼─→ [MIXER] ─→ [DRIVE] ─→ [LADDER FILTER] ─→ [VCA] ─→ [CHORUS] ─→ out
[OSC 3] ─┤                               ↑                ↑
[NOISE] ─┘                         [FILTER ENV]      [AMP ENV]
                                         ↑
                              [LFO / NOISE × MOD MIX]
```

---

## License

MIT
