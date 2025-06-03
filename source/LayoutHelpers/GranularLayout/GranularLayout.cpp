//
// Created by smoke on 6/3/2025.
//

#include "GranularLayout.h"
#include "../ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "../ControlSetupHelpers/SliderSetup/SliderSetup.h"

GranularLayout::GranularLayout (juce::AudioProcessorValueTreeState& apvts)
{
    // initialize sliders
    SliderSetup::setupRotarySlider(delayTimeSlider, this);
    SliderSetup::setupRotarySlider(grainSizeSlider, this);
    SliderSetup::setupRotarySlider(grainDensitySlider, this);
    SliderSetup::setupRotarySlider(pitchShiftSlider, this);
    SliderSetup::setupRotarySlider(feedbackSlider, this);
    SliderSetup::setupRotarySlider(wetDrySlider, this);
    SliderSetup::setupRotarySlider(spreadSlider, this);

    // initialize labels
    LabelSetup::setupLabel(delayTimeLabel, "Delay Time", this);
    LabelSetup::setupLabel(grainSizeLabel, "Grain Size", this);
    LabelSetup::setupLabel(grainDensityLabel, "Grain Density", this);
    LabelSetup::setupLabel(pitchShiftLabel, "Pitch Shift", this);
    LabelSetup::setupLabel(feedbackLabel, "Feedback", this);
    LabelSetup::setupLabel(wetDryLabel, "Wet/Dry Mix", this);
    LabelSetup::setupLabel(spreadLabel, "Spread", this);

    // create slider attachments using the AttachmentSetup helper
    delayTimeAttachment = AttachmentSetup::createSliderAttachment(apvts, "delayTime", delayTimeSlider);
    grainSizeAttachment = AttachmentSetup::createSliderAttachment(apvts, "grainSize", grainSizeSlider);
    grainDensityAttachment = AttachmentSetup::createSliderAttachment(apvts, "grainDensity", grainDensitySlider);
    pitchShiftAttachment = AttachmentSetup::createSliderAttachment(apvts, "pitchShift", pitchShiftSlider);
    feedbackAttachment = AttachmentSetup::createSliderAttachment(apvts, "feedback", feedbackSlider);
    wetDryAttachment = AttachmentSetup::createSliderAttachment(apvts, "wetDry", wetDrySlider);
    spreadAttachment = AttachmentSetup::createSliderAttachment(apvts, "spread", spreadSlider);
}

void GranularLayout::resized()
{
    auto bounds = getLocalBounds();

    //TODO: continue working here
}