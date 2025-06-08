//
// Created by smoke on 6/6/2025.
//

#pragma once

#ifndef SIGNALPATHMANAGER_H
#define SIGNALPATHMANAGER_H

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

// include processor headers here
#include "../Granular-Delay/GranularProcessor.h"
#include "../Reverb/ReverbProcessor.h"
#include "../Standard-Delay/DelayProcessor.h"
#include "../Looper/LooperProcessor.h"

// add #include directives above for additional processors as we add them


class SignalPathManager : public juce::dsp::ProcessorBase
{
public:
    SignalPathManager();
    ~SignalPathManager() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // processing mode enum
    enum ProcessingMode
    {
        DelayOnly = 0,
        ReverbOnly = 1,
        GranularOnly = 2,
        LooperOnly = 3,
        Serial = 4,
        // TODO: PROCESSOR_ADDITION_CHAIN(9): add a new processing mode for the
        //       new processor here
    };

    // set the processing mode (this will initialize the necessary chains)
    void setProcessingMode(ProcessingMode newMode);

    // get the current processing mode
    ProcessingMode getCurrentMode() const { return currentMode; }

    // get processor references for parameter updates
    // this is more efficient than having separate parameter update methods
    DelayProcessor* getDelayProcessor();
    ReverbProcessor* getReverbProcessor();
    GranularProcessor* getGranularProcessor();
    LooperProcessor* getLooperProcessor();

    // update processor chain parameters based on the current mode
    void updateProcessorChainParameters (const juce::AudioProcessorValueTreeState& apvts);

private:
    // define processor chain index constants
    enum
    {
        looper,
        delay,
        granular,
        reverb
        // TODO: PROCESSOR_ADDITION_CHAIN(10): add a new index for the new
        //       processor here
    };

    // current processing mode
    ProcessingMode currentMode = DelayOnly;

    // process spec for lazily initializing processors
    juce::dsp::ProcessSpec currentSpec;

    // TODO: PROCESSOR_ADDITION_CHAIN(11): add new processor chain pointers here

    // processor chain pointers (only initialized when needed)
    std::unique_ptr<juce::dsp::ProcessorChain<DelayProcessor>> delayChain;
    std::unique_ptr<juce::dsp::ProcessorChain<ReverbProcessor>> reverbChain;
    std::unique_ptr<juce::dsp::ProcessorChain<GranularProcessor>> granularChain;
    std::unique_ptr<juce::dsp::ProcessorChain<LooperProcessor>> looperChain;

    // serial chain setup:
    // add more processors as we create them and put them in the order that the
    // effects need to be processed in.
    std::unique_ptr<juce::dsp::ProcessorChain<LooperProcessor,
                                              DelayProcessor,
                                              GranularProcessor,
                                              ReverbProcessor>> serialChain;

    // TODO: PROCESSOR_ADDITION_CHAIN(12): add the processor to the initialization logic

    // helper methods for initialization
    void initializeDelayChain();
    void initializeReverbChain();
    void initializeGranularChain();
    void initializeLooperChain();
    void initializeSerialChain();

    // clean up unused chains to free memory
    void cleanupUnusedChains();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalPathManager)
};

#endif //SIGNALPATHMANAGER_H

