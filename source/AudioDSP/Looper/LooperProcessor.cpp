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
    if (!context.isBypassed)
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        // set up mix variables for overdubbing
        float mixL = 0.0f, mixR = 0.0f;

        const auto numChannels = juce::jmin(
            inputBlock.getNumChannels(),
            outputBlock.getNumChannels(),
            static_cast<size_t> (2)
        );
        const auto numSamples = inputBlock.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // read input samples
            float inputL = inputBlock.getSample(0, sample);
            float inputR;
            if (numChannels > 1)
                inputR = inputBlock.getSample (1, sample);
            else
                inputR = inputL;

            // handle looper states
            switch (currentState)
            {
                case Recording:
                    // store input samples in loop buffer and increment position
                    loopBuffer.setSample(0, position, inputL);
                    loopBuffer.setSample(1, position, inputR);
                    position++;

                    // check if buffer is full
                    if (position >= maxBufferSize)
                    {
                        loopLength = maxBufferSize;

                        // reset position to start of loop
                        position = 0;

                        // switch to playing state because we hit the end of the buffer
                        currentState = Playing;
                    }
                    break;

                case Playing:
                    // read samples from loop buffer and write to output
                    inputL = loopBuffer.getSample(0, position);
                    inputR = loopBuffer.getSample(1, position);

                    // wrap to the start of the loop if we reach the end
                    position = (position + 1) % juce::jmax(1, loopLength);
                    break;

                case Overdubbing:
                    // mix input with existing loop samples
                    mixL = loopBuffer.getSample(0, position) + inputL;
                    mixR = loopBuffer.getSample(1, position) + inputR;

                    // store mixed result
                    loopBuffer.setSample(0, position, mixL);
                    loopBuffer.setSample(1, position, mixR);

                    // output mixed samples
                    inputL = mixL;
                    inputR = mixR;

                    // wrap to the start of the loop if we reach the end
                    position = (position + 1) % juce::jmax(1, loopLength);
                    break;

                case Stopped:
                    // pass input directly to output, no processing needed
                    break;

                default:
                    // handle unexpected state (*should* never happen, but just to be safe)
                    jassertfalse; // debug break if we hit this case
                    break;
            }

            // write to output block
            outputBlock.setSample(0, sample, inputL);
            if (numChannels > 1)
            {
                outputBlock.setSample(1, sample, inputR);
            }
            else
            {
                outputBlock.setSample(1, sample, inputL); // mono case
            }
        }
    }

}

// state management methods
void LooperProcessor::startRecording()
{
    clear();
    currentState = Recording;
}

void LooperProcessor::startPlayback()
{
    if (loopLength > 0)
    {
        // go to the start of the loop to play
        position = 0;
        currentState = Playing;
    }
}

void LooperProcessor::startOverdubbing()
{
    if (loopLength > 0)
    {
        currentState = Overdubbing;
    }
}

void LooperProcessor::stop()
{
    if (currentState == Recording)
    {
        loopLength = position;
    }
    position = 0;
    currentState = Stopped;
}

void LooperProcessor::clear()
{
    loopBuffer.clear();
    position = 0;
    loopLength = 0;
    currentState = Stopped;
}

float LooperProcessor::getLoopPosition() const noexcept
{
    if (loopLength == 0)
        return 0.0f;
    return static_cast<float>(position) / static_cast<float>(loopLength);
}

void LooperProcessor::updateParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    // parameter retrieval from APVTS
    auto looperStateParam = apvts.getRawParameterValue("looperState");  // Changed from "loopState" to "looperState"

    if (looperStateParam)
    {
        int stateValue = static_cast<int>(*looperStateParam);

        // Only change state if it's different from current state
        if (stateValue != static_cast<int>(currentState))
        {
            // Call the appropriate state management method based on parameter value
            switch (stateValue)
            {
                case Stopped:
                    stop();
                    break;

                case Recording:
                    startRecording();
                    break;

                case Playing:
                    startPlayback();
                    break;

                case Overdubbing:
                    startOverdubbing();
                    break;

                case 4: // Clear
                    clear();
                    break;

                default:
                    // Handle unexpected state value
                    jassertfalse;
                    break;
            }
        }
    }
}