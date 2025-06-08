#pragma once

#include "BinaryData.h"
#include "ProcessorLayouts/ProcessingModeLayout/ProcessingModeLayout.h"
#include "PluginProcessor.h"
#include "ProcessorLayouts/DelayLayout/DelayLayout.h"
#include "ProcessorLayouts/GranularLayout/GranularLayout.h"
#include "ProcessorLayouts/ReverbLayout/ReverbLayout.h"
#include "ProcessorLayouts/LooperLayout/LooperLayout.h"
#include "ProcessorLayouts/SignalPathManagerLayout/SignalPathManagerLayout.h"
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

    // TODO: PROCESSOR_ADDITION_CHAIN(7) add an instance of the processor's layout class here

    // use dedicated layout components for effect sections
    DelayLayout delayLayout;
    ReverbLayout reverbLayout;
    GranularLayout granularLayout;
    LooperLayout looperLayout;
    SignalPathManagerLayout spmLayout;

    // legacy processing mode selector
    // ProcessingModeLayout processingModeSelector;  // processing mode selector

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

