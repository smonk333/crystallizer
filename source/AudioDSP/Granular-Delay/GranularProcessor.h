//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef GRANULARPROCESSOR_H
#define GRANULARPROCESSOR_H

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <random>

class GranularProcessor : public juce::dsp::ProcessorBase
{
public:
    struct GranularParams {
        float delayTime = 0.5f;
        float grainSize = 0.1f;
        float grainDensity = 0.5f;
        float pitchShift = 0.0f;
        float feedback = 0.5f;
        float wetDryMix = 0.5f;
        float spread = 0.0f;
    };

    GranularProcessor();
    ~GranularProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;


    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // // parameter setters for ProcessorChain use
    // void setDelayTime(float newDelayTime);
    // void setGrainSize(float newGrainSize);
    // void setGrainDensity(float newDensity);
    // void setPitchShift(float newPitchShift);
    // void setFeedback(float newFeedback);
    // void setWetDryMix(float newWetDryMix);
    // void setSpread(float newSpread);

    void updateParameters(const GranularParams& params);


private:

    // grain structure for holding individual grain parameters
    struct Grain
    {
        float delayPosition = 0.0f;     // position in the delay buffer
        float readPosition = 0.0f;      // current read position (for pitch shift)
        float grainAmplitude = 0.0f;    // amplitude of the grain
        float pitchRatio = 1.0f;        // pitch ratio for grain playback
        int currentSample = 0;          // current sample index in the grain
        int totalSamples = 0;           // total grain length in samples
        bool active = false;            // whether this grain is currently active
    };

    // // initialize parameters to a default value
    // float currentDelayTime = 0.5f;      // delay time in seconds
    // float currentGrainSize = 0.5f;      // grain size in seconds
    // float currentGrainDensity = 0.5f;   // grain density (grains per second)
    // float currentPitchShift = 0.5f;     // pitch shift ratio (1.0 = no shift)
    // float currentFeedback = 0.5f;       // feedback level (0.0 to 1.0)
    // float currentWetDryMix = 0.5f;      // wet/dry mix ratio (0.0 to 1.0)
    // float currentSpread = 0.5f;         // random position spread

    // store the current parameters in a struct
    GranularParams granularParams = {0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f,
        0.5f};

    // store the sample rate, which is set during prepare
    double sampleRate = 44100.0;

    // calculate the maximum number of samples for the delay buffer
    int maxDelaySamples = static_cast<int>(sampleRate * 5.0); // 5 seconds max delay

    // create delay buffer
    juce::AudioBuffer<float> delayBuffer;
    int bufferSize = 0;

    // store the current write position in the delay buffer
    int writePos = 0;

    // juce::Array to hold grains instead of std::vector
    juce::Array<Grain> grains;
    float grainTriggerTimer = 0.0f;      // timer for triggering new grains
    float samplesPerGrainTrigger = 0.0f; // samples per grain trigger based on density

    // random number generator for spread + pitch shift + grain density
    std::mt19937 rng;

    // distributions for generating random values
    std::uniform_real_distribution<float> spreadDist {-1.0f, 1.0f};

    // helper methods
    void triggerNewGrain();
    void updateGrainTiming();
    float processActiveGrains(int channel);
    float getDelayedSample(int channel, float position);
    float applyWindow(float phase); // hanning window function for grains (TODO: implement more window types)
    int samplesToDelayPosition(float delaySamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularProcessor)
};

#endif //GRANULARPROCESSOR_H
