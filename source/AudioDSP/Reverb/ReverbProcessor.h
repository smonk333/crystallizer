//
// Created by smoke on 5/30/2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

#ifndef REVERBPROCESSOR_H
#define REVERBPROCESSOR_H

class ReverbProcessor : public juce::dsp::ProcessorBase {
public:
    ReverbProcessor();
    ~ReverbProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;

    // original process method (keep for backward compatibility)
    // void process(
    //     juce::AudioBuffer<float>& buffer,
    //     float* cleanSignalL,
    //     float* cleanSignalR,
    //     float roomSize,
    //     float damping,
    //     float wetLevel,
    //     float dryLevel,
    //     float width,
    //     float freezeMode);

    // Required implementation of ProcessorBase::process
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // parameter setters for ProcessorChain use
    void setRoomSize (float newRoomSize);
    void setDamping (float newDamping);
    void setWetLevel (float newWetLevel);
    void setDryLevel (float newDryLevel);
    void setWidth (float newWidth);
    void setFreezeMode (float newFreezeMode);

    // unified method to update all parameters at once
    void updateParameters (float roomSize, float damping, float wet,
        float dry, float width, float freeze);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    // additional processing for stereo width
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    double sampleRate;

    // parameters for ProcessorChain compatibility
    float currentRoomSize = 0.5f;
    float currentDamping = 0.5f;
    float currentWetLevel = 0.33f;
    float currentDryLevel = 0.4f;
    float currentWidth = 1.0f;
    float currentFreezeMode = 0.0f;
};

#endif //REVERBPROCESSOR_H
