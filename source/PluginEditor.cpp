#include "PluginEditor.h"
#include "LayoutHelpers/ControlSetupHelpers/SliderSetup/SliderSetup.h"
#include "LayoutHelpers/ControlSetupHelpers/LabelSetup/LabelSetup.h"
#include "LayoutHelpers/ControlSetupHelpers/ToggleSetup/ToggleSetup.h"
#include "AudioDSP/Looper/LooperProcessor.h"

// TODO: PROCESSOR_ADDITION_CHAIN(8): add layouts for new processors here
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
    processorRef (p),
    delayLayout(p.apvts),
    reverbLayout(p.apvts),
    granularLayout (p.apvts),
    looperLayout(p.apvts, *p.getSignalPathManager().getLooperProcessor()),
    spmLayout(p.apvts)
{
    // Add the layout components to the editor
    addAndMakeVisible(delayLayout);
    addAndMakeVisible(reverbLayout);
    addAndMakeVisible(granularLayout);
    addAndMakeVisible(looperLayout);
    addAndMakeVisible(spmLayout);

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
    setSize (600, 800);
    setResizable(true, true);
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

    //=standard delay section===================================================
    auto delaySection = area.removeFromTop(140);

    //=position the DelayLayout component in the delay section==================
    delayLayout.setBounds(delaySection);

    //=reverb section ==========================================================
    auto reverbSection = area.removeFromTop(200);

    //=position the ReverbLayout component in the reverb section================
    reverbLayout.setBounds(reverbSection);

    //=signal path manager section=============================================
    auto signalPathSection = area.removeFromTop(100);

    //=position the SignalPathManagerLayout component in the signal path section
    spmLayout.setBounds(signalPathSection);

    //=granular section=========================================================
    auto granularSection = area.removeFromTop(200);

    //=position the GranularLayout component in the granular section============
    granularLayout.setBounds(granularSection);

    //=looper section===========================================================
    auto looperSection = area.removeFromTop(200);

    //=position the LooperLayout component in the looper section================
    looperLayout.setBounds(looperSection);

    //=position the inspect button (TODO: REMOVE THIS LATER)====================
    inspectButton.setBounds(getWidth() - 100, getHeight() - 30, 80, 25);
}
