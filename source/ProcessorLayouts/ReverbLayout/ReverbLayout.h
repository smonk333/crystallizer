//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef REVERBLAYOUT_H
#define REVERBLAYOUT_H

    #include <juce_audio_processors/juce_audio_processors.h>
    #include <juce_gui_basics/juce_gui_basics.h>

class ReverbLayout : public juce::GroupComponent
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbLayout)
};

#endif //REVERBLAYOUT_H
