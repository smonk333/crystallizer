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

void ReverbProcessor::prepare (const juce::dsp::ProcessSpec& spec)
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

void ReverbProcessor::process (juce::AudioBuffer<float>& buffer,
    float* cleanSignalL, float* cleanSignalR, float roomSize, float damping,
    float wetLevel, float dryLevel, float width, float freezeMode)
{
    // update reverb parameters
    reverbParams.roomSize = juce::jlimit(0.0f, 1.0f, roomSize);
    reverbParams.damping = juce::jlimit(0.0f, 1.0f, damping);
    reverbParams.wetLevel = juce::jlimit(0.0f, 1.0f, wetLevel);
    reverbParams.dryLevel = juce::jlimit(0.0f, 1.0f, dryLevel);
    reverbParams.width = juce::jlimit(0.0f, 1.0f, width);
    reverbParams.freezeMode = freezeMode > 0.5f ? 1.0f : 0.0f; // binary toggle

    // apply parameters to the reverb
    reverb.setParameters(reverbParams);

    // update low-pass filter based on damping
    float cutoffFreq = juce::jmap(damping, 0.0f,
        1.0f, 20000.0f, 1000.0f);
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, cutoffFreq, 0.707f);

    // grab channel pointers from the buffer
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    // create audio block for processing the reverb

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // process with reverb

    reverb.process(context);

    // apply additional low-pass filtering to enhance damping effect
    if (damping > 0.1f)
    {
        lowPassFilter.process(context);
    }

    // manual wet/dry mixing for finer control
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // the reverb already applies an internal wet/dry mix,
        // but we can add additional control here if needed
        float wetL = channelDataL[sample];
        float wetR = channelDataR[sample];

        // apply stereo width adjustment
        float mid = (wetL + wetR) * 0.5f;
        float side = (wetL - wetR) * width * 0.5f;

        wetL = mid + side;
        wetR = mid - side;

        // final mix with clean signal
        channelDataL[sample] = wetL + (cleanSignalL[sample] * (dryLevel * 0.5f));
        channelDataR[sample] = wetR + (cleanSignalR[sample] * (dryLevel * 0.5f));
    }

}