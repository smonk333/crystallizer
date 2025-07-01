//
// Created by smoke on 6/6/2025.
//

#include "SignalPathManager.h"

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
    //=error handling===========================================================
    jassert(spec.sampleRate > 0.0);
    jassert(spec.maximumBlockSize > 0.0);
    jassert(spec.numChannels > 0 && spec.numChannels <= 8);

    if (spec.sampleRate <= 0.0 || spec.maximumBlockSize <= 0.0 || spec.numChannels <= 0)
    {
        throw std::invalid_argument("Invalid ProcessSpec in SignalPathManager");
    }
    //=end error handling=======================================================

    // store the spec for later use
    currentSpec = spec;

    // initialize the processor chain
    if (!processorChain)
    {
        processorChain = std::make_unique<MainChainType>();
        initializeProcessorChain();
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
    //=error handling: context validation=======================================
    auto& outputBlock = context.getOutputBlock();
    if (outputBlock.getNumChannels() == 0 || outputBlock.getNumSamples() == 0)
    {
        jassertfalse; // output block is invalid
        return;
    }

    // validate against prepared specs
    if (outputBlock.getNumSamples() > currentSpec.maximumBlockSize)
    {
        jassertfalse;
        DBG("Process block size: " << outputBlock.getNumSamples() << " exceeds maximum: " << currentSpec.maximumBlockSize);
        return;
    }
    //=end error handling=======================================================

    if (processorChain)
    {
        //=wrap in try-catch to ensure the processorChain is initialized========
        try
        {
            // apply bypass states for all processors using our helper function
            setProcessorBypassStates(*processorChain);

            processorChain->process(context);
        }

        catch (const std::exception& e)
        {
            DBG("Error in signal processing: " << e.what());
            outputBlock.clear(); //safe fallback
        }
    }
    else
        jassertfalse; // no processor chain initialized, should never happen
}

void SignalPathManager::setProcessingMode(ProcessingMode newMode)
{
    // avoid reinitializing if the mode hasn't changed
    if (currentMode == newMode)
        return;

    currentMode = newMode;

    // update which processors are active based on the new mode
    updateActiveProcessors();
}

// TODO: PROCESSOR_ADDITION_CHAIN(14): add new processor *chain* getters here
// helper methods to get processor references from the main chain
// Template-based processor getter usage examples
DelayProcessor& SignalPathManager::getDelayFromChain()
{
    return getProcessorFromChain<DelayProcessor, delay>();
}

ReverbProcessor& SignalPathManager::getReverbFromChain()
{
    return getProcessorFromChain<ReverbProcessor, reverb>();
}

GranularProcessor& SignalPathManager::getGranularFromChain()
{
    return getProcessorFromChain<GranularProcessor, granular>();
}

LooperProcessor& SignalPathManager::getLooperFromChain()
{
    return getProcessorFromChain<LooperProcessor, looper>();
}

// TODO: PROCESSOR_ADDITION_CHAIN(20): add new *direct* processor getters here
DelayProcessor* SignalPathManager::getDelayProcessor()
{
    if (processorChain && processorActiveStates[delay])
        return &getDelayFromChain();
    return nullptr;
}

ReverbProcessor* SignalPathManager::getReverbProcessor()
{
    if (processorChain && processorActiveStates[reverb])
        return &getReverbFromChain();
    return nullptr;
}

GranularProcessor* SignalPathManager::getGranularProcessor()
{
    if (processorChain && processorActiveStates[granular])
        return &getGranularFromChain();
    return nullptr;
}

LooperProcessor* SignalPathManager::getLooperProcessor()
{
    if (processorChain && processorActiveStates[looper])
        return &getLooperFromChain();
    return nullptr;
}


void SignalPathManager::initializeProcessorChain()
{
    jassert(currentSpec.sampleRate >= 0); // Ensure the sample rate is valid
    if (currentSpec.sampleRate <= 0)
        return; // Exit if the sample rate is invalid

    // initialize the entire processorChain
    if (processorChain)
        processorChain->prepare(currentSpec);

    // TODO: PROCESSOR_ADDITION_CHAIN(12): add the processor to the initialization logic
}

