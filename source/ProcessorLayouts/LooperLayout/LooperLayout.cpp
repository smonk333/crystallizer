//
// Created by smoke on 6/5/2025.
//

#include "LooperLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"

LooperLayout::LooperLayout(juce::AudioProcessorValueTreeState& apvts)
{
    // Set up looper control buttons
    ToggleSetup::setupToggleButton(recordButton, "Record", this);
    ToggleSetup::setupToggleButton(playButton, "Play", this);
    ToggleSetup::setupToggleButton(overdubButton, "Overdub", this);
    ToggleSetup::setupToggleButton(stopButton, "Stop", this);
    ToggleSetup::setupToggleButton(clearButton, "Clear", this);

    // Set buttons to radio button mode (only one can be selected at a time)
    recordButton.setRadioGroupId(1);
    playButton.setRadioGroupId(1);
    overdubButton.setRadioGroupId(1);
    stopButton.setRadioGroupId(1);
    // Clear button is not in radio group as it's an action button

    // Create simple button attachments - each button has its own parameter
    recordAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "looperRecord", recordButton);
    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "looperPlay", playButton);
    overdubAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "looperOverdub", overdubButton);
    stopAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "looperStop", stopButton);
    clearAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "looperClear", clearButton);

    DBG("LooperLayout: Created with individual parameter attachments");
}

void LooperLayout::resized()
{
    auto bounds = getLocalBounds();

    // Set up margin for buttons
    const int margin = 5;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    // Create a 3x2 grid (3 rows, 2 columns)
    juce::Grid grid;
    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    // Add items to the grid
    grid.items.addArray({
        juce::GridItem(recordButton).withMargin(margin),   // row 1, col 1
        juce::GridItem(playButton).withMargin(margin),     // row 1, col 2
        juce::GridItem(overdubButton).withMargin(margin),  // row 2, col 1
        juce::GridItem(stopButton).withMargin(margin),     // row 2, col 2
        juce::GridItem(clearButton).withMargin(margin).withArea(3, 1, 4, 3)  // row 3, span both columns
    });

    // Add spacing
    bounds.removeFromTop(20);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    bounds.removeFromBottom(10);

    // Perform the layout
    grid.performLayout(bounds);
    this->setText("Looper");
}