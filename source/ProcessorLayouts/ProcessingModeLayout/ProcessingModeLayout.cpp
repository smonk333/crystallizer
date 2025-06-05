//
// Created by smoke on 6/2/2025.
//

#include "ProcessingModeLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"

ProcessingModeLayout::ProcessingModeLayout(juce::AudioProcessorValueTreeState& apvts)
    : processorState(apvts)
{
    // Set up the ComboBox
    addAndMakeVisible(modeSelector);

    // Add the mode options - these must match the choices in AudioParameterChoice
    modeSelector.addItem("Delay Only", 1);
    modeSelector.addItem("Reverb Only", 2);
    modeSelector.addItem("Serial", 3);
    modeSelector.addItem("Granular Only (test)", 4);
    modeSelector.addItem("Parallel", 5);

    // Add the label
    addAndMakeVisible(modeLabel);
    modeLabel.setText("Routing Mode:", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::right);
    modeLabel.attachToComponent(&modeSelector, true);

    // Connect the ComboBox directly to the parameter
    modeAttachment = AttachmentSetup::createComboBoxAttachment(
        processorState, "processingMode", modeSelector);

    // Listen for parameter changes (in case it's changed from elsewhere)
    processorState.addParameterListener("processingMode", this);
}

ProcessingModeLayout::~ProcessingModeLayout()
{
    processorState.removeParameterListener("processingMode", this);
}

void ProcessingModeLayout::resized()
{
    auto area = getLocalBounds();

    // Position the combo box to take up available space
    // Label will automatically position itself attached to the combo box
    modeSelector.setBounds(area.withTrimmedLeft(100));
}

// AudioProcessorValueTreeState::Listener implementation
void ProcessingModeLayout::parameterChanged(const juce::String& parameterID, float newValue)
{
    // We don't need to do anything here as the attachment handles updates
    juce::ignoreUnused(parameterID, newValue);
}
