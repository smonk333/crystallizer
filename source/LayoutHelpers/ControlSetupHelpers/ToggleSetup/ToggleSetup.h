//
// Created by smoke on 6/2/2025.
//
#pragma once

#ifndef TOGGLESETUP_H
#define TOGGLESETUP_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ToggleSetup {
public:
    // sets up a standard toggle button
    static void setupToggleButton(juce::TextButton& button, const juce::String& text, juce::Component* parent);

    // connects a button to an APVTS parameter
    static std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
    attachButton(juce::AudioProcessorValueTreeState& apvts,
                const juce::String& paramID,
                juce::Button& button);
};

#endif //TOGGLESETUP_H

