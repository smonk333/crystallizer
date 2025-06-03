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
    // Calculate the maximum delay in samples for 60 seconds
    const int maxDelaySamples = static_cast<int>(spec.sampleRate * 60.0);

    // Prepare the left DelayLine with the sampleRate and maximum delay time
    leftDelay.prepare(spec);
    leftDelay.setMaximumDelayInSamples(maxDelaySamples);
    leftDelay.reset();

    // Prepare the right DelayLine with the sampleRate and maximum delay time
    rightDelay.prepare(spec);
    rightDelay.setMaximumDelayInSamples(maxDelaySamples);
    rightDelay.reset();

    // Store sample rate for delay time calculations
    currentSampleRate = spec.sampleRate;

    // Update delay time based on current sample rate
    setDelayTime(currentDelayTime);
}

void DelayProcessor::reset()
{
    // make sure the channels are both empty
    leftDelay.reset();
    rightDelay.reset();
}

void DelayProcessor::process(juce::AudioBuffer<float>& buffer,
    float* cleanSignalL, float* cleanSignalR, float delayTime,
    float feedbackL, float feedbackR, float wetDryMix)
{
    // Update parameters
    setDelayTime(delayTime);
    setFeedback((feedbackL + feedbackR) * 0.5f);
    setWetLevel(wetDryMix);

    // grab a write pointer for the two channels in the input buffer
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    // for every sample in the buffer,
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // Store clean signal for wet/dry mixing if pointers are provided
        if (cleanSignalL != nullptr)
            cleanSignalL[sample] = channelDataL[sample];
        if (cleanSignalR != nullptr)
            cleanSignalR[sample] = channelDataR[sample];

        // ... pop the delayed samples from both channels
        float delayedL = leftDelay.popSample(0);
        float delayedR = rightDelay.popSample(0);

        // ... write the delayed samples back into the delay lines, adding the feedback
        leftDelay.pushSample(0, channelDataL[sample] + (delayedL * currentFeedback));
        rightDelay.pushSample(0, channelDataR[sample] + (delayedR * currentFeedback));

        // ... apply wet/dry mix to produce the final output, where the delay is
        // mixed with the original signal
        channelDataL[sample] = (delayedL * currentWetLevel) + (channelDataL[sample] * (1.0f - currentWetLevel));
        channelDataR[sample] = (delayedR * currentWetLevel) + (channelDataR[sample] * (1.0f - currentWetLevel));
    }
}

void DelayProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    // Handle mono case by duplicating single channel
    const auto numChannels = juce::jmin(inputBlock.getNumChannels(), outputBlock.getNumChannels(), size_t(2));

    // Process samples using our current parameter values
    for (int i = 0; i < static_cast<int>(inputBlock.getNumSamples()); ++i)
    {
        float inputL = inputBlock.getSample(0, i);
        float inputR = numChannels > 1 ? inputBlock.getSample(1, i) : inputL;

        // Store clean signal temporarily for wet/dry mixing
        auto cleanL = inputL;
        auto cleanR = inputR;

        // Process through delay lines
        auto delayedL = leftDelay.popSample(0);
        auto delayedR = rightDelay.popSample(0);

        leftDelay.pushSample(0, inputL + (delayedL * currentFeedback));
        rightDelay.pushSample(0, inputR + (delayedR * currentFeedback));

        // Mix clean and wet signals
        outputBlock.setSample(0, i, (delayedL * currentWetLevel) + (cleanL * (1.0f - currentWetLevel)));
        if (numChannels > 1)
            outputBlock.setSample(1, i, (delayedR * currentWetLevel) + (cleanR * (1.0f - currentWetLevel)));
    }
}

void DelayProcessor::setDelayTime(float newDelayTime)
{
    currentDelayTime = newDelayTime;
    auto delayInSamples = currentDelayTime * currentSampleRate;
    leftDelay.setDelay(delayInSamples);
    rightDelay.setDelay(delayInSamples);
}

void DelayProcessor::setFeedback(float newFeedback)
{
    currentFeedback = newFeedback;
}

void DelayProcessor::setWetLevel(float newWetLevel)
{
    currentWetLevel = newWetLevel;
}

void DelayProcessor::updateParameters(float time, float feedback, float mix)
{
    setDelayTime(time);
    setFeedback(feedback);
    setWetLevel(mix);
}
