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

    //=standard delay section (updating to use FlexBox layout)==================

    auto delaySection = area.removeFromTop(140);
    delaySection.removeFromTop(30); // space for section label

    juce::FlexBox delayControls;
    delayControls.flexDirection = juce::FlexBox::Direction::row;
    delayControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    delayControls.items.add(juce::FlexItem(delayTimeSlider).withFlex(1));
    delayControls.items.add(juce::FlexItem(feedbackSlider).withFlex(1));
    delayControls.items.add(juce::FlexItem(wetDrySlider).withFlex(1));

    // delay labels
    juce::FlexBox delayLabels;
    delayLabels.flexDirection = juce::FlexBox::Direction::row;
    delayLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    delayLabels.items.add(juce::FlexItem(delayTimeLabel).withFlex(1));
    delayLabels.items.add(juce::FlexItem(feedbackLabel).withFlex(1));
    delayLabels.items.add(juce::FlexItem(wetDryLabel).withFlex(1));

    //=reverb section ==========================================================

    auto reverbSection = area.removeFromTop(200);
    reverbSection.removeFromTop(30); // space for section label

    // upper row controls
    juce::FlexBox reverbTopControls;
    reverbTopControls.flexDirection = juce::FlexBox::Direction::row;
    reverbTopControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    reverbTopControls.items.add(juce::FlexItem(reverbRoomSizeSlider).withFlex(1));
    reverbTopControls.items.add(juce::FlexItem(reverbDampingSlider).withFlex(1));
    reverbTopControls.items.add(juce::FlexItem(reverbMixSlider).withFlex(1));

    // upper row labels
    juce::FlexBox reverbTopLabels;
    reverbTopLabels.flexDirection = juce::FlexBox::Direction::row;
    reverbTopLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    reverbTopLabels.items.add(juce::FlexItem(reverbRoomSizeLabel).withFlex(1));
    reverbTopLabels.items.add(juce::FlexItem(reverbDampingLabel).withFlex(1));
    reverbTopLabels.items.add(juce::FlexItem(reverbMixLabel).withFlex(1));

    // lower row controls
    juce::FlexBox reverbBottomControls;
    reverbBottomControls.flexDirection = juce::FlexBox::Direction::row;
    reverbBottomControls.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    reverbBottomControls.items.add(juce::FlexItem(reverbWidthSlider).withFlex(1));
    reverbBottomControls.items.add(juce::FlexItem(reverbFreezeButton).withFlex(1));
    reverbBottomControls.items.add(juce::FlexItem().withFlex(1)); // empty space for balance

    // lower row labels
    juce::FlexBox reverbBottomLabels;
    reverbBottomLabels.flexDirection = juce::FlexBox::Direction::row;
    reverbBottomLabels.justifyContent = juce::FlexBox::JustifyContent::spaceAround;

    reverbBottomLabels.items.add(juce::FlexItem(reverbWidthLabel).withFlex(1));
    reverbBottomLabels.items.add(juce::FlexItem(reverbFreezeLabel).withFlex(1));
    reverbBottomLabels.items.add(juce::FlexItem().withFlex(1)); // empty space for balance

    //=add more sections here as we build the processor classes for them========


    //=perform layouts==========================================================

    //=delay section============================================================
    delayControls.performLayout(delaySection.removeFromTop(80));
    delayLabels.performLayout(delaySection);

    //=reverb section===========================================================
    reverbTopControls.performLayout(reverbSection.removeFromTop(60));
    reverbTopLabels.performLayout(reverbSection.removeFromTop(20));
    reverbBottomControls.performLayout(reverbSection.removeFromTop(60));
    reverbBottomLabels.performLayout(reverbSection);

    //=add more sections here as we build the processor classes for them========


    //=position the inspect button (TODO: REMOVE THIS LATER)====================
    inspectButton.setBounds(getWidth() - 100, getHeight() - 30, 80, 25);

}
