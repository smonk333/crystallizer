//
// Created by smoke on 6/2/2025.
//

/**
 * @file GranularProcessor.cpp
 * @brief Granular delay processor with pitch shifting capabilities
 *
 * Implements real-time granular synthesis on delayed audio signals.
 * Supports variable grain size, density, and pitch shifting.
 *
 * @description
 * delayTime: Delay time in seconds (0.01 - 5.0)
 * grainSize: Grain size in seconds (0.01 - 5.0)
 * density: Influences chance of grains spawning (0.01 - 10.0)
 * pitchShift: Ratio for pitch shifting (-4.0 to 4.0) (work in progress)
 * feedback: Feedback level (0.0 - 1.0)
 * wetDryMix: Wet/dry mix ratio (0.0 - 1.0)
 * spread: Random position spread for grains (0.0 - 1.0)
 */

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
    int maxGrains = 256; // maximum number of grains in the pool
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
    void resetGrain(Grain& grain);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularProcessor)
};

#endif //GRANULARPROCESSOR_H
