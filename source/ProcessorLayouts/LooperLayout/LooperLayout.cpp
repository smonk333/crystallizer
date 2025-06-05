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

    // set up attachments for looper controls using the AttachmentSetup helper
    recordAttach = AttachmentSetup::createButtonAttachment(apvts,
        "looperRecord", recordButton);
    playAttach = AttachmentSetup::createButtonAttachment(apvts,
        "looperPlay", playButton);
    overdubAttach = AttachmentSetup::createButtonAttachment(apvts,
        "looperOverdub", overdubButton);
    stopAttach = AttachmentSetup::createButtonAttachment(apvts,
        "looperStop", stopButton);
    clearAttach = AttachmentSetup::createButtonAttachment(apvts,
        "looperClear", clearButton);
}

void LooperLayout::resized()
{
//     auto bounds = getLocalBounds();
//
//     // arrange buttons using FlexBox
//     juce::FlexBox flexBox;
//     flexBox.flexDirection = juce::FlexBox::Direction::column;
//     flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
//     flexBox.alignItems = juce::FlexBox::AlignItems::stretch;
//
//     // set up margin for buttons
//     const float margin = 5.0f;
//
//     // first row: record and play
//     juce::FlexBox row1;
//     row1.flexDirection = juce::FlexBox::Direction::row;
//     row1.items.add(juce::FlexItem(recordButton).withMargin(margin).withFlex(1));
//     row1.items.add(juce::FlexItem(playButton).withMargin(margin).withFlex(1));
//     flexBox.items.add(juce::FlexItem(row1).withFlex(1));
//
//     // second row: overdub and stop
//     juce::FlexBox row2;
//     row2.flexDirection = juce::FlexBox::Direction::row;
//     row2.items.add(juce::FlexItem(overdubButton).withMargin(margin).withFlex(1));
//     row2.items.add(juce::FlexItem(stopButton).withMargin(margin).withFlex(1));
//     flexBox.items.add(juce::FlexItem(row2).withFlex(1));
//
//     // third row: clear button spanning both columns
//     flexBox.items.add(juce::FlexItem(clearButton).withMargin(margin).withFlex(1));
//
//     // perform the layout
//     flexBox.performLayout(bounds.toFloat());

    auto bounds = getLocalBounds();

    // set up margin for buttons
    const int margin = 5;

    // create a 3x2 grid (3 rows, 2 columns)
    juce::Grid grid;
    grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), juce::Grid::TrackInfo(juce::Grid::Fr(1)), juce::Grid::TrackInfo(juce::Grid::Fr(1)) };
    grid.templateColumns = { juce::Grid::TrackInfo(juce::Grid::Fr(1)), juce::Grid::TrackInfo(juce::Grid::Fr(1)) };

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