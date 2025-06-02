//
// Created by smoke on 5/30/2025.
//

#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor()
{
    // empty constructor
}

ReverbProcessor::~ReverbProcessor()
{
    // empty destructor
}

void ReverbProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // prepare the reverb processor
    reverb.prepare(spec);
    reverb.reset();

    // prepare low-pass filter for damping enhancement
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    // set initial filter coefficients (12khz cutoff)
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        spec.sampleRate, 12000.0f, 0.707f);
}

void ReverbProcessor::reset()
{
    reverb.reset();
    lowPassFilter.reset();
}

// legacy process method for backward compatibility
void ReverbProcessor::process(juce::AudioBuffer<float>& buffer,
    float* cleanSignalL, float* cleanSignalR, float roomSize, float damping,
    float wetLevel, float dryLevel, float width, float freezeMode)
{
    // update reverb parameters
    reverbParams.roomSize = juce::jlimit(0.0f, 1.0f, roomSize);
    reverbParams.damping = juce::jlimit(0.0f, 1.0f, damping);
    reverbParams.wetLevel = juce::jlimit(0.0f, 1.0f, wetLevel);
    reverbParams.dryLevel = juce::jlimit(0.0f, 1.0f, dryLevel);
    reverbParams.width = juce::jlimit(0.0f, 1.0f, width);
    if (freezeMode > 0.5f)
        reverbParams.freezeMode = true;
    else
        reverbParams.freezeMode = false;

    reverb.setParameters(reverbParams);

    // apply reverb to the buffer (stereo only)
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

