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

    // original process method (keep for backward compatibility)
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

    // process method for ProcessorChain compatibility
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        // make sure we have right number of channels
        jassert(inputBlock.getNumChannels() == 2);
        jassert(outputBlock.getNumChannels() == 2);

        // create an AudioBlock to work with
        juce::dsp::AudioBlock<float> block(outputBlock);

        // copy input to output if they're not already the same
        if (!context.isBypassed)
        {
            if (context.usesSeparateInputAndOutputBlocks())
                block.copyFrom(inputBlock);

            // store clean signal for dry/wet mixing
            juce::HeapBlock<float> cleanSignalL(block.getNumSamples());
            juce::HeapBlock<float> cleanSignalR(block.getNumSamples());

            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                cleanSignalL[i] = block.getSample(0, i);
                cleanSignalR[i] = block.getSample(1, i);
            }

            // apply reverb parameters
            reverbParams.roomSize = currentRoomSize;
            reverbParams.damping = currentDamping;
            reverbParams.wetLevel = currentWetLevel;
            reverbParams.dryLevel = currentDryLevel;
            reverbParams.width = currentWidth;
            reverbParams.freezeMode = currentFreezeMode > 0.5f;
            reverb.setParameters(reverbParams);

            // process through reverb
            juce::dsp::ProcessContextReplacing<float> reverbContext(block);
            reverb.process(reverbContext);

            // apply low-pass filter for damping enhancement
             *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
                 sampleRate, 12000.0f, 0.707f);
             lowPassFilter.process(reverbContext);
        }
    }

    // parameter setters for ProcessorChain use - inline for better performance
    void setRoomSize(float newRoomSize) { currentRoomSize = newRoomSize; }
    void setDamping(float newDamping) { currentDamping = newDamping; }
    void setWetLevel(float newWetLevel) { currentWetLevel = newWetLevel; }
    void setDryLevel(float newDryLevel) { currentDryLevel = newDryLevel; }
    void setWidth(float newWidth) { currentWidth = newWidth; }
    void setFreezeMode(float newFreezeMode) { currentFreezeMode = newFreezeMode; }

    // unified method to update all parameters at once
    void updateParameters(float roomSize, float damping, float wet,
        float dry, float width, float freeze)
    {
        setRoomSize(roomSize);
        setDamping(damping);
        setWetLevel(wet);
        setDryLevel(dry);
        setWidth(width);
        setFreezeMode(freeze);
    }

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
