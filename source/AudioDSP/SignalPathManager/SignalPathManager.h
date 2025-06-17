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
        Serial = 4
        // TODO: PROCESSOR_ADDITION_CHAIN(9): add a new processing mode for the
        //       new processor here
    };

    // set the processing mode (this will initialize the necessary chains)
    void setProcessingMode(ProcessingMode newMode);

    // get the current processing mode
    [[nodiscard]] ProcessingMode getCurrentMode() const { return currentMode; }

    // get processor references for parameter updates
    // this is more efficient than having separate parameter update methods
    DelayProcessor* getDelayProcessor();
    ReverbProcessor* getReverbProcessor();
    GranularProcessor* getGranularProcessor();
    LooperProcessor* getLooperProcessor();

    // template getter for processors in the main chain
    template<typename ProcessorType, int Index>
    ProcessorType& getProcessorFromChain()
    {
        jassert(processorChain != nullptr);
        return processorChain->template get<Index>();
    }

    // update processor chain parameters based on the current mode
    void updateProcessorChainParameters (const juce::AudioProcessorValueTreeState& apvts);

private:
    // define processor chain index constants
    enum ProcessorIndex
    {
        looper,
        delay,
        granular,
        reverb
        // TODO: PROCESSOR_ADDITION_CHAIN(10): add a new index for the new
        //       processor here
    };

    // Map of processor indices to their active states
    // TODO: PROCESSOR_ADDITION_CHAIN(23): add a new flag to set the new processor's active state
    std::unordered_map<ProcessorIndex, bool> processorActiveStates = {
        {looper, false},
        {delay, false},
        {granular, false},
        {reverb, false} // this is a placeholder for the serial chain, not a processor
        // When adding a new processor, add it here too
    };

    // Helper function to set processor bypass states
    template<typename ChainType>
    void setProcessorBypassStates(ChainType& chain)
    {
        // Template specializations to handle each processor type
        if constexpr (std::is_same_v<ChainType, MainChainType>) {
            // TODO: PROCESSOR_ADDITION_CHAIN(21): add a new bypass handler for the
            //       new processor here
            chain.setBypassed<looper>(!processorActiveStates[looper]);
            chain.setBypassed<delay>(!processorActiveStates[delay]);
            chain.setBypassed<granular>(!processorActiveStates[granular]);
            chain.setBypassed<reverb>(!processorActiveStates[reverb]);
        }
    }

    // current processing mode
    ProcessingMode currentMode = DelayOnly;

    // process spec for initializing processors
    juce::dsp::ProcessSpec currentSpec;

    // Serial chain type definition (used for all chains)
    using MainChainType = juce::dsp::ProcessorChain<
        LooperProcessor,
        DelayProcessor,
        GranularProcessor,
        ReverbProcessor>;

    // TODO: PROCESSOR_ADDITION_CHAIN(11): add new processor chain pointers here
    //       + add a flag for it to track whether or not it's enabled

    // single processor chain instance
    std::unique_ptr<MainChainType> processorChain;

    // Initialize and configure the processor chain for the current mode
    void initializeProcessorChain();

    // Helper methods to get processor references from the main chain
    DelayProcessor& getDelayFromChain();
    ReverbProcessor& getReverbFromChain();
    GranularProcessor& getGranularFromChain();
    LooperProcessor& getLooperFromChain();

    // Configure which processors are active in the current chain
    void updateActiveProcessors();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalPathManager)
};

#endif //SIGNALPATHMANAGER_H

