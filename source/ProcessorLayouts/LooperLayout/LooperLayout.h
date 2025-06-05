//
// Created by smoke on 6/5/2025.
//

#pragma once

#ifndef LOOPERLAYOUT_H
#define LOOPERLAYOUT_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class LooperLayout : public juce::Component
{
public:
    LooperLayout(juce::AudioProcessorValueTreeState& apvts);
    ~LooperLayout() override = default;
    void resized() override;

private:
    // button setup
    juce::TextButton
        recordButton, playButton, overdubButton, stopButton, clearButton;

    // attachments for buttons
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        recordAttach, playAttach, overdubAttach, stopAttach, clearAttach;
};



#endif //LOOPERLAYOUT_H
