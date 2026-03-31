#pragma once
#include <cmath>
#include <algorithm>

enum class OscWaveform { Sine, Triangle, RevSaw, Sawtooth, Square, WidePulse, NarrowPulse };
enum class OscRange   { Lo, F32, F16, F8, F4, F2 };

class Oscillator
{
public:
    void setSampleRate(double sr)   { sampleRate = sr; }
    void setWaveform(OscWaveform w) { waveform = w; }
    void setRange(OscRange r)       { range = r; }
    void setFineTune(float semitones) { fineTune = semitones; } // -7 to +7 semitones
    void setFrequency(double baseHz)
    {
        double rangeMultiplier = 1.0;
        switch (range)
        {
            case OscRange::Lo:  rangeMultiplier = 0.0625; break;
            case OscRange::F32: rangeMultiplier = 0.5;    break;
            case OscRange::F16: rangeMultiplier = 1.0;    break;
            case OscRange::F8:  rangeMultiplier = 2.0;    break;
            case OscRange::F4:  rangeMultiplier = 4.0;    break;
            case OscRange::F2:  rangeMultiplier = 8.0;    break;
        }
        double tuned = baseHz * rangeMultiplier * std::pow(2.0, fineTune / 12.0);
        phaseIncrement = tuned / sampleRate;
    }

    void reset() { phase = 0.0; }

    // pitchMod in semitones
    float process(float pitchMod = 0.0f)
    {
        double modInc = phaseIncrement * std::pow(2.0, pitchMod / 12.0);
        float sample = generateSample((float)phase, (float)modInc);
        phase += modInc;
        if (phase >= 1.0) phase -= 1.0;
        return sample;
    }

private:
    // PolyBLEP correction — smooths the discontinuity at phase t with step size dt
    static float polyBlep(float t, float dt)
    {
        if (t < dt)        { t /= dt; return  t + t - t*t - 1.0f; }
        if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t*t + t + t + 1.0f; }
        return 0.0f;
    }

    float generateSample(float p, float dt) const
    {
        switch (waveform)
        {
            case OscWaveform::Sine:
                return std::sin(p * 6.28318530f);

            case OscWaveform::Triangle:
                // Triangles have no hard discontinuities — naive is fine
                return p < 0.5f ? (4.0f * p - 1.0f) : (3.0f - 4.0f * p);

            case OscWaveform::RevSaw: {
                float v = 1.0f - 2.0f * p;
                v += polyBlep(p, dt);
                return v;
            }

            case OscWaveform::Sawtooth: {
                float v = 2.0f * p - 1.0f;
                v -= polyBlep(p, dt);
                return v;
            }

            case OscWaveform::Square: {
                float v = p < 0.5f ? 1.0f : -1.0f;
                v += polyBlep(p, dt);
                v -= polyBlep(std::fmod(p + 0.5f, 1.0f), dt);
                return v;
            }

            case OscWaveform::WidePulse: {    // 25% duty
                float v = p < 0.25f ? 1.0f : -1.0f;
                v += polyBlep(p, dt);
                v -= polyBlep(std::fmod(p + 0.75f, 1.0f), dt);
                return v;
            }

            case OscWaveform::NarrowPulse: {  // 10% duty
                float v = p < 0.1f ? 1.0f : -1.0f;
                v += polyBlep(p, dt);
                v -= polyBlep(std::fmod(p + 0.9f, 1.0f), dt);
                return v;
            }

            default: return 0.0f;
        }
    }

    double sampleRate     = 44100.0;
    double phase          = 0.0;
    double phaseIncrement = 0.0;
    OscWaveform waveform  = OscWaveform::Sawtooth;
    OscRange range        = OscRange::F8;
    float fineTune        = 0.0f;
};

// ── LFO ──────────────────────────────────────────────────────────────────────
enum class LfoWaveform { Sine, Triangle, Saw, Square };

class Lfo
{
public:
    void setSampleRate(double sr) { sampleRate = sr; }
    void setRate(float hz)        { phaseIncrement = (double)hz / sampleRate; }
    void setWaveform(LfoWaveform w) { waveform = w; }

    float process()
    {
        float out = generateSample((float)phase);
        phase += phaseIncrement;
        if (phase >= 1.0) phase -= 1.0;
        return out;
    }

private:
    static constexpr float kPi = 3.14159265f;

    float generateSample(float p) const
    {
        switch (waveform)
        {
            case LfoWaveform::Sine:
                return std::sin(p * 2.0f * kPi);
            case LfoWaveform::Triangle:
                return p < 0.5f ? (4.0f * p - 1.0f) : (3.0f - 4.0f * p);
            case LfoWaveform::Saw:
                return 2.0f * p - 1.0f;
            case LfoWaveform::Square:
                return p < 0.5f ? 1.0f : -1.0f;
            default: return 0.0f;
        }
    }

    double sampleRate     = 44100.0;
    double phase          = 0.0;
    double phaseIncrement = 0.0;
    LfoWaveform waveform  = LfoWaveform::Sine;
};
