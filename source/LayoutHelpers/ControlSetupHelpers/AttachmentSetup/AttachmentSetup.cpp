//
// Created by smoke on 6/2/2025.
//

#include "AttachmentSetup.h"

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
AttachmentSetup::createSliderAttachment(juce::AudioProcessorValueTreeState& apvts,
                                     const juce::String& paramID,
                                     juce::Slider& slider)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramID, slider);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
AttachmentSetup::createButtonAttachment(juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& paramID,
                                      juce::Button& button)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, paramID, button);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
AttachmentSetup::createComboBoxAttachment(juce::AudioProcessorValueTreeState& apvts,
                                        const juce::String& paramID,
                                        juce::ComboBox& comboBox)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, paramID, comboBox);
}
