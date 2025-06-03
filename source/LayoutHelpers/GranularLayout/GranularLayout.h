//
// Created by smoke on 6/3/2025.
//

#pragma once

#ifndef GRANULARLAYOUT_H
#define GRANULARLAYOUT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>



class GranularLayout : public juce::Component
{
public:
    GranularLayout(juce::AudioProcessorValueTreeState& apvts);
    ~GranularLayout() override = default;
    void resized() override;

private:
    // sliders and labels
    juce::Slider delayTimeSlider, grainSizeSlider, grainDensitySlider,
                 pitchShiftSlider, feedbackSlider, wetDrySlider,
                 spreadSlider;
    juce::Label delayTimeLabel, grainSizeLabel, grainDensityLabel,
                pitchShiftLabel, feedbackLabel, wetDryLabel,
                spreadLabel;

    // attachments for the sliders
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
                delayTimeAttachment,
                grainSizeAttachment, grainDensityAttachment,
                pitchShiftAttachment, feedbackAttachment,
                wetDryAttachment, spreadAttachment;
};



#endif //GRANULARLAYOUT_H
