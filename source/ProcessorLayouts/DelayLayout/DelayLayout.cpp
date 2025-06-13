//
// Created by smoke on 6/2/2025.
//

#include "DelayLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/SliderSetup/SliderSetup.h"

DelayLayout::DelayLayout(juce::AudioProcessorValueTreeState& apvts)
{
    // initialize sliders and labels
    SliderSetup::setupRotarySlider(delayTimeSlider, this);
    SliderSetup::setupRotarySlider(feedbackSlider, this);
    SliderSetup::setupRotarySlider(wetDrySlider, this);

    // initialize labels
    LabelSetup::setupLabel(delayTimeLabel, "Delay Time", this);
    LabelSetup::setupLabel(feedbackLabel, "Feedback", this);
    LabelSetup::setupLabel(wetDryLabel, "Wet/Dry Mix", this);

    // create slider attachments using the AttachmentSetup helper
    delayTimeSliderAttach = AttachmentSetup::createSliderAttachment(apvts,
        "delayTime", delayTimeSlider);
    feedbackSliderAttach = AttachmentSetup::createSliderAttachment(apvts,
        "feedback", feedbackSlider);
    wetDrySliderAttach = AttachmentSetup::createSliderAttachment(apvts,
        "wetDry", wetDrySlider);
}

void DelayLayout::resized()
{
    auto bounds = getLocalBounds();

    // create FlexBox for sliders
    juce::FlexBox sliderBox;
    sliderBox.flexDirection = juce::FlexBox::Direction::row;
    sliderBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    sliderBox.items.add(juce::FlexItem(delayTimeSlider).withFlex(1));
    sliderBox.items.add(juce::FlexItem(feedbackSlider).withFlex(1));
    sliderBox.items.add(juce::FlexItem(wetDrySlider).withFlex(1));

    // create FlexBox for labels
    juce::FlexBox labelBox;
    labelBox.flexDirection = juce::FlexBox::Direction::row;
    labelBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    labelBox.items.add(juce::FlexItem(delayTimeLabel).withFlex(1));
    labelBox.items.add(juce::FlexItem(feedbackLabel).withFlex(1));
    labelBox.items.add(juce::FlexItem(wetDryLabel).withFlex(1));

    // remove 20px from the top for spacing
    bounds.removeFromTop(20);

    // perform layout
    sliderBox.performLayout(bounds.removeFromTop(80));
    labelBox.performLayout(bounds.removeFromTop(20));
    this->setText("Standard Delay");
}

