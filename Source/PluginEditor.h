#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KamLookAndFeel.h"

using Attachment       = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

class Kam00gEditor : public juce::AudioProcessorEditor
{
public:
    Kam00gEditor(Kam00gProcessor&);
    ~Kam00gEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Kam00gProcessor& processor;
    KamLookAndFeel   laf;

    struct KnobGroup
    {
        juce::Slider slider;
        juce::Label  label;
        std::unique_ptr<Attachment> attachment;

        void setup(juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& paramId,
                   const juce::String& labelText,
                   juce::Component* parent,
                   juce::LookAndFeel* lf)
        {
            slider.setSliderStyle(juce::Slider::Rotary);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            slider.setLookAndFeel(lf);
            parent->addAndMakeVisible(slider);
            label.setText(labelText, juce::dontSendNotification);
            label.setJustificationType(juce::Justification::centred);
            label.setLookAndFeel(lf);
            parent->addAndMakeVisible(label);
            attachment = std::make_unique<Attachment>(apvts, paramId, slider);
        }

        void place(int x, int y, int w, int h)
        {
            slider.setBounds(x, y, w, h);
            label.setBounds(x - 4, y + h + 1, w + 8, 14);
        }
    };

    struct ComboGroup
    {
        juce::ComboBox combo;
        juce::Label    label;
        std::unique_ptr<ComboAttachment> attachment;

        void setup(juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& paramId,
                   const juce::String& labelText,
                   juce::Component* parent,
                   juce::LookAndFeel* lf)
        {
            // Manually populate items — ComboBoxAttachment does NOT do this
            if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(paramId)))
                combo.addItemList(param->choices, 1);

            combo.setLookAndFeel(lf);
            parent->addAndMakeVisible(combo);
            label.setText(labelText, juce::dontSendNotification);
            label.setJustificationType(juce::Justification::centred);
            label.setLookAndFeel(lf);
            parent->addAndMakeVisible(label);
            attachment = std::make_unique<ComboAttachment>(apvts, paramId, combo);
        }

        void place(int x, int y, int w, int h)
        {
            combo.setBounds(x, y, w, h);
            label.setBounds(x, y + h + 2, w, 13);
        }
    };

    struct ToggleGroup
    {
        juce::ToggleButton button;
        std::unique_ptr<ButtonAttachment> attachment;

        void setup(juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& paramId,
                   const juce::String& labelText,
                   juce::Component* parent)
        {
            button.setButtonText(labelText);
            parent->addAndMakeVisible(button);
            attachment = std::make_unique<ButtonAttachment>(apvts, paramId, button);
        }
    };

    // Controllers
    KnobGroup masterTune, glideKnob, bendRange;

    // OSC
    ComboGroup osc1Wave, osc1Range;
    ComboGroup osc2Wave, osc2Range;
    ComboGroup osc3Wave, osc3Range;
    KnobGroup  osc1Tune, osc2Tune, osc3Tune;

    // Mixer
    KnobGroup osc1Level, osc2Level, osc3Level, noiseLevel;

    // Filter
    KnobGroup cutoff, resonance, filterEnvAmt, keyTrack;

    // Filter Envelope
    KnobGroup fAttack, fDecay, fSustain, fRelease;

    // Amp Envelope
    KnobGroup aAttack, aDecay, aSustain, aRelease;

    // Modulation
    KnobGroup   modAmt, modMix, lfoRate;
    ComboGroup  lfoWave;
    ToggleGroup modToFilter, modToOsc;

    // FX
    KnobGroup drive, chorusRate, chorusDepth, chorusMix;

    // Output
    KnobGroup volume;

    // Preset
    juce::ComboBox presetCombo;
    juce::Label    presetLabel;

    // Preset data
    struct PresetData
    {
        const char* name;
        float osc1wave, osc1range, osc1tune, osc1level;
        float osc2wave, osc2range, osc2tune, osc2level;
        float osc3wave, osc3range, osc3tune, osc3level;
        float noiselevel;
        float cutoff, resonance, filterenvamt, keytrack;
        float fattack, fdecay, fsustain, frelease;
        float aattack, adecay, asustain, arelease;
        float modamt, modtofilter, modtoosc, modmix;
        float lforate, lfowave;
        float mastertune, glide, volume;
    };

    static const PresetData presets[6];
    void loadPreset(int presetId);

    void paintSection(juce::Graphics& g, int x, int y, int w, int h,
                      juce::Colour fill, juce::Colour accent, const juce::String& title);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Kam00gEditor)
};
