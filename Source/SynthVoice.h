#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"
#include "LadderFilter.h"
#include "Envelope.h"

class SynthVoice
{
public:
    void setSampleRate(double sr)
    {
        sampleRate = sr;
        osc1.setSampleRate(sr);
        osc2.setSampleRate(sr);
        osc3.setSampleRate(sr);
        lfo.setSampleRate(sr);
        filterEnv.setSampleRate(sr);
        ampEnv.setSampleRate(sr);
        filter.setSampleRate(sr);
    }

    void noteOn(int midiNote, float velocity)
    {
        currentNote     = midiNote;
        currentVelocity = velocity;
        targetHz = noteToHz(midiNote);
        // If glide is off or first note, snap immediately
        if (currentHz <= 0.0 || glideTime < 0.001f)
            currentHz = targetHz;
        updateOscFrequencies(currentHz);
        filterEnv.noteOn();
        ampEnv.noteOn();
        active = true;
    }

    void noteOff()
    {
        filterEnv.noteOff();
        ampEnv.noteOff();
    }

    bool isActive() const { return active && ampEnv.isActive(); }

    float process()
    {
        // --- Glide: smoothly interpolate currentHz toward targetHz ---
        if (glideTime > 0.001f && std::abs(currentHz - targetHz) > 0.01)
        {
            float coeff = std::exp(-1.0f / ((float)sampleRate * glideTime));
            currentHz = currentHz * coeff + targetHz * (1.0 - coeff);
            updateOscFrequencies(currentHz);
        }

        // --- Noise ---
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        // --- LFO + Noise crossfade as mod source ---
        float lfoSample = lfo.process();
        float modSource = lfoSample * (1.0f - modMix) + noise * modMix;

        // Smooth mod source for filter — prevents audio-rate noise from trashing cutoff
        smoothedMod = smoothedMod * 0.995f + modSource * 0.005f;

        // Mod wheel scales effective depth (wheel defaults to 1.0 = full depth)
        float depth = modAmount * modWheelValue;
        float pitchMod  = modToOsc    ? depth * modSource  * 12.0f : 0.0f;
        float filterMod = modToFilter ? depth * smoothedMod         : 0.0f;

        // --- OSC mix (OSC3 always contributes to audio) ---
        float osc3Sample = osc3.process();
        float mixed = osc1.process(pitchMod) * osc1Level
                    + osc2.process(pitchMod) * osc2Level
                    + osc3Sample             * osc3Level
                    + noise                  * noiseLevel;

        // --- Filter ---
        float filterEnvValue = filterEnv.process();
        float cutoff = baseCutoff
                     + filterEnvAmount * filterEnvValue * 8000.0f
                     + keyTrackAmount  * (currentNote - 60) * 30.0f
                     + filterMod       * 4000.0f;

        cutoff = juce::jlimit(20.0f, 18000.0f, cutoff);
        filter.setCutoff(cutoff);

        float filtered = filter.process(mixed);

        // --- Amp ---
        float ampValue = ampEnv.process();
        if (!ampEnv.isActive()) active = false;

        return filtered * ampValue * currentVelocity * 0.4f;
    }

    // Glide
    void setGlideTime(float seconds) { glideTime = seconds; }

    // Oscillator params
    void setOsc1Waveform(OscWaveform w) { osc1.setWaveform(w); }
    void setOsc2Waveform(OscWaveform w) { osc2.setWaveform(w); }
    void setOsc3Waveform(OscWaveform w) { osc3.setWaveform(w); }
    void setOsc1Range(OscRange r)       { osc1.setRange(r); updateOscFrequencies(noteToHz(currentNote)); }
    void setOsc2Range(OscRange r)       { osc2.setRange(r); updateOscFrequencies(noteToHz(currentNote)); }
    void setOsc3Range(OscRange r)       { osc3.setRange(r); updateOscFrequencies(noteToHz(currentNote)); }
    void setOsc1FineTune(float st)      { osc1.setFineTune(st); updateOscFrequencies(noteToHz(currentNote)); }
    void setOsc2FineTune(float st)      { osc2.setFineTune(st); updateOscFrequencies(noteToHz(currentNote)); }
    void setOsc3FineTune(float st)      { osc3.setFineTune(st); updateOscFrequencies(noteToHz(currentNote)); }

