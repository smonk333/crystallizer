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

}

void SignalPathManagerLayout::resized()
{

}
void SignalPathManagerLayout::parameterChanged (const juce::String& parameterID, float newValue)
{

}
