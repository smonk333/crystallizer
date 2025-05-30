#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    // Standard delay slider, attachment and label setup
    juce::Slider delayTimeSlider, feedbackSlider, wetDrySlider;
    juce::Label delayTimeLabel, feedbackLabel, wetDryLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        delayTimeSliderAttach, feedbackSliderAttach, wetDrySliderAttach;

    // add fx controls, labels, and attachments here as we build the
    // processor classes for them

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
