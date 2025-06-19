//
// Created by smoke on 5/30/2025.
//

/**
 * @file ReverbProcessor.cpp
 * @brief Reverb processor with adjustable parameters
 *
 * Implements a standard reverb effect, with parameters for room size, damping, wet/dry mix, stereo width, and freeze mode.
 *
 * @description
 * roomSize: Size of the reverb room (0.0 - 1.0)
 * damping: High pass filter damping (0.0 - 1.0)
 * wetLevel: Wet level of the reverb (0.0 - 1.0)
 * dryLevel: Dry level of the reverb (0.0 - 1.0)
 * width: Stereo width of the reverb (0.0 - 1.0)
 * freezeMode: Freeze mode (0.0 - 1.0, implemented as a toggle button in the UI
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

    // required implementation of ProcessorBase::process
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    void updateParameters(const ReverbParams& params);
private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    // additional processing for stereo width
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                juce::dsp::IIR::Coefficients<float>> lowPassFilter;

    double sampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbProcessor)
};

#endif //REVERBPROCESSOR_H