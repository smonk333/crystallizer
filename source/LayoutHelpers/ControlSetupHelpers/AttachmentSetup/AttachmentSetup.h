//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef ATTACHMENTSETUP_H
#define ATTACHMENTSETUP_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class AttachmentSetup {
public:
    // create a slider attachment
    static std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    createSliderAttachment(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& paramID,
                          juce::Slider& slider);

    // create a button attachment
    static std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
    createButtonAttachment(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& paramID,
                          juce::Button& button);

    // create a combo box attachment
    static std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
    createComboBoxAttachment(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& paramID,
                            juce::ComboBox& comboBox);
};

#endif //ATTACHMENTSETUP_H
