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

    // Processing mode enum
    enum ProcessingMode
    {
        DelayOnly = 0,
        ReverbOnly = 1,
        GranularOnly = 2,
        LooperOnly = 3,
        Serial = 4,
        Parallel = 5,
        // Add new modes here as needed
    };

    // Set the processing mode (this will initialize the necessary chains)
    void setProcessingMode(ProcessingMode newMode);

    // Get the current processing mode
    ProcessingMode getCurrentMode() const { return currentMode; }

    // Get processor references for parameter updates
    // This is more efficient than having separate parameter update methods
    DelayProcessor* getDelayProcessor();
    ReverbProcessor* getReverbProcessor();
    GranularProcessor* getGranularProcessor();
    LooperProcessor* getLooperProcessor();

private:
    // Define processor chain index constants
    enum
    {
        delayIndex,
        reverbIndex
    };

    // Current processing mode
    ProcessingMode currentMode = DelayOnly;

    // Process spec for lazily initializing processors
    juce::dsp::ProcessSpec currentSpec;
    bool isPrepared = false;

    // Processor chain pointers (only initialized when needed)
    std::unique_ptr<juce::dsp::ProcessorChain<DelayProcessor>> delayChain;
    std::unique_ptr<juce::dsp::ProcessorChain<ReverbProcessor>> reverbChain;
    std::unique_ptr<juce::dsp::ProcessorChain<DelayProcessor, ReverbProcessor>> serialChain;
    std::unique_ptr<juce::dsp::ProcessorChain<GranularProcessor>> granularChain;
    std::unique_ptr<juce::dsp::ProcessorChain<LooperProcessor>> looperChain;

    // Buffers for parallel processing
    juce::AudioBuffer<float> parallelDelayBuffer;
    juce::AudioBuffer<float> parallelReverbBuffer;

    // Helper methods for initialization
    void initializeDelayChain();
    void initializeReverbChain();
    void initializeSerialChain();
    void initializeGranularChain();
    void initializeLooperChain();
    void initializeParallelChain();

    // Clean up unused chains to free memory
    void cleanupUnusedChains();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalPathManager)
};



#endif //SIGNALPATHMANAGER_H
