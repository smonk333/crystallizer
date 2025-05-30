//
// Created by smoke on 5/30/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

#ifndef REVERBPROCESSOR_H
#define REVERBPROCESSOR_H



class ReverbProcessor {
public:
    ReverbProcessor();
    ~ReverbProcessor();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

    void process(
        juce::AudioBuffer<float>& buffer,
        float* cleanSignalL,
        float* cleanSignalR,
        float roomSize,
        float damping,
        float wetLevel,
        float dryLevel,
        float width,
        float freezeMode);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    // additional processing for stereo width
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    double sampleRate;
};



#endif //REVERBPROCESSOR_H
