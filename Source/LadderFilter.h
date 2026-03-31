#pragma once
#include <cmath>
#include <algorithm>

// Huovilainen Moog Ladder Filter model
// 24dB/oct resonant lowpass — the core of the Minimoog sound
class LadderFilter
{
public:
    void setSampleRate(double sr)
    {
        sampleRate = sr;
        updateCoefficients();
    }

    // cutoff 20–20000 Hz
    void setCutoff(float hz)
    {
        cutoffHz = hz;
        updateCoefficients();
    }

    // resonance 0–1 (self-oscillates above ~0.95)
    void setResonance(float r)
    {
        resonance = r;
        updateCoefficients();
    }

    float process(float input)
    {
        // Thermal noise constant
        const float THERMAL = 0.000025f;

        float x = input - resonance4 * y4;

        // Four one-pole filters in series
        y1 = y1 + p * (tanh((x + THERMAL) * thermal) - tanh_y1);
        tanh_y1 = std::tanh(y1 * thermal);

        y2 = y2 + p * (tanh_y1 - tanh_y2);
        tanh_y2 = std::tanh(y2 * thermal);

        y3 = y3 + p * (tanh_y2 - tanh_y3);
        tanh_y3 = std::tanh(y3 * thermal);

        y4 = y4 + p * (tanh_y3 - std::tanh(y4 * thermal));

        return y4;
    }

    void reset()
    {
        y1 = y2 = y3 = y4 = 0.0f;
        tanh_y1 = tanh_y2 = tanh_y3 = 0.0f;
    }

private:
    void updateCoefficients()
    {
        float f = cutoffHz * 2.0f / (float)sampleRate;
        f = std::min(f, 0.99f);

        p = f * (1.8f - 0.8f * f);
        float k = 2.0f * std::sin(f * (float)M_PI * 0.5f) - 1.0f;
        float t1 = (1.0f - p) * 1.386249f;
        float t2 = 12.0f + t1 * t1;
        resonance4 = resonance * (t2 + 6.0f * t1) / (t2 - 6.0f * t1);
        thermal = 1.0f / 0.000025f;
    }

    double sampleRate   = 44100.0;
    float  cutoffHz     = 1000.0f;
    float  resonance    = 0.0f;
    float  p            = 0.0f;
    float  resonance4   = 0.0f;
    float  thermal      = 40000.0f;
    float  y1 = 0, y2 = 0, y3 = 0, y4 = 0;
    float  tanh_y1 = 0, tanh_y2 = 0, tanh_y3 = 0;
};
