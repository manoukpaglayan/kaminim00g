#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "Chorus.h"

class Kam00gProcessor : public juce::AudioProcessor
{
public:
    Kam00gProcessor();
    ~Kam00gProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "kam00g"; }
    bool   acceptsMidi()  const override { return true; }
    bool   producesMidi() const override { return false; }
    bool   isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.5; }

    int  getNumPrograms()    override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    SynthVoice   voice;
    StereoChorus chorus;

    void updateVoiceParameters();

    int   currentNote    = -1;
    bool  noteIsOn       = false;
    float bendRangeSemi  = 2.0f;   // semitones for full pitch wheel deflection

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Kam00gProcessor)
};