void SignalPathManager::updateActiveProcessors()
{
    // TODO: PROCESSOR_ADDITION_CHAIN(22): set up a new processor active flag,
    //       and add it to the switch statement below + default to false

    // reset all processor states to inactive
    for (auto& [processor, active] : processorActiveStates) {
        active = false;
    }

    // set active flags based on current mode
    switch (currentMode)
    {
        case DelayOnly:
            processorActiveStates[delay] = true;
            break;

        case ReverbOnly:
            processorActiveStates[reverb] = true;
            break;

        case GranularOnly:
            processorActiveStates[granular] = true;
            break;

        case LooperOnly:
            processorActiveStates[looper] = true;
            break;

        case Serial:
            // all processors are active in serial mode, except for the standard
            // delay, because it's not necessary with the granular delay
            // processor
            // TODO: PROCESSOR_ADDITION_CHAIN(24): add new processor active flags here
            processorActiveStates[looper] = true;
            processorActiveStates[delay] = false;
            processorActiveStates[granular] = true;
            processorActiveStates[reverb] = true;
            break;

        default:
            // use DelayOnly as a fallback
            processorActiveStates[delay] = true;
            jassertfalse; // unexpected mode, should never happen
            break;
    }

    // TODO: PROCESSOR_ADDITION_CHAIN(15): add new processor active flags here
}

void SignalPathManager::updateProcessorChainParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    if (auto* delay = getDelayProcessor()) {
        DelayProcessor::DelayParams params;
        if (auto* v = apvts.getRawParameterValue("delayTime")) params.delayTime = *v;
        if (auto* v = apvts.getRawParameterValue("feedback")) params.feedback = *v;
        if (auto* v = apvts.getRawParameterValue("wetDry")) params.wetLevel = *v;
        delay->updateParameters(params);
    }
    if (auto* reverb = getReverbProcessor()) {
        ReverbProcessor::ReverbParams params;
        if (auto* v = apvts.getRawParameterValue("reverbRoomSize")) params.roomSize = *v;
        if (auto* v = apvts.getRawParameterValue("reverbDamping")) params.damping = *v;
        if (auto* v = apvts.getRawParameterValue("reverbMix")) {
            params.wetLevel = *v;
            params.dryLevel = 1.0f - *v;
        }
        if (auto* v = apvts.getRawParameterValue("reverbWidth")) params.width = *v;
        if (auto* v = apvts.getRawParameterValue("reverbFreeze")) params.freezeMode = *v;
        reverb->updateParameters(params);
    }
    if (auto* granular = getGranularProcessor()) {
        GranularProcessor::GranularParams params;
        if (auto* v = apvts.getRawParameterValue("granularDelayTime")) params.delayTime = *v;
        if (auto* v = apvts.getRawParameterValue("grainSize")) params.grainSize = *v;
        if (auto* v = apvts.getRawParameterValue("grainDensity")) params.grainDensity = *v;
        if (auto* v = apvts.getRawParameterValue("pitchShift")) params.pitchShift = *v;
        if (auto* v = apvts.getRawParameterValue("granularFeedback")) params.feedback = *v;
        if (auto* v = apvts.getRawParameterValue("granularWetDry")) params.wetDryMix = *v;
        if (auto* v = apvts.getRawParameterValue("spread")) params.spread = *v;
        granular->updateParameters(params);
    }
    if (auto* looper = getLooperProcessor()) {
        LooperProcessor::LooperParams params;
        // read the looperState parameter
        if (auto* v = apvts.getRawParameterValue("looperState"))
            params.looperState = static_cast<int>(*v);
        looper->updateParameters(params);
    }
    // TODO: PROCESSOR_ADDITION_CHAIN(?): Add similar blocks for other processor parameters here
}

