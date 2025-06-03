//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef PROCESSINGMODELAYOUT_H
#define PROCESSINGMODELAYOUT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ProcessingModeLayout : public juce::Component,
                            public juce::AudioProcessorValueTreeState::Listener
{
public:
    ProcessingModeLayout(juce::AudioProcessorValueTreeState& apvts);
    ~ProcessingModeLayout() override;
    void resized() override;

    // AudioProcessorValueTreeState::Listener implementation
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::AudioProcessorValueTreeState& processorState;

    juce::ComboBox modeSelector;
    juce::Label modeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessingModeLayout)
};

#endif //PROCESSINGMODELAYOUT_H
