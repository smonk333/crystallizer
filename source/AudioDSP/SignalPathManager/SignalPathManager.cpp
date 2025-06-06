//
// Created by smoke on 6/6/2025.
//

#include "SignalPathManager.h"
#include <unordered_map>
#include <functional>

SignalPathManager::SignalPathManager()
{
    // empty constructor
}

SignalPathManager::~SignalPathManager()
{
    // empty destructor
}

void SignalPathManager::prepare (const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec; // store the spec for later use

    // use the modeInitializers map to initialize the chain for the current mode
    if (auto it = modeInitializers.find(currentMode); it != modeInitializers.end())
    {
        it->second();
    }

    isPrepared = true; // mark as prepared
}

void SignalPathManager::reset()
{
    // Iterate through the list and execute each reset function

}

void SignalPathManager::setProcessingMode (ProcessingMode newMode)
{
    if (currentMode == newMode)
        return; // avoid reinitializing if the mode hasn't changed

    currentMode = newMode;

    // Find and execute the initializer for the current mode
    if (auto it = modeInitializers.find(currentMode); it != modeInitializers.end())
    {
        it->second();
    }
}

DelayProcessor* SignalPathManager::getDelayProcessor()
{
    if (delayChain)
        // return the first processor (delay) in the delay chain
        return &delayChain->get<0>();
    return nullptr;
}

ReverbProcessor* SignalPathManager::getReverbProcessor()
{
    if (reverbChain)
        // return the first processor (reverb) in the reverb chain
        return &reverbChain->get<0>();
    return nullptr;
}

GranularProcessor* SignalPathManager::getGranularProcessor()
{
    if (granularChain)
        // return the first processor (granular) in the granular chain
        return &granularChain->get<0>();
    return nullptr;
}

LooperProcessor* SignalPathManager::getLooperProcessor()
{
    if (looperChain)
        // return the first processor (looper) in the looper chain
        return &looperChain->get<0>();
    return nullptr;
}

void SignalPathManager::initializeReverbChain()
{
    if (!reverbChain)
    {
        reverbChain = std::make_unique<juce::dsp::ProcessorChain<ReverbProcessor>>();
        reverbChain->get<0>().prepare(currentSpec);
    }
}

void SignalPathManager::initializeDelayChain()
{
    if (!delayChain)
    {
        delayChain = std::make_unique<juce::dsp::ProcessorChain<DelayProcessor>>();
        delayChain->get<0>().prepare(currentSpec);
    }
}

void SignalPathManager::initializeGranularChain()
{
    if (!granularChain)
    {
        granularChain = std::make_unique<juce::dsp::ProcessorChain<GranularProcessor>>();
        granularChain->get<0>().prepare(currentSpec);
    }
}

void SignalPathManager::initializeLooperChain()
{
    if (!looperChain)
    {
        looperChain = std::make_unique<juce::dsp::ProcessorChain<LooperProcessor>>();
        looperChain->get<0>().prepare(currentSpec);
    }
}

void SignalPathManager::initializeSerialChain()
{
    if (!serialChain)
    {
        // Create the serial chain if it doesn't exist
        serialChain = std::make_unique<juce::dsp::ProcessorChain<
            LooperProcessor,
            DelayProcessor,
            GranularProcessor,
            ReverbProcessor>>();
        // add processors here as we create them, in the order that the effect should be applied
    }
}

void SignalPathManager::cleanupUnusedChains()
{
    // Reset chains that are not required for the current mode
    if (currentMode != DelayOnly && delayChain)
        delayChain.reset();

    if (currentMode != ReverbOnly && reverbChain)
        reverbChain.reset();

    if (currentMode != GranularOnly && granularChain)
        granularChain.reset();

    if (currentMode != LooperOnly && looperChain)
        looperChain.reset();

    if (currentMode != Serial && serialChain)
        serialChain.reset();
}