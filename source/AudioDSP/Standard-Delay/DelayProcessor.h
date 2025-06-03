//
// Created by smoke on 5/30/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

#ifndef DELAYPROCESSOR_H
#define DELAYPROCESSOR_H

class DelayProcessor : public juce::dsp::ProcessorBase {
public:
    DelayProcessor();
    ~DelayProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;

    // Required implementation of ProcessorBase::process
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // Parameter setters for ProcessorChain use
    void setDelayTime(float newDelayTime);
    void setFeedback(float newFeedback);
    void setWetLevel(float newWetLevel);
    void updateParameters(float time, float feedback, float mix);

private:
    juce::dsp::DelayLine<float> leftDelay { 410000 };
    juce::dsp::DelayLine<float> rightDelay { 410000 };

    // Parameters for ProcessorChain compatibility
    float currentDelayTime = 0.5f;
    float currentFeedback = 0.5f;
    float currentWetLevel = 0.5f;
    double currentSampleRate = 44100.0;
};

#endif //DELAYPROCESSOR_H
