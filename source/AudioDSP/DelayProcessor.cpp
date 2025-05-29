//
// Created by smoke on 5/29/2025.
//

#include "DelayProcessor.h"
#include "../debug/stacktrace.h"

void DelayProcessor::prepare (double sr, int maxDelaySamples)
{
    sampleRate = sr;

    delayL.setMaximumDelayInSamples (maxDelaySamples);
    delayR.setMaximumDelayInSamples (maxDelaySamples);
    printStackTrace();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2; // stereo

    reset();
}

void DelayProcessor::reset()
{
    delayL.reset();
    delayR.reset();
}

void DelayProcessor::setParameters (float delaySeconds, float fb, float wdMix)
{
    delayTimeSeconds = delaySeconds;
    feedback = juce::jlimit(0.0f, 1.0f, fb);
    wetDryMix = juce::jlimit(0.0f, 1.0f, wdMix);
}

void DelayProcessor::process (juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int delaySamples = static_cast<int>(sampleRate * delayTimeSeconds);;

    auto* channelL = buffer.getWritePointer(0);
    auto* channelR = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float inL = channelL[sample];
        float inR = channelR[sample];

        float delayedL = delayL.popSample(0, delaySamples);
        float delayedR = delayR.popSample(0, delaySamples);

        delayL.pushSample(0, inL + delayedL * feedback);
        delayR.pushSample(0, inR + delayedR * feedback);

        channelL[sample] = inL * (1.0f - wetDryMix) + delayedL * wetDryMix;
        channelR[sample] = inR * (1.0f - wetDryMix) + delayedR * wetDryMix;
    }
}