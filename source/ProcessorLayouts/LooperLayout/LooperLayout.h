//
// Created by smoke on 6/5/2025.
//

#pragma once

#ifndef LOOPERLAYOUT_H
#define LOOPERLAYOUT_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

// Forward declaration for friendship
class LooperStateListener;

class LooperLayout : public juce::GroupComponent,
                     public juce::Button::Listener
{
public:
    LooperLayout(juce::AudioProcessorValueTreeState& apvts);
    ~LooperLayout() override;
    void resized() override;

    // Button listener callback
    void buttonClicked(juce::Button* button) override;

    // Made public so the listener can access it
    void updateButtonStates(int looperState);

private:
    // Button setup
    juce::TextButton recordButton, playButton, overdubButton, stopButton, clearButton;

    // Reference to the APVTS for setting parameter values
    juce::AudioProcessorValueTreeState& apvts;

    // Parameter pointer for updating UI state from parameter changes
    std::atomic<float>* looperStateParam = nullptr;

    // For updating UI when parameter changes
    std::unique_ptr<LooperStateListener> parameterListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperLayout)
};

#endif //LOOPERLAYOUT_H