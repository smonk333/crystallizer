//
// Created by smoke on 6/5/2025.
//

#include "LooperLayout.h"
#include "../../LayoutHelpers/ControlSetupHelpers/AttachmentSetup/AttachmentSetup.h"
#include "../../LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>

// Custom message for looper state change
class LooperStateChangeMessage : public juce::Message {
public:
    LooperStateChangeMessage(int newState) : state(newState) {}
    int state;
};

// Custom message for looper clear (one-shot)
class LooperClearMessage : public juce::Message {
public:
    LooperClearMessage() = default;
};

// Helper class to monitor parameter changes and update UI
class LooperStateListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    LooperStateListener(LooperLayout& parent) : owner(parent) {}

    void parameterChanged(const juce::String& parameterID, float newValue) override
    {
        if (parameterID == "looperState")
        {
            // Use MessageManager to safely update UI from audio thread
            juce::MessageManager::callAsync([this, newValue]() {
                owner.updateButtonStates(static_cast<int>(newValue));
            });
        }
    }

private:
    LooperLayout& owner;
};

LooperLayout::LooperLayout(juce::AudioProcessorValueTreeState& apvts, LooperProcessor& looperProcessor)
    : apvts(apvts), looperProcessor(looperProcessor)
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

    // Add this class as a listener to each button
    recordButton.addListener(this);
    playButton.addListener(this);
    overdubButton.addListener(this);
    stopButton.addListener(this);
    clearButton.addListener(this);

    // Get the looper state parameter
    looperStateParam = apvts.getRawParameterValue("looperState");

    // Initial UI update based on current parameter value
    updateButtonStates(static_cast<int>(looperStateParam->load()));

    // Set up parameter listener for external changes
    parameterListener = std::make_unique<LooperStateListener>(*this);
    apvts.addParameterListener("looperState", parameterListener.get());

    DBG("LooperLayout: Created with single parameter state control");

    timerLabel = std::make_unique<juce::Label>("LooperTimer", "Time: 0.00s");
    stateLabel = std::make_unique<juce::Label>("LooperState", "State: Stopped");
    addAndMakeVisible(*timerLabel);
    addAndMakeVisible(*stateLabel);
    startTimerHz(1000); // Start the timer to update 1000 times per second
}

LooperLayout::~LooperLayout()
{
    // Remove parameter listener
    apvts.removeParameterListener("looperState",
        dynamic_cast<LooperStateListener*>(parameterListener.get()));

    // Remove button listeners
    recordButton.removeListener(this);
    playButton.removeListener(this);
    overdubButton.removeListener(this);
    stopButton.removeListener(this);
    clearButton.removeListener(this);
}

void LooperLayout::buttonClicked(juce::Button* button)
{
    int newState = -1;

    if (button == &recordButton)
        newState = 0; // Recording
    else if (button == &playButton)
        newState = 1; // Playing
    else if (button == &overdubButton)
        newState = 2; // Overdubbing
    else if (button == &stopButton)
        newState = 3; // Stopped
    else if (button == &clearButton) {
        // Only post a clear message if not already in Clear state
        if (static_cast<int>(looperProcessor.getState()) != 4) {
            postMessage(new LooperClearMessage());
        }
        return;
    }

    if (newState >= 0)
    {
        // Set the parameter value
        apvts.getParameterAsValue("looperState").setValue(newState);
        DBG("LooperLayout: Button clicked, setting looperState to " << newState);
    }
}

void LooperLayout::updateButtonStates(int looperState)
{
    // Update button states based on looperState parameter
    recordButton.setToggleState(looperState == 0, juce::dontSendNotification);
    playButton.setToggleState(looperState == 1, juce::dontSendNotification);
    overdubButton.setToggleState(looperState == 2, juce::dontSendNotification);
    stopButton.setToggleState(looperState == 3, juce::dontSendNotification);
    // Clear button is momentary and doesn't stay toggled

    DBG("LooperLayout: Updated button states to reflect looperState " << looperState);
}

void LooperLayout::updateTimerAndState() {
    timerLabel->setText(juce::String("Time: ") + juce::String(looperProcessor.getElapsedTimeSeconds(), 2) + "s", juce::dontSendNotification);
    stateLabel->setText(juce::String("State: ") + looperProcessor.getStateString(), juce::dontSendNotification);
}

void LooperLayout::timerCallback() {
    if (looperProcessor.consumeBufferLimitReachedFlag()) {
        postMessage(new LooperStateChangeMessage(1)); // 1 = Playing
    }
    if (looperProcessor.consumeClearProcessedFlag()) {
        postMessage(new LooperStateChangeMessage(3)); // 3 = Stopped
    }
    updateTimerAndState();
}

// override handleMessage to process custom messages
void LooperLayout::handleMessage(const juce::Message& message) {
    if (auto* looperMsg = dynamic_cast<const LooperStateChangeMessage*>(&message)) {
        if (auto* param = apvts.getParameter("looperState")) {
            param->beginChangeGesture();
            param->setValueNotifyingHost(looperMsg->state / param->getNormalisableRange().getRange().getLength());
            param->endChangeGesture();
        }
    } else if (dynamic_cast<const LooperClearMessage*>(&message)) {
        if (auto* param = apvts.getParameter("looperState")) {
            param->beginChangeGesture();
            param->setValueNotifyingHost(4.0f / param->getNormalisableRange().getRange().getLength()); // 4 = Clear
            param->endChangeGesture();
        }
    }
}

void LooperLayout::resized()
{
    auto bounds = getLocalBounds();

    // get up margin for buttons
    const int margin = 5;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    // create a 3x2 grid (3 rows, 2 columns)
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

    auto area = getLocalBounds();
    timerLabel->setBounds(area.removeFromTop(24));
    stateLabel->setBounds(area.removeFromTop(24));
}