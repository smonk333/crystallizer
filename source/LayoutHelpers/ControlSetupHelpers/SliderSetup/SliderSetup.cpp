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

