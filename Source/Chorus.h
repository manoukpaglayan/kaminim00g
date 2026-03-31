#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

// Simple stereo chorus — two delay lines modulated by quadrature LFOs
class StereoChorus
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        int maxDelay = (int)(0.04 * sr) + 2;   // 40ms max
        bufL.assign(maxDelay, 0.0f);
        bufR.assign(maxDelay, 0.0f);
        bufSize  = maxDelay;
        writePos = 0;
        lfoPhase = 0.0f;
    }

    void setRate(float hz)   { rateInc = hz / (float)sampleRate; }
    void setDepth(float d)   { depthSec = d * 0.007f; }  // 0–7ms modulation depth
    void setMix(float m)     { mix = m; }

    std::pair<float,float> process(float input)
    {
        // Quadrature LFO (90° apart → widest stereo image)
        const float twoPi = 6.28318530f;
        float lfoL = std::sin(lfoPhase * twoPi);
        float lfoR = std::cos(lfoPhase * twoPi);

        lfoPhase += rateInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;

        float center   = (float)sampleRate * 0.015f;        // 15ms centre
        float modRange = (float)sampleRate * depthSec;

        float delayL = center + lfoL * modRange;
        float delayR = center + lfoR * modRange;

        bufL[writePos] = input;
        bufR[writePos] = input;

        float wetL = readAt(bufL, delayL);
        float wetR = readAt(bufR, delayR);

        if (++writePos >= bufSize) writePos = 0;

        float dry = 1.0f - mix;
        return { input * dry + wetL * mix,
                 input * dry + wetR * mix };
    }

    void reset()
    {
        std::fill(bufL.begin(), bufL.end(), 0.0f);
        std::fill(bufR.begin(), bufR.end(), 0.0f);
        writePos = 0;
        lfoPhase = 0.0f;
    }

private:
    float readAt(const std::vector<float>& buf, float delaySamples) const
    {
        float readF = (float)writePos - delaySamples;
        while (readF < 0.0f) readF += (float)bufSize;
        int i0 = (int)readF % bufSize;
        int i1 = (i0 + 1) % bufSize;
        float frac = readF - (float)(int)readF;
        return buf[i0] * (1.0f - frac) + buf[i1] * frac;
    }

    double sampleRate = 44100.0;
    std::vector<float> bufL, bufR;
    int   bufSize  = 0;
    int   writePos = 0;
    float lfoPhase = 0.0f;
    float rateInc  = 0.0f;
    float depthSec = 0.003f;
    float mix      = 0.0f;
};
