//
// Created by smoke on 6/4/2025.
//

#include "LooperProcessor.h"

LooperProcessor::LooperProcessor()
{
    // empty constructor
}

LooperProcessor::~LooperProcessor()
{
    // empty destructor
}

void LooperProcessor::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // calculate buffer size for maximum buffer length in samples
    const int bufferSize = static_cast<int>(60 * sampleRate); // 60 seconds of looper memory

    // prepare AudioBuffer for loop
    loopBuffer.setSize(2, bufferSize);
    reset();
}

void LooperProcessor::reset()
{
    loopBuffer.clear();
    position = 0;
    loopLength = 0;
    currentState = Stopped;
}

void LooperProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    const auto numSamples = inputBlock.getNumSamples();
    const auto numChannels = inputBlock.getNumChannels();

}

// void LooperProcessor::startRecording()
// {
// }
//
// void LooperProcessor::startPlayback()
// {
// }
//
// void LooperProcessor::startOverdubbing()
// {
// }
//
// void LooperProcessor::stop()
// {
// }
//
// void LooperProcessor::clear()
// {
// }
//
// float LooperProcessor::getLoopPosition() const noexcept
// {
// }