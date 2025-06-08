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
    auto delayInSamples = static_cast<float>(currentDelayTime * currentSampleRate);

    // Get the maximum delay in samples from the delay lines
    auto maxDelaySamples = static_cast<float>(leftDelay.getMaximumDelayInSamples());

    // Ensure delay doesn't exceed maximum
    delayInSamples = juce::jmin(delayInSamples, maxDelaySamples);

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

// legacy parameter update method
// void DelayProcessor::updateParameters(float time, float feedback, float mix)
// {
//     setDelayTime(time);
//     setFeedback(feedback);
//     setWetLevel(mix);
// }

void DelayProcessor::updateParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    // parameter retrieval from APVTS
    auto delayTimeParam = apvts.getRawParameterValue("delayTime");
    auto feedbackParam = apvts.getRawParameterValue("feedback");
    auto wetLevelParam = apvts.getRawParameterValue("wetLevel");

    if (delayTimeParam) setDelayTime(*delayTimeParam);
    if (feedbackParam) setFeedback(*feedbackParam);
    if (wetLevelParam) setWetLevel(*wetLevelParam);
}


