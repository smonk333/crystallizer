//
// Created by smoke on 6/8/2025.
//

#include "SignalPathManagerLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"

SignalPathManagerLayout::SignalPathManagerLayout (juce::AudioProcessorValueTreeState& apvts)
    : processorState(apvts)
{
    // set up a comboBox to select the signal path
    addAndMakeVisible(pathSelector);

    // add path options - must match the choices in AudioParameterChoice in PluginProcessor.cpp
    // TODO: PROCESSOR_ADDITION_CHAIN(19): Add new routing options to the combo
    //       box to allow them to show up in the GUI

    pathSelector.addItem("Delay Only", 1);
    pathSelector.addItem("Reverb Only", 2);
    pathSelector.addItem("Granular Only", 3);
    pathSelector.addItem("Looper Only", 4);
    pathSelector.addItem("Serial", 5);

    // add a label
    addAndMakeVisible(pathLabel);
    pathLabel.setText("Signal Path:", juce::dontSendNotification);
    pathLabel.setJustificationType(juce::Justification::right);
    pathLabel.attachToComponent(&pathSelector, true);

    // connect the ComboBox directly to the parameter
    pathAttachment = AttachmentSetup::createComboBoxAttachment(
        processorState, "signalPath", pathSelector);

    // listen for parameter changes (in case it's changed from somewhere else)
       processorState.addParameterListener("signalPath", this);

}

SignalPathManagerLayout::~SignalPathManagerLayout()
{
    processorState.removeParameterListener("signalPath", this);
}

void SignalPathManagerLayout::resized()
{
    auto area = getLocalBounds();

    // position the combo box to take up available space
    // label will automatically position itself attached to the combo box
    pathSelector.setBounds (area.withTrimmedLeft (100));
}

void SignalPathManagerLayout::parameterChanged (const juce::String& parameterID, float newValue)
{
    // Removed logic to notify PluginProcessor directly.
    juce::ignoreUnused(parameterID, newValue);
}
