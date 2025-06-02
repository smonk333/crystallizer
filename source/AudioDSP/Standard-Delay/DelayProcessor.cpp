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
    const size_t maxDelaySamples = static_cast<size_t>(spec.sampleRate * 60.0);

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
}

void DelayProcessor::reset()
{
    // make sure the channels are both empty
    leftDelay.reset();
    rightDelay.reset();
}

void DelayProcessor::process (juce::AudioBuffer<float>& buffer,
    float* cleanSignalL, float* cleanSignalR, float delayTime,
    float feedbackL, float feedbackR, float wetDryMix)
{
    // set the delay length for both channels
    leftDelay.setDelay(delayTime);
    rightDelay.setDelay(delayTime);

    // grab a write pointer for the two channels in the input buffer
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    // for every sample in the buffer,
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // ... pop the delayed samples from both channels
        float delayedL = leftDelay.popSample(0);
        float delayedR = rightDelay.popSample(1);

        // ... write the delayed samples back into the delay lines, adding the
        // feedback
        leftDelay.pushSample(0, channelDataL[sample] + feedbackL * delayedL);
        rightDelay.pushSample(1, channelDataR[sample] + feedbackR * delayedR);

        // ... apply wet/dry mix to produce the final output, where the delay is
        // mixed with the original signal
        channelDataL[sample] = ((delayedL * wetDryMix) + (cleanSignalL[sample] * (1 - wetDryMix)));
        channelDataR[sample] = ((delayedR * wetDryMix) + (cleanSignalR[sample] * (1 - wetDryMix)));
    }
}

