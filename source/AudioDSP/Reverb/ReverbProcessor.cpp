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

// required implementation for ProcessorBase inheritance
void ReverbProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (context.getOutputBlock().getNumChannels() != 0)
    {
        DBG("Signal has hit the ReverbProcessor!");
    }
    if (!context.isBypassed)
    {
        DBG("Signal was not bypassed at the ReverbProcessor");
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        // make sure we have right number of channels
        jassert (inputBlock.getNumChannels() >= 1);
        jassert (outputBlock.getNumChannels() >= 1);

        // // create an AudioBlock to work with
        // juce::dsp::AudioBlock<float> block (outputBlock);

        // // copy input to output if they're not already the same
        // if (context.usesSeparateInputAndOutputBlocks())
        //     outputBlock.copyFrom (inputBlock);

        // store clean signal for dry/wet mixing
        juce::HeapBlock<float> cleanSignalL (outputBlock.getNumSamples());
        juce::HeapBlock<float> cleanSignalR (outputBlock.getNumSamples());

        for (int i = 0; i < outputBlock.getNumSamples(); ++i)
        {
            cleanSignalL[i] = outputBlock.getSample (0, i);
            cleanSignalR[i] = outputBlock.getNumChannels() > 1 ? outputBlock.getSample (1, i) : outputBlock.getSample (0, i);
        }

        reverb.setParameters(reverbParams);

        // process through reverb
        juce::dsp::ProcessContextReplacing<float> reverbContext (outputBlock);
        reverb.process(reverbContext);

        // apply low-pass filter for damping enhancement
        *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (
            sampleRate, 12000.0f, 0.707f);
        lowPassFilter.process(reverbContext);
    } else DBG("Signal was bypassed at the ReverbProcessor");
}

void ReverbProcessor::updateParameters(const ReverbParams& params)
{
    reverbParams = {
        params.roomSize,
        params.damping,
        params.wetLevel,
        params.dryLevel,
        params.width,
        static_cast<float>(params.freezeMode > 0.5f)
    };
}