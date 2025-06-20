//
// Created by smoke on 6/2/2025.
//

#include "ReverbLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/SliderSetup/SliderSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"

ReverbLayout::ReverbLayout(juce::AudioProcessorValueTreeState& apvts)
{
    // set up reverb control sliders
    SliderSetup::setupRotarySlider(roomSizeSlider, this);
    SliderSetup::setupRotarySlider(dampingSlider, this);
    SliderSetup::setupRotarySlider(mixSlider, this);
    SliderSetup::setupRotarySlider(widthSlider, this);
    ToggleSetup::setupToggleButton(freezeButton, "Freeze", this);

    // set up reverb control labels
    LabelSetup::setupLabel(roomSizeLabel, "Room Size", this);
    LabelSetup::setupLabel(dampingLabel, "Damping", this);
    LabelSetup::setupLabel(mixLabel, "Wet/Dry Mix", this);
    LabelSetup::setupLabel(widthLabel, "Width", this);
    LabelSetup::setupLabel(freezeLabel, "Freeze", this);

    // set up attachments for reverb controls using the AttachmentSetup helper
    roomSizeAttach = AttachmentSetup::createSliderAttachment(apvts,
        "reverbRoomSize", roomSizeSlider);
    dampingAttach = AttachmentSetup::createSliderAttachment(apvts,
        "reverbDamping", dampingSlider);
    mixAttach = AttachmentSetup::createSliderAttachment(apvts,
        "reverbMix", mixSlider);
    widthAttach = AttachmentSetup::createSliderAttachment(apvts,
        "reverbWidth", widthSlider);
    freezeAttach = AttachmentSetup::createButtonAttachment(apvts,
        "reverbFreeze", freezeButton);
}

void ReverbLayout::resized()
{
    auto bounds = getLocalBounds();

    // upper row controls
    juce::FlexBox topControls;
    topControls.flexDirection = juce::FlexBox::Direction::row;
    topControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    topControls.items.add(juce::FlexItem(roomSizeSlider).withFlex(1));
    topControls.items.add(juce::FlexItem(dampingSlider).withFlex(1));
    topControls.items.add(juce::FlexItem(mixSlider).withFlex(1));

    // upper row labels
    juce::FlexBox topLabels;
    topLabels.flexDirection = juce::FlexBox::Direction::row;
    topLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    topLabels.items.add(juce::FlexItem(roomSizeLabel).withFlex(1));
    topLabels.items.add(juce::FlexItem(dampingLabel).withFlex(1));
    topLabels.items.add(juce::FlexItem(mixLabel).withFlex(1));

    // lower row controls
    juce::FlexBox bottomControls;
    bottomControls.flexDirection = juce::FlexBox::Direction::row;
    bottomControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    bottomControls.items.add(juce::FlexItem(widthSlider).withFlex(1));
    bottomControls.items.add(juce::FlexItem(freezeButton).withFlex(1));
    bottomControls.items.add(juce::FlexItem().withFlex(1)); // empty space for balance

    // lower row labels
    juce::FlexBox bottomLabels;
    bottomLabels.flexDirection = juce::FlexBox::Direction::row;
    bottomLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    bottomLabels.items.add(juce::FlexItem(widthLabel).withFlex(1));
    bottomLabels.items.add(juce::FlexItem(freezeLabel).withFlex(1));
    bottomLabels.items.add(juce::FlexItem().withFlex(1)); // empty space for balance

    // add spacing for label
    bounds.removeFromTop(20);

    // perform layouts
    topControls.performLayout(bounds.removeFromTop(60));
    topLabels.performLayout(bounds.removeFromTop(20));
    bottomControls.performLayout(bounds.removeFromTop(60));
    bottomLabels.performLayout(bounds.removeFromTop(20));
    this->setText("Reverb");
}
