//
// Created by smoke on 5/30/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

#ifndef DELAYPROCESSOR_H
#define DELAYPROCESSOR_H

class DelayProcessor : public juce::dsp::ProcessorBase {
public:
    // TDOD: PROCESSOR_ADDITION_CHAIN(EXAMPLE): Define a struct for parameters
    struct DelayParams {
        float delayTime = 0.5f;
        float feedback = 0.5f;
        float wetLevel = 0.5f;
    };

    DelayProcessor();
    ~DelayProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;

    // required implementation of ProcessorBase::process
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // Set all parameters at once using a struct of raw types
    void updateParameters(const DelayParams& params);

    // // Optionally, keep the individual setters for flexibility
    // void setDelayTime(float newDelayTime);
    // void setFeedback(float newFeedback);
    // void setWetLevel(float newWetLevel);

private:
    juce::dsp::DelayLine<float> leftDelay;
    juce::dsp::DelayLine<float> rightDelay;

    // set up parameters for the delay processor as a struct
    DelayParams delayParams = {0.5f, 0.5f, 0.5f};

    // float currentDelayTime = 0.5f;
    // float currentFeedback = 0.5f;
    // float currentWetLevel = 0.5f;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayProcessor)
};

#endif //DELAYPROCESSOR_H