    // Mixer levels 0–1
    void setOsc1Level(float v)  { osc1Level  = v; }
    void setOsc2Level(float v)  { osc2Level  = v; }
    void setOsc3Level(float v)  { osc3Level  = v; }
    void setNoiseLevel(float v) { noiseLevel = v; }

    // Filter
    void setFilterCutoff(float hz)   { baseCutoff = hz; }
    void setFilterResonance(float r) { filter.setResonance(r); }
    void setFilterEnvAmount(float a) { filterEnvAmount = a; }
    void setKeyTrack(float kt)       { keyTrackAmount = kt; }

    // Filter envelope
    void setFilterAttack(float s)  { filterEnv.setAttack(s); }
    void setFilterDecay(float s)   { filterEnv.setDecay(s); }
    void setFilterSustain(float v) { filterEnv.setSustain(v); }
    void setFilterRelease(float s) { filterEnv.setRelease(s); }

    // Amp envelope
    void setAmpAttack(float s)  { ampEnv.setAttack(s); }
    void setAmpDecay(float s)   { ampEnv.setDecay(s); }
    void setAmpSustain(float v) { ampEnv.setSustain(v); }
    void setAmpRelease(float s) { ampEnv.setRelease(s); }

    // Modulation
    void setModAmount(float a)   { modAmount   = a; }
    void setModToFilter(bool b)  { modToFilter = b; }
    void setModToOsc(bool b)     { modToOsc    = b; }
    void setModMix(float mix)    { modMix      = mix; }  // 0=LFO, 1=noise
    void setModWheel(float w)    { modWheelValue = w; }  // 0-1, scales mod depth

    // LFO
    void setLfoRate(float hz)         { lfo.setRate(hz); }
    void setLfoWaveform(LfoWaveform w){ lfo.setWaveform(w); }

    // Pitch bend (-range to +range semitones)
    void setPitchBend(float semitones)
    {
        pitchBend = semitones;
        targetHz  = noteToHz(currentNote);
        if (glideTime < 0.001f) currentHz = targetHz;
    }

    // Master tune (-12 to +12 semitones)
    void setMasterTune(float semitones)
    {
        masterTune = semitones;
        targetHz   = noteToHz(currentNote);
        if (glideTime < 0.001f) currentHz = targetHz;
    }

    int getCurrentNote() const { return currentNote; }

private:
    double noteToHz(int note) const
    {
        return 440.0 * std::pow(2.0, (note - 69 + masterTune + pitchBend) / 12.0);
    }

    void updateOscFrequencies(double hz)
    {
        osc1.setFrequency(hz);
        osc2.setFrequency(hz);
        osc3.setFrequency(hz);
    }

    double sampleRate      = 44100.0;
    int    currentNote     = 60;
    float  currentVelocity = 1.0f;
    float  glideTime       = 0.0f;
    bool   active          = false;
    double currentHz       = 0.0;   // current (gliding) frequency
    double targetHz        = 440.0; // destination frequency

    Oscillator   osc1, osc2, osc3;
    Lfo          lfo;
    LadderFilter filter;
    Envelope     filterEnv, ampEnv;

    float osc1Level  = 0.8f;
    float osc2Level  = 0.0f;
    float osc3Level  = 0.0f;
    float noiseLevel = 0.0f;

    float baseCutoff      = 2000.0f;
    float filterEnvAmount = 0.5f;
    float keyTrackAmount  = 0.5f;

    float modAmount     = 0.0f;
    bool  modToFilter   = false;
    bool  modToOsc      = false;
    float modMix        = 0.0f;   // 0=LFO, 1=noise
    float modWheelValue = 1.0f;   // default 1 so knob works without a wheel
    float masterTune    = 0.0f;
    float pitchBend     = 0.0f;   // semitones from MIDI pitch wheel
    float smoothedMod   = 0.0f;
};
