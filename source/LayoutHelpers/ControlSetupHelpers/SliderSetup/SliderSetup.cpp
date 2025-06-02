//
// Created by smoke on 6/2/2025.
//

#include "SliderSetup.h"

void SliderSetup::setupRotarySlider(juce::Slider& slider, juce::Component* parent)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    if (parent != nullptr)
        parent->addAndMakeVisible(slider);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
SliderSetup::attachSlider(juce::AudioProcessorValueTreeState& apvts,
                         const juce::String& paramID,
                         juce::Slider& slider)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramID, slider);
}

