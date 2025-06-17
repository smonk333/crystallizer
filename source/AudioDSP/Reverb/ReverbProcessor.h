//
// Created by smoke on 5/30/2025.
//

/**
 * @brief Reverb processor with adjustable parameters
 *
 * Implements a standard reverb effect, with parameters for room size, damping, wet/dry mix, stereo width, and freeze mode.
 *
 * @param roomSize Size of the reverb room (0.0 - 1.0)
 * @param damping High pass filter damping (0.0 - 1.0)
 * @param wetLevel Wet level of the reverb (0.0 - 1.0)
 * @param dryLevel Dry level of the reverb (0.0 - 1.0)
 * @param width Stereo width of the reverb (0.0 - 1.0)
 * @param freezeMode Freeze mode (0.0 - 1.0, implemented as a toggle button in the UI
 */

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

#ifndef REVERBPROCESSOR_H
#define REVERBPROCESSOR_H

class ReverbProcessor : public juce::dsp::ProcessorBase {
public:
    struct ReverbParams {
        float roomSize = 0.5f;
        float damping = 0.5f;
        float wetLevel = 0.33f;
        float dryLevel = 0.4f;
        float width = 1.0f;
        float freezeMode = 0.0f;
    };

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

    void updateParameters(const ReverbParams& params);

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};

#endif //REVERBPROCESSOR_H
