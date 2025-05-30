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

void DelayProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    // prepare the left DelayLine with the sampleRate and maximum delay time
    leftDelay.prepare(spec);
    leftDelay.reset();

    // prepare the right DelayLine with the sampleRate and maximum delay time
    rightDelay.prepare(spec);
    rightDelay.reset();
}

void DelayProcessor::reset()
{
    leftDelay.reset();
    rightDelay.reset();
}

void DelayProcessor::process (juce::AudioBuffer<float>& buffer,
    float* cleanSignalL, float* cleanSignalR, float delayTime,
    float feedbackL, float feedbackR, float wetDryMix, float output)
{
    leftDelay.setDelay(delayTime);
    rightDelay.setDelay(delayTime);

    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float delayedL = leftDelay.popSample(0);
        float delayedR = rightDelay.popSample(1);

        leftDelay.pushSample(0, channelDataL[sample] + feedbackL * delayedL);
        rightDelay.pushSample(1, channelDataR[sample] + feedbackR * delayedR);

        channelDataL[sample] = ((delayedL * wetDryMix) + (cleanSignalL[sample] * (1 - wetDryMix))) * juce::Decibels::decibelsToGain(output);
        channelDataR[sample] = ((delayedR * wetDryMix) + (cleanSignalR[sample] * (1 - wetDryMix))) * juce::Decibels::decibelsToGain(output);
    }
}