//
// Created by smoke on 6/2/2025.
//

#include "LabelSetup.h"

void LabelSetup::setupLabel(juce::Label& label, const juce::String& text, juce::Component* parent)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    if (parent != nullptr)
        parent->addAndMakeVisible(label);
}


