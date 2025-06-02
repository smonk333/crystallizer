//
// Created by smoke on 5/30/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

#ifndef DELAYPROCESSOR_H
#define DELAYPROCESSOR_H

class DelayProcessor {
public:
    DelayProcessor();
    ~DelayProcessor();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    // original process method (keep for backward compatibility)
    void process(
        juce::AudioBuffer<float>& buffer,
        float* cleanSignalL,
        float* cleanSignalR,
        float delayTime,
        float feedbackL,
        float feedbackR,
        float wetDryMix);

    // process method for ProcessorChain compatibility
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        // make sure we have right number of channels
        jassert(inputBlock.getNumChannels() == 2);
        jassert(outputBlock.getNumChannels() == 2);

        // process samples using our current parameter values
        for (size_t i = 0; i < inputBlock.getNumSamples(); ++i)
        {
            auto inputL = inputBlock.getSample(0, i);
            auto inputR = inputBlock.getSample(1, i);

            // store clean signal temporarily for wet/dry mixing
            auto cleanL = inputL;
            auto cleanR = inputR;

            // process through delay lines
            auto delayedL = leftDelay.popSample(0);
            auto delayedR = rightDelay.popSample(0);

            leftDelay.pushSample(0, inputL + (delayedL * currentFeedback));
            rightDelay.pushSample(0, inputR + (delayedR * currentFeedback));

            // mix clean and wet signals
            outputBlock.setSample(0, i, (delayedL * currentWetLevel) + (cleanL * (1.0f - currentWetLevel)));
            outputBlock.setSample(1, i, (delayedR * currentWetLevel) + (cleanR * (1.0f - currentWetLevel)));
        }
    }

    // parameter setters for ProcessorChain use - inline for better performance
    void setDelayTime(float newDelayTime)
    {
        currentDelayTime = newDelayTime;
        auto delayInSamples = currentDelayTime * currentSampleRate;
        leftDelay.setDelay(delayInSamples);
        rightDelay.setDelay(delayInSamples);
    }

    void setFeedback(float newFeedback) { currentFeedback = newFeedback; }
    void setWetLevel(float newWetLevel) { currentWetLevel = newWetLevel; }

    // unified method to update all parameters at once
    void updateParameters(float time, float feedback, float mix)
    {
        setDelayTime(time);
        setFeedback(feedback);
        setWetLevel(mix);
    }

private:
    juce::dsp::DelayLine<float> leftDelay { 410000 };
    juce::dsp::DelayLine<float> rightDelay { 410000 };

    // parameters for ProcessorChain compatibility
    float currentDelayTime = 0.5f;
    float currentFeedback = 0.5f;
    float currentWetLevel = 0.5f;
    double currentSampleRate = 44100.0;
};

#endif //DELAYPROCESSOR_H
