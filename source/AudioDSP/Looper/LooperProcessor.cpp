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
    maxBufferSize = static_cast<int>(60 * sampleRate); // 60 seconds of looper memory

    // prepare AudioBuffer for loop
    loopBuffer.setSize(2, maxBufferSize);
    reset();
}

void LooperProcessor::reset()
{
    loopBuffer.clear();
    position = 0;
    loopLength = 0;
    currentState = Stopped;
}

void LooperProcessor::setState(State newState)
{
    // DBG("setState: " << currentState << " -> " << newState);
    currentState = newState;
    switch (currentState)
    {
        case Recording:
            processSample = [this](float inL, float inR, float& outL, float& outR) { handleRecording(inL, inR, outL, outR); };
            break;
        case Playing:
            processSample = [this](float, float, float& outL, float& outR) { handlePlaying(outL, outR); };
            break;
        case Overdubbing:
            processSample = [this](float inL, float inR, float& outL, float& outR) { handleOverdubbing(inL, inR, outL, outR); };
            break;
        case Stopped:
            processSample = [this](float inL, float inR, float& outL, float& outR) { outL = inL; outR = inR; };
            break;
        case Clear:
            processSample = [this](float, float, float&, float&) { clear(); currentState = Stopped; };
            break;
        default:
            processSample = nullptr;
            break;
    }
}

void LooperProcessor::process (const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (context.isBypassed) return;

    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();
    const auto numChannels = juce::jmin(
        inputBlock.getNumChannels(),
        outputBlock.getNumChannels(),
        static_cast<size_t> (2)
    );
    const auto numSamples = inputBlock.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float inputL = inputBlock.getSample(0, sample);
        float inputR = (numChannels > 1) ? inputBlock.getSample(1, sample) : inputL;
        float outL = 0.0f, outR = 0.0f;

        if (processSample)
            processSample(inputL, inputR, outL, outR);

        outputBlock.setSample(0, sample, outL);
        if (numChannels > 1)
            outputBlock.setSample(1, sample, outR);
        else
            outputBlock.setSample(1, sample, outL); // mono case
    }
}

// Helper methods for each state
void LooperProcessor::handleRecording(float inputL, float inputR, float& outL, float& outR)
{
    loopBuffer.setSample(0, position, inputL);
    loopBuffer.setSample(1, position, inputR);
    position++;
    // Always update loopLength to the current position while recording
    loopLength = position;
    // DBG("Recording: pos=" << position << ", inL=" << inputL << ", inR=" << inputR << ", loopLength=" << loopLength);
    outL = inputL;
    outR = inputR;
    if (position >= maxBufferSize)
    {
        // If we hit max buffer, set loopLength and switch to playing
        loopLength = maxBufferSize;
        position = 0;
        setState(Playing);
    }
}

void LooperProcessor::handlePlaying(float& outL, float& outR)
{
    outL = loopBuffer.getSample(0, position);
    outR = loopBuffer.getSample(1, position);
    // DBG("Playing: pos=" << position << ", outL=" << outL << ", outR=" << outR << ", loopLength=" << loopLength);
    position = (position + 1) % juce::jmax(1, loopLength);
}

void LooperProcessor::handleOverdubbing(float inputL, float inputR, float& outL, float& outR)
{
    // Simple overdub: add input to buffer sample
    float mixL = loopBuffer.getSample(0, position) + inputL;
    float mixR = loopBuffer.getSample(1, position) + inputR;
    loopBuffer.setSample(0, position, mixL);
    loopBuffer.setSample(1, position, mixR);
    outL = mixL;
    outR = mixR;
    position = (position + 1) % juce::jmax(1, loopLength);
}

// state management methods
void LooperProcessor::startRecording()
{
    // Optionally clear previous loop (uncomment if you want this behavior)
    // clear();
    setState(Recording);
    // DBG("RECORDING");
    position = 0;
    // Optionally clear loopLength to avoid confusion
    // loopLength = 0;
}

void LooperProcessor::startPlayback()
{
    if (loopLength > 0)
    {
        position = 0;
        setState(Playing);
        // DBG("PLAYING");
    }
    else
    {
        // DBG("Cannot start playback: loopLength is 0");
    }
}

void LooperProcessor::startOverdubbing()
{
    if (loopLength > 0)
    {
        setState(Overdubbing);
        // DBG("OVERDUBBING");
    }
}

void LooperProcessor::stop()
{
    if (currentState == Recording)
    {
        loopLength = position; // Always set loopLength to number of recorded samples
        // DBG("STOPPED RECORDING, LOOP LENGTH = " << loopLength << " samples (" << (loopLength / sampleRate) << " seconds)");
    }
    position = 0;
    setState(Stopped);
}

void LooperProcessor::clear()
{
    loopBuffer.clear();
    position = 0;
    loopLength = 0;
    // DBG("LOOP CLEARED");
    setState(Stopped);
    // DBG("current state: " << currentState);
}

float LooperProcessor::getLoopPosition() const noexcept
{
    if (loopLength == 0)
        return 0.0f;
    return static_cast<float>(position) / static_cast<float>(loopLength);
}

void LooperProcessor::updateParameters(const LooperParams& params)
{
    if (static_cast<int>(currentState) != params.looperState)
    {
        // DBG("LooperProcessor: State changing from " << currentState << " to " << params.looperState);
        setState(static_cast<State>(params.looperState));
    }
    previousParams = params;
}
