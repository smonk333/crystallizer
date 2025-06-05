//
// Created by smoke on 6/2/2025.
//

#include "ToggleSetup.h"

void ToggleSetup::setupToggleButton(juce::TextButton& button, const juce::String& text, juce::Component* parent)
{
    button.setButtonText(text);
    button.setClickingTogglesState(true);
    if (parent != nullptr)
        parent->addAndMakeVisible(button);
}