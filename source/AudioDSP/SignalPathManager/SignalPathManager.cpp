//
// Created by smoke on 6/6/2025.
//

#include "SignalPathManager.h"
#include <unordered_map>
#include <functional>

SignalPathManager::SignalPathManager() : currentSpec()
{
    // empty constructor
}

SignalPathManager::~SignalPathManager()
{
    // empty destructor
}

void SignalPathManager::prepare(const juce::dsp::ProcessSpec& spec)
{
    // store the spec for later use
    currentSpec = spec;

    // initialize the processor chain
    if (!processorChain)
    {
        processorChain = std::make_unique<MainChainType>();
        // initializeProcessorChain();
    }

    // make sure all processors in the chain are properly prepared
    processorChain->prepare(spec);

    // set up active processors based on current mode
    updateActiveProcessors();
}

void SignalPathManager::reset()
{
    // reset the main processor chain if it exists
    if (processorChain)
        processorChain->reset();

    // TODO: PROCESSOR_ADDITION_CHAIN(13): add reset logic for any new chains
    //       here. remember that we are using one main chain to handle all of
    //       the processors, so calling reset on the main chain should be fine
    //       unless you're adding a whole new chain that needs separate logic
    //       to reset it.
}

void SignalPathManager::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (processorChain)
    {
        // TODO: PROCESSOR_ADDITION_CHAIN(21): add a new bypass handler for the
        //       new processor here
        processorChain->setBypassed<looper>(!looperActive);
        processorChain->setBypassed<delay>(!delayActive);
        processorChain->setBypassed<granular>(!granularActive);
        processorChain->setBypassed<reverb>(!reverbActive);

        processorChain->process(context);
    }
    else
        jassertfalse; // no processor chain initialized, should never happen
}

void SignalPathManager::setProcessingMode(ProcessingMode newMode)
{
    if (currentMode == newMode)
        return; // avoid reinitializing if the mode hasn't changed

    currentMode = newMode;

    // update which processors are active based on the new mode
    updateActiveProcessors();
}

// TODO: PROCESSOR_ADDITION_CHAIN(14): add new processor *chain* getters here
// helper methods to get processor references from the main chain
DelayProcessor& SignalPathManager::getDelayFromChain()
{
    jassert(processorChain != nullptr);
    return processorChain->get<delay>();
}

ReverbProcessor& SignalPathManager::getReverbFromChain()
{
    jassert(processorChain != nullptr);
    return processorChain->get<reverb>();
}

GranularProcessor& SignalPathManager::getGranularFromChain()
{
    jassert(processorChain != nullptr);
    return processorChain->get<granular>();
}

LooperProcessor& SignalPathManager::getLooperFromChain()
{
    jassert(processorChain != nullptr);
    return processorChain->get<looper>();
}

// TODO: PROCESSOR_ADDITION_CHAIN(20): add new *direct* processor getters here
DelayProcessor* SignalPathManager::getDelayProcessor()
{
    if (processorChain && delayActive)
        return &getDelayFromChain();
    return nullptr;
}

ReverbProcessor* SignalPathManager::getReverbProcessor()
{
    if (processorChain && reverbActive)
        return &getReverbFromChain();
    return nullptr;
}

GranularProcessor* SignalPathManager::getGranularProcessor()
{
    if (processorChain && granularActive)
        return &getGranularFromChain();
    return nullptr;
}

LooperProcessor* SignalPathManager::getLooperProcessor()
{
    if (processorChain && looperActive)
        return &getLooperFromChain();
    return nullptr;
}


void SignalPathManager::initializeProcessorChain()
{
    jassert(currentSpec.sampleRate >= 0); // Ensure the sample rate is valid
    if (currentSpec.sampleRate <= 0)
        return; // Exit if the sample rate is invalid

    // // Initialize all processors in the chain
    // getLooperFromChain().prepare(currentSpec);
    // getDelayFromChain().prepare(currentSpec);
    // getGranularFromChain().prepare(currentSpec);
    // getReverbFromChain().prepare(currentSpec);

    // initialize the entire processorChain
    if (processorChain)
        processorChain->prepare(currentSpec);

    // TODO: PROCESSOR_ADDITION_CHAIN(12): add the processor to the initialization logic
}

void SignalPathManager::updateActiveProcessors()
{
    // TODO: PROCESSOR_ADDITION_CHAIN(22): set up a new processor active flag,
    //       and add it to the switch statement below + default to false
    // reset all flags to inactive
    looperActive = false;
    delayActive = false;
    granularActive = false;
    reverbActive = false;

    // set active flags based on current mode
    switch (currentMode)
    {
        case DelayOnly:
            delayActive = true;
            break;

        case ReverbOnly:
            reverbActive = true;
            break;

        case GranularOnly:
            granularActive = true;
            break;

        case LooperOnly:
            looperActive = true;
            break;

        case Serial:
            // all processors are active in serial mode, except for the standard
            // delay, because it's not necessary with the granular delay
            // processor
            looperActive = true;
            delayActive = false;
            granularActive = true;
            reverbActive = true;
            break;

        default:
            // use DelayOnly as a fallback
            delayActive = true;
            jassertfalse; // unexpected mode, should never happen
            break;
    }

    // TODO: PROCESSOR_ADDITION_CHAIN(15): add new processor active flags here
}

void SignalPathManager::updateProcessorChainParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    if (!processorChain)
        return;

    // only update parameters for active processors to avoid unnecessary processing
    if (looperActive)
        getLooperFromChain().updateParameters(apvts);

    if (delayActive)
        getDelayFromChain().updateParameters(apvts);

    if (granularActive)
        getGranularFromChain().updateParameters(apvts);

    if (reverbActive)
        getReverbFromChain().updateParameters(apvts);

    // TODO: PROCESSOR_ADDITION_CHAIN(?): add parameter updates for new processors here
}

