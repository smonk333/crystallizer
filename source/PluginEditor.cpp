#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    auto& params = processorRef.apvts;

    // lambda function to set up sliders
    auto setupSlider = [this](juce::Slider& s) {
        s.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(&s);
    };

    // lambda function to set up labels
    auto setupLabel = [this](juce::Label& l, const juce::String& text) {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(&l);
    };

    // set up standard delay control sliders
    setupSlider(delayTimeSlider);
    setupSlider(feedbackSlider);
    setupSlider(wetDrySlider);

    // set up standard delay control labels
    setupLabel(delayTimeLabel, "Delay Time");
    setupLabel(feedbackLabel, "Feedback");
    setupLabel(wetDryLabel, "Wet/Dry Mix");

    // set up attachments for standard delay controls
    delayTimeSliderAttach = std::make_unique<SliderAttachment>(params, "delayTime", delayTimeSlider);
    feedbackSliderAttach = std::make_unique<SliderAttachment>(params, "feedback", feedbackSlider);
    wetDrySliderAttach = std::make_unique<SliderAttachment>(params, "wetDry", wetDrySlider);

    // reverb controls =========================================================

    // set up reverb control sliders
    setupSlider(reverbRoomSizeSlider);
    setupSlider(reverbDampingSlider);
    setupSlider(reverbMixSlider);
    setupSlider(reverbWidthSlider);
    addAndMakeVisible(&reverbFreezeButton); // add button to editor
    reverbFreezeButton.setClickingTogglesState(true); // make button toggleable

    // set up reverb control labels
    setupLabel(reverbRoomSizeLabel, "Room Size");
    setupLabel(reverbDampingLabel, "Damping");
    setupLabel(reverbMixLabel, "Wet/Dry Mix");
    setupLabel(reverbWidthLabel, "Width");
    setupLabel(reverbFreezeLabel, "Freeze");

    // set up attachments for reverb controls
    reverbRoomSizeAttach = std::make_unique<SliderAttachment>(params, "reverbRoomSize", reverbRoomSizeSlider);
    reverbDampingAttach = std::make_unique<SliderAttachment>(params, "reverbDamping", reverbDampingSlider);
    reverbMixAttach = std::make_unique<SliderAttachment>(params, "reverbMix", reverbMixSlider);
    reverbWidthAttach = std::make_unique<SliderAttachment>(params, "reverbWidth", reverbWidthSlider);
    reverbFreezeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(params, "reverbFreeze", reverbFreezeButton);

    // set up additional fx controls, labels, and attachments here as we build them


    // add a button to inspect the UI in the melatonin inspector
    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
    setResizable(true, true);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("DELAY", 20, 10, 560, 20, juce::Justification::left);
    g.drawText("REVERB", 20, 160, 560, 20, juce::Justification::left);
}

void PluginEditor::resized()
{
// layout the positions of your child components here
    auto area = getLocalBounds().reduced(20);

    // Standard delay section
    auto delaySection = area.removeFromTop(140);
    delaySection.removeFromTop(30); // space for section label
    auto delayRow = delaySection.removeFromTop(80);
    auto delayLabelRow = delaySection;

    auto delaySliderWidth = delayRow.getWidth() / 3; // 3 controls
    delayTimeSlider.setBounds(delayRow.removeFromLeft(delaySliderWidth));
    feedbackSlider.setBounds(delayRow.removeFromLeft(delaySliderWidth));
    wetDrySlider.setBounds(delayRow.removeFromLeft(delaySliderWidth));

    // delay labels
    auto delayLabelWidth = delayLabelRow.getWidth() / 3;
    delayTimeLabel.setBounds(delayLabelRow.removeFromLeft(delayLabelWidth));
    feedbackLabel.setBounds(delayLabelRow.removeFromLeft(delayLabelWidth));
    wetDryLabel.setBounds(delayLabelRow.removeFromLeft(delayLabelWidth));

    // reverb section ==========================================================
    auto reverbSection = area.removeFromTop(200);
    reverbSection.removeFromTop(30); // space for section label

    // split reverb into two rows (3 controls each)
    auto reverbRowTop = reverbSection.removeFromTop(60);
    auto reverbLabelRowTop = reverbSection.removeFromTop(20);
    auto reverbRowBottom = reverbSection.removeFromTop(60);
    auto reverbLabelRowBottom = reverbSection.removeFromTop(20);

    auto reverbSliderWidth = reverbRowTop.getWidth() / 3;

    // top row: Room Size, Damping, Wet Level
    reverbRoomSizeSlider.setBounds(reverbRowTop.removeFromLeft(reverbSliderWidth));
    reverbDampingSlider.setBounds(reverbRowTop.removeFromLeft(reverbSliderWidth));
    reverbMixSlider.setBounds(reverbRowTop.removeFromLeft(reverbSliderWidth));

    // top row labels
    auto reverbLabelWidth = reverbLabelRowTop.getWidth() / 3;
    reverbRoomSizeLabel.setBounds(reverbLabelRowTop.removeFromLeft(reverbLabelWidth));
    reverbDampingLabel.setBounds(reverbLabelRowTop.removeFromLeft(reverbLabelWidth));
    reverbMixLabel.setBounds(reverbLabelRowTop.removeFromLeft(reverbLabelWidth));

    // bottom row: Dry Level, Width, Freeze
    reverbWidthSlider.setBounds(reverbRowBottom.removeFromLeft(reverbSliderWidth));
    reverbFreezeButton.setBounds(reverbRowBottom.removeFromLeft(reverbSliderWidth)); // button instead of slider

    // inspect button (remove later)
    inspectButton.setBounds(getWidth() - 100, getHeight() - 30, 80, 25);

    // bottom row labels
    reverbLabelWidth = reverbLabelRowBottom.getWidth() / 3;
    reverbWidthLabel.setBounds(reverbLabelRowBottom.removeFromLeft(reverbLabelWidth));
    reverbFreezeLabel.setBounds(reverbLabelRowBottom.removeFromLeft(reverbLabelWidth));

}
