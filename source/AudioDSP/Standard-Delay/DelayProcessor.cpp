//
// Created by smoke on 5/30/2025.
//

#include "DelayProcessor.h"

DelayProcessor::DelayProcessor()
{
    // empty constructor
}

DelayProcessor::~DelayProcessor()
{
    // empty destructor
}

void DelayProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    // calculate the maximum delay in samples for 60 seconds
    const int maxDelaySamples = static_cast<int>(spec.sampleRate * 60.0);

    // prepare the left DelayLine with the sampleRate and maximum delay time
    leftDelay.prepare(spec);
    leftDelay.setMaximumDelayInSamples(maxDelaySamples);
    leftDelay.reset();

    // prepare the right DelayLine with the sampleRate and maximum delay time
    rightDelay.prepare(spec);
    rightDelay.setMaximumDelayInSamples(maxDelaySamples);
    rightDelay.reset();

    // store sample rate for delay time calculations
    currentSampleRate = spec.sampleRate;

    // update delay time based on current sample rate
    updateParameters(delayParams);
}

void DelayProcessor::reset()
{
    // make sure the channels are both empty
    leftDelay.reset();
    rightDelay.reset();
}

void DelayProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    // TODO: PROCESSOR_ADDITION_CHAIN(EXAMPLE): make sure you wrap the processing
    //       in a check to see if the processor is bypassed, as shown below
    if (!context.isBypassed)
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        // handle mono case by duplicating single channel
        const auto numChannels = juce::jmin(
            inputBlock.getNumChannels(),
            outputBlock.getNumChannels(),
            static_cast<size_t> (2)
        );

        // process samples using our current parameter values
        for (int i = 0; i < static_cast<int>(inputBlock.getNumSamples()); ++i)
        {
            float inputL = inputBlock.getSample(0, i);
            float inputR = numChannels > 1 ? inputBlock.getSample(1, i) : inputL;

            // store clean signal temporarily for wet/dry mixing
            auto cleanL = inputL;
            auto cleanR = inputR;

            // process through delay lines
            auto delayedL = leftDelay.popSample(0);
            auto delayedR = rightDelay.popSample(0);

            leftDelay.pushSample(0, inputL + (delayedL * delayParams.feedback));
            rightDelay.pushSample(0, inputR + (delayedR * delayParams.feedback));

            // mix clean and wet signals
            outputBlock.setSample(0, i, (delayedL * delayParams.wetLevel) + (cleanL * (1.0f - delayParams.wetLevel)));
            if (numChannels > 1)
                outputBlock.setSample(1, i, (delayedR * delayParams.wetLevel) + (cleanR * (1.0f - delayParams.wetLevel)));
        }
    }
}

void DelayProcessor::updateParameters(const DelayParams& params)
{
    delayParams = params;

    // update delay time in samples for both delay lines
    auto delayInSamples = static_cast<float>(delayParams.delayTime * currentSampleRate);
    leftDelay.setDelay(delayInSamples);
    rightDelay.setDelay(delayInSamples);
}
