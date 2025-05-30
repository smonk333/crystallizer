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

    void process(
        juce::AudioBuffer<float>& buffer,
        float* cleanSignalL,
        float* cleanSignalR,
        float delayTime,
        float feedbackL,
        float feedbackR,
        float wetDryMix,
        float output);

private:
    juce::dsp::DelayLine<float> leftDelay {410000};
    juce::dsp::DelayLine<float> rightDelay {410000};
};



#endif //DELAYPROCESSOR_H
