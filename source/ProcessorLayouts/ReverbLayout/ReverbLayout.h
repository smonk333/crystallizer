//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef REVERBLAYOUT_H
#define REVERBLAYOUT_H

    #include <juce_audio_processors/juce_audio_processors.h>
    #include <juce_gui_basics/juce_gui_basics.h>

class ReverbLayout : public juce::Component
{
public:
    ReverbLayout(juce::AudioProcessorValueTreeState& apvts);
    ~ReverbLayout() override = default;
    void resized() override;

private:
    // sliders, buttons and labels
    juce::Slider roomSizeSlider, dampingSlider, mixSlider, widthSlider;
    juce::TextButton freezeButton;

    juce::Label roomSizeLabel, dampingLabel, mixLabel, widthLabel, freezeLabel;

    // attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        roomSizeAttach, dampingAttach, mixAttach, widthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttach;
};

#endif //REVERBLAYOUT_H
