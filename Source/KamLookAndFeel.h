#pragma once
#include <JuceHeader.h>

class KamLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KamLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFFE8E0C8));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        float radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        float cx = x + width  * 0.5f;
        float cy = y + height * 0.5f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillEllipse(cx - radius + 2, cy - radius + 2, radius * 2.0f, radius * 2.0f);

        // Knob body — cream/ivory
        juce::ColourGradient grad(juce::Colour(0xFFD8D0B0), cx - radius * 0.3f, cy - radius * 0.3f,
                                  juce::Colour(0xFF908870), cx + radius * 0.4f, cy + radius * 0.4f, true);
        g.setGradientFill(grad);
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        // Knob rim
        g.setColour(juce::Colour(0xFF505040));
        g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        // Indicator line
        juce::Path pointer;
        float pointerLen = radius * 0.65f;
        pointer.addRectangle(-1.5f, -pointerLen, 3.0f, pointerLen * 0.7f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillPath(pointer);

        // Center dot (color from slider name hint — passed via componentID)
        g.setColour(juce::Colour(0xFFCC4400));
        g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(juce::Colour(0xFFE0D8B8));
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawFittedText(label.getText(), label.getLocalBounds(),
                         juce::Justification::centred, 1);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                      int, int, int, int, juce::ComboBox& box) override
    {
        g.setColour(juce::Colour(0xFF1A1A14));
        g.fillRoundedRectangle(0, 0, width, height, 3.0f);
        g.setColour(juce::Colour(0xFF504830));
        g.drawRoundedRectangle(0.5f, 0.5f, width - 1.0f, height - 1.0f, 3.0f, 1.0f);
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(juce::Colour(0xFF1A1A14));
    }
};
