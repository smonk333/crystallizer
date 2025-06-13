//
// Created by smoke on 6/5/2025.
//

#pragma once

#ifndef LOOPERLAYOUT_H
#define LOOPERLAYOUT_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class LooperLayout : public juce::GroupComponent
{
public:
    LooperLayout(juce::AudioProcessorValueTreeState& apvts);
    ~LooperLayout() override = default;
    void resized() override;

private:
    // Button setup
    juce::TextButton recordButton, playButton, overdubButton, stopButton, clearButton;

    // Simple button attachments - one parameter per button
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        recordAttachment, playAttachment, overdubAttachment, stopAttachment, clearAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperLayout)
};

#endif //LOOPERLAYOUT_H