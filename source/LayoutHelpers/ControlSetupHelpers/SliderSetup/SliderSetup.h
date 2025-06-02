//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef SLIDERSETUP_H
#define SLIDERSETUP_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class SliderSetup {
public:
    // sets up a rotary slider with standard appearance
    static void setupRotarySlider(juce::Slider& slider, juce::Component* parent);

    // connects a slider to an APVTS parameter
    static std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    attachSlider(juce::AudioProcessorValueTreeState& apvts,
                const juce::String& paramID,
                juce::Slider& slider);
};

#endif //SLIDERSETUP_H



