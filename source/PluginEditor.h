#pragma once

#include "BinaryData.h"
#include "ProcessorLayouts/ProcessingModeLayout/ProcessingModeLayout.h"
#include "PluginProcessor.h"
#include "ProcessorLayouts/DelayLayout/DelayLayout.h"
#include "ProcessorLayouts/GranularLayout/GranularLayout.h"
#include "ProcessorLayouts/ReverbLayout/ReverbLayout.h"
#include "ProcessorLayouts/LooperLayout/LooperLayout.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    // use dedicated layout components for effect sections
    DelayLayout delayLayout;
    ReverbLayout reverbLayout;
    ProcessingModeLayout processingModeSelector;  // processing mode selector
    GranularLayout granularLayout;
    LooperLayout looperLayout;

    // add fx controls, labels, and attachments here as we build the
    // processor classes for them

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
