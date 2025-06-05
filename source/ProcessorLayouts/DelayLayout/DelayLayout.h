//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef DELAYLAYOUT_H
#define DELAYLAYOUT_H

    #include <juce_audio_processors/juce_audio_processors.h>
    #include <juce_gui_basics/juce_gui_basics.h>

class DelayLayout : public juce::Component
{
public:
    DelayLayout(juce::AudioProcessorValueTreeState& apvts);
    ~DelayLayout() override = default;
    void resized() override;

private:
    // sliders and labels
    juce::Slider delayTimeSlider, feedbackSlider, wetDrySlider;
    juce::Label delayTimeLabel, feedbackLabel, wetDryLabel;

    // attachments for the sliders
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        delayTimeSliderAttach, feedbackSliderAttach, wetDrySliderAttach;
};

#endif //DELAYLAYOUT_H
