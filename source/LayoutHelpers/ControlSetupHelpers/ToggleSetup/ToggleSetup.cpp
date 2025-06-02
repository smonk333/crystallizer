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

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
ToggleSetup::attachButton(juce::AudioProcessorValueTreeState& apvts,
                        const juce::String& paramID,
                        juce::Button& button)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, paramID, button);
}

