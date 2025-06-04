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
    delayTimeAttachment = AttachmentSetup::createSliderAttachment(apvts, "granularDelayTime", delayTimeSlider);
    grainSizeAttachment = AttachmentSetup::createSliderAttachment(apvts, "grainSize", grainSizeSlider);
    grainDensityAttachment = AttachmentSetup::createSliderAttachment(apvts, "grainDensity", grainDensitySlider);
    pitchShiftAttachment = AttachmentSetup::createSliderAttachment(apvts, "pitchShift", pitchShiftSlider);
    feedbackAttachment = AttachmentSetup::createSliderAttachment(apvts, "granularFeedback", feedbackSlider);
    wetDryAttachment = AttachmentSetup::createSliderAttachment(apvts, "granularWetDry", wetDrySlider);
    spreadAttachment = AttachmentSetup::createSliderAttachment(apvts, "spread", spreadSlider);
}

void GranularLayout::resized()
{
    auto bounds = getLocalBounds();

    // upper row controls
    juce::FlexBox topControls;
    topControls.flexDirection = juce::FlexBox::Direction::row;
    topControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    topControls.items.add(juce::FlexItem(delayTimeSlider).withFlex(1));
    topControls.items.add(juce::FlexItem(grainSizeSlider).withFlex(1));
    topControls.items.add(juce::FlexItem(grainDensitySlider).withFlex(1));

    // upper row labels
    juce::FlexBox topLabels;
    topLabels.flexDirection = juce::FlexBox::Direction::row;
    topLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    topLabels.items.add(juce::FlexItem(delayTimeLabel).withFlex(1));
    topLabels.items.add(juce::FlexItem(grainSizeLabel).withFlex(1));
    topLabels.items.add(juce::FlexItem(grainDensityLabel).withFlex(1));

    // lower row controls
    juce::FlexBox bottomControls;
    bottomControls.flexDirection = juce::FlexBox::Direction::row;
    bottomControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    bottomControls.items.add(juce::FlexItem(pitchShiftSlider).withFlex(1));
    bottomControls.items.add(juce::FlexItem(feedbackSlider).withFlex(1));
    bottomControls.items.add(juce::FlexItem(wetDrySlider).withFlex(1));
    bottomControls.items.add(juce::FlexItem(spreadSlider).withFlex(1));

    // lower row labels
    juce::FlexBox bottomLabels;
    bottomLabels.flexDirection = juce::FlexBox::Direction::row;
    bottomLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    bottomLabels.items.add(juce::FlexItem(pitchShiftLabel).withFlex(1));
    bottomLabels.items.add(juce::FlexItem(feedbackLabel).withFlex(1));
    bottomLabels.items.add(juce::FlexItem(wetDryLabel).withFlex(1));
    bottomLabels.items.add(juce::FlexItem(spreadLabel).withFlex(1));

    // perform layouts
    topControls.performLayout(bounds.removeFromTop(60));
    topLabels.performLayout(bounds.removeFromTop(20));
    bottomControls.performLayout(bounds.removeFromTop(60));
    bottomLabels.performLayout(bounds.removeFromTop(20));
}

