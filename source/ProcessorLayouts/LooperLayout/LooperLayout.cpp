//
// Created by smoke on 6/5/2025.
//

#include "LooperLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"

LooperLayout::LooperLayout (juce::AudioProcessorValueTreeState& apvts)
{
    // set up looper control buttons
    ToggleSetup::setupToggleButton(recordButton, "Record", this);
    ToggleSetup::setupToggleButton(playButton, "Play", this);
    ToggleSetup::setupToggleButton(overdubButton, "Overdub", this);
    ToggleSetup::setupToggleButton(stopButton, "Stop", this);
    ToggleSetup::setupToggleButton(clearButton, "Clear", this);

    // disable toggle state
    for (auto* btn : {&recordButton, &playButton, &overdubButton, &stopButton, &clearButton})
    {
        btn->setClickingTogglesState(false);
    }

    // TODO: PROCESSOR_ADDITION_CHAIN(EXAMPLE): this is where the parameter names are set up
    // set up button actions to update APVTS parameters
    recordButton.onClick = [&]() { apvts.getParameter("looperState")->setValueNotifyingHost(1.0f); }; // Recording
    playButton.onClick = [&]() { apvts.getParameter("looperState")->setValueNotifyingHost(2.0f); };   // Playing
    overdubButton.onClick = [&]() { apvts.getParameter("looperState")->setValueNotifyingHost(3.0f); }; // Overdubbing
    stopButton.onClick = [&]() { apvts.getParameter("looperState")->setValueNotifyingHost(0.0f); };    // Stopped
    clearButton.onClick = [&]() { apvts.getParameter("looperState")->setValueNotifyingHost(4.0f); };   // Clear

    // set up attachments for looper controls using the AttachmentSetup helper
    recordAttach = AttachmentSetup::createButtonAttachment(apvts, "looperRecord", recordButton);
    playAttach = AttachmentSetup::createButtonAttachment(apvts, "looperPlay", playButton);
    overdubAttach = AttachmentSetup::createButtonAttachment(apvts, "looperOverdub", overdubButton);
    stopAttach = AttachmentSetup::createButtonAttachment(apvts, "looperStop", stopButton);
    clearAttach = AttachmentSetup::createButtonAttachment(apvts, "looperClear", clearButton);
}

void LooperLayout::resized()
{
    auto bounds = getLocalBounds();

    // set up margin for buttons
    const int margin = 5;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    // create a 3x2 grid (3 rows, 2 columns)
    juce::Grid grid;
    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    // add items to the grid
    grid.items.addArray({
        juce::GridItem(recordButton).withMargin(margin),   // row 1, col 1
        juce::GridItem(playButton).withMargin(margin),     // row 1, col 2
        juce::GridItem(overdubButton).withMargin(margin),  // row 2, col 1
        juce::GridItem(stopButton).withMargin(margin),     // row 2, col 2
        juce::GridItem(clearButton).withMargin(margin).withArea(3, 1, 4, 3)  // row 3, span both columns
    });

    // perform the layout
    grid.performLayout(bounds);
}