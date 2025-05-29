//
// Created by smoke on 5/29/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

#ifndef DELAYPROCESSOR_H
#define DELAYPROCESSOR_H

class DelayProcessor
{
public:
    void prepare(double sr, int maxDelayInSamples);  // Renamed parameter
    void reset();
    void setParameters(float delaySeconds, float fb, float wdMix);

    void process(juce::AudioBuffer<float>& buffer);

private:
    // Sample rate as int to match usage
    int sampleRate = 44100;

    // delay parameters
    float delayTimeSeconds = 0.5f;          // default 500ms
    float feedback = 0.5f;                  // default 50% feedback
    float wetDryMix = 0.5f;                 // default 50% wet/dry mix

    // Delay lines initialized with default size
    juce::dsp::DelayLine<float> delayL { sampleRate };
    juce::dsp::DelayLine<float> delayR { sampleRate };
};

#endif //DELAYPROCESSOR_H