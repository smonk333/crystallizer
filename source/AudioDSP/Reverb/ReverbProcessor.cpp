//
// Created by smoke on 5/30/2025.
//

#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor()
{
    // empty constructor
}

ReverbProcessor::~ReverbProcessor()
{
    // empty destructor
}

void ReverbProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // prepare the reverb processor
    reverb.prepare(spec);
    reverb.reset();

    // prepare low-pass filter for damping enhancement
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    // set initial filter coefficients (12khz cutoff)
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        spec.sampleRate, 12000.0f, 0.707f);
}

void ReverbProcessor::reset()
{
    reverb.reset();
    lowPassFilter.reset();
}

// Required implementation for ProcessorBase inheritance
void ReverbProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    // make sure we have right number of channels
    jassert (inputBlock.getNumChannels() >= 1);
    jassert (outputBlock.getNumChannels() >= 1);

    // create an AudioBlock to work with
    juce::dsp::AudioBlock<float> block (outputBlock);

    // copy input to output if they're not already the same
    if (!context.isBypassed)
    {
        if (context.usesSeparateInputAndOutputBlocks())
            block.copyFrom (inputBlock);

        // store clean signal for dry/wet mixing
        juce::HeapBlock<float> cleanSignalL (block.getNumSamples());
        juce::HeapBlock<float> cleanSignalR (block.getNumSamples());

        for (int i = 0; i < block.getNumSamples(); ++i)
        {
            cleanSignalL[i] = block.getSample (0, i);
            cleanSignalR[i] = block.getNumChannels() > 1 ? block.getSample (1, i) : block.getSample (0, i);
        }

        // apply reverb parameters
        reverbParams.roomSize = currentRoomSize;
        reverbParams.damping = currentDamping;
        reverbParams.wetLevel = currentWetLevel;
        reverbParams.dryLevel = currentDryLevel;
        reverbParams.width = currentWidth;
        reverbParams.freezeMode = currentFreezeMode > 0.5f;
        reverb.setParameters (reverbParams);

        // process through reverb
        juce::dsp::ProcessContextReplacing<float> reverbContext (block);
        reverb.process (reverbContext);

        // apply low-pass filter for damping enhancement
        *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (
            sampleRate, 12000.0f, 0.707f);
        lowPassFilter.process (reverbContext);
    }
}

void ReverbProcessor::setRoomSize (float newRoomSize)
{
    currentRoomSize = newRoomSize;
}

void ReverbProcessor::setDamping (float newDamping)
{
    currentDamping = newDamping;
}

void ReverbProcessor::setWetLevel (float newWetLevel)
{
    currentWetLevel = newWetLevel;
}

void ReverbProcessor::setDryLevel (float newDryLevel)
{
    currentDryLevel = newDryLevel;
}

void ReverbProcessor::setWidth (float newWidth)
{
    currentWidth = newWidth;
}

void ReverbProcessor::setFreezeMode (float newFreezeMode)
{
    currentFreezeMode = newFreezeMode;
}

// legacy parameter update method
// void ReverbProcessor::updateParameters (float roomSize, float damping, float wet, float dry, float width, float freeze)
// {
//     setRoomSize (roomSize);
//     setDamping (damping);
//     setWetLevel (wet);
//     setDryLevel (dry);
//     setWidth (width);
//     setFreezeMode (freeze);
// }

void ReverbProcessor::updateParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    // parameter retrieval from APVTS
    auto roomSizeParam = apvts.getRawParameterValue("reverbRoomSize");
    auto dampingParam = apvts.getRawParameterValue("reverbDamping");
    auto mixParam = apvts.getRawParameterValue("reverbMix"); // Combined wet/dry mix
    auto widthParam = apvts.getRawParameterValue("reverbWidth");
    auto freezeModeParam = apvts.getRawParameterValue("reverbFreeze");

    if (roomSizeParam) setRoomSize(*roomSizeParam);
    if (dampingParam) setDamping(*dampingParam);
    if (mixParam)
    {
        setWetLevel(*mixParam); // Use reverbMix for wet level
        setDryLevel(1.0f - *mixParam); // Complement for dry level
    }
    if (widthParam) setWidth(*widthParam);
    if (freezeModeParam) setFreezeMode(*freezeModeParam);
}
