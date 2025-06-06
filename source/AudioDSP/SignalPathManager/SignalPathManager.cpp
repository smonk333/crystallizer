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

}

void SignalPathManager::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
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

void SignalPathManager::initializeParallelChain()
{
}

void SignalPathManager::initializeSerialChain()
{
}

void SignalPathManager::cleanupUnusedChains()
{
}