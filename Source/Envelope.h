#pragma once

class Envelope
{
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    void setSampleRate(double sr) { sampleRate = sr; }

    void setAttack(float seconds)  { attackTime  = std::max(0.001f, seconds); }
    void setDecay(float seconds)   { decayTime   = std::max(0.001f, seconds); }
    void setSustain(float level)   { sustainLevel = juce::jlimit(0.0f, 1.0f, level); }
    void setRelease(float seconds) { releaseTime = std::max(0.001f, seconds); }

    void noteOn()
    {
        stage = Stage::Attack;
        // Re-trigger from current value to avoid clicks
    }

    void noteOff()
    {
        if (stage != Stage::Idle)
            stage = Stage::Release;
    }

    bool isActive() const { return stage != Stage::Idle; }

    float process()
    {
        switch (stage)
        {
            case Stage::Attack:
                currentLevel += 1.0f / (float)(attackTime * sampleRate);
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    stage = Stage::Decay;
                }
                break;

            case Stage::Decay:
                currentLevel -= (1.0f - sustainLevel) / (float)(decayTime * sampleRate);
                if (currentLevel <= sustainLevel)
                {
                    currentLevel = sustainLevel;
                    stage = Stage::Sustain;
                }
                break;

            case Stage::Sustain:
                currentLevel = sustainLevel;
                break;

            case Stage::Release:
                currentLevel -= currentLevel / (float)(releaseTime * sampleRate);
                if (currentLevel < 0.0001f)
                {
                    currentLevel = 0.0f;
                    stage = Stage::Idle;
                }
                break;

            case Stage::Idle:
            default:
                currentLevel = 0.0f;
                break;
        }
        return currentLevel;
    }

    float getCurrentLevel() const { return currentLevel; }

private:
    double sampleRate  = 44100.0;
    Stage  stage       = Stage::Idle;
    float  currentLevel = 0.0f;
    float  attackTime   = 0.01f;
    float  decayTime    = 0.1f;
    float  sustainLevel = 0.7f;
    float  releaseTime  = 0.3f;
};
