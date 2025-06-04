#include "PluginEditor.h"
#include "LayoutHelpers/ControlSetupHelpers/SliderSetup/SliderSetup.h"
#include "LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      delayLayout(p.apvts), reverbLayout(p.apvts),
      processingModeSelector(p.apvts), granularLayout (p.apvts)
{
    // Add the layout components to the editor
    addAndMakeVisible(delayLayout);
    addAndMakeVisible(reverbLayout);
    addAndMakeVisible(processingModeSelector);
    addAndMakeVisible(granularLayout);

    // Add a button to inspect the UI in the melatonin inspector
    addAndMakeVisible(inspectButton);

    // This chunk of code instantiates and opens the melatonin inspector
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
    g.drawText("ROUTING MODE", 20, 350, 560, 20, juce::Justification::left);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds().reduced(20);

    //=standard delay section===================================================
    auto delaySection = area.removeFromTop(140);
    delaySection.removeFromTop(30); // space for section label

    //=position the DelayLayout component in the delay section==================
    delayLayout.setBounds(delaySection);

    //=reverb section ==========================================================
    auto reverbSection = area.removeFromTop(200);
    reverbSection.removeFromTop(30); // space for section label

    //=position the ReverbLayout component in the reverb section================
    reverbLayout.setBounds(reverbSection);

    //=processing mode selector section=========================================
    auto processingSection = area.removeFromTop(100);
    processingSection.removeFromTop(30); // space for section label

    // position the ProcessingModeSelector component
    processingModeSelector.setBounds(processingSection);

    //=granular section=========================================================
    auto granularSection = area.removeFromTop(200);
    granularSection.removeFromTop(30); // space for section label

    //=position the GranularLayout component in the granular section============
    granularLayout.setBounds(granularSection);

    //=position the inspect button (TODO: REMOVE THIS LATER)====================
    inspectButton.setBounds(getWidth() - 100, getHeight() - 30, 80, 25);
}
