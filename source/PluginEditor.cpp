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

    // set up additional fx controls, labels, and attachments here as we build them




    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds().reduced(20);

    // Standard delay section
    auto standardSection = area.removeFromTop(100);
    auto standardRow = standardSection.removeFromTop(80);
    auto standardLabelRow = standardSection;

    auto sliderWidth = standardRow.getWidth() / 3; // 3 controls

    delayTimeSlider.setBounds(area.removeFromLeft(sliderWidth));
    feedbackSlider.setBounds(area.removeFromLeft(sliderWidth));
    wetDrySlider.setBounds(area.removeFromLeft(sliderWidth));

    // standard delay labels
    auto labelWidth = standardLabelRow.getWidth() / 3;
    delayTimeLabel.setBounds(standardLabelRow.removeFromLeft(labelWidth));
    feedbackLabel.setBounds(standardLabelRow.removeFromLeft(labelWidth));
    wetDryLabel.setBounds(standardLabelRow.removeFromLeft(labelWidth));

}
