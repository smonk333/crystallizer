//
// Created by smoke on 6/8/2025.
//

#pragma once

#ifndef SIGNALPATHMANAGERLAYOUT_H
#define SIGNALPATHMANAGERLAYOUT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>


class SignalPathManagerLayout : public juce::Component,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit SignalPathManagerLayout (juce::AudioProcessorValueTreeState& apvts);
    ~SignalPathManagerLayout() override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& processorState;

    juce::ComboBox pathSelector;
    juce::Label pathLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> pathAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalPathManagerLayout)
};



#endif //SIGNALPATHMANAGERLAYOUT_H
