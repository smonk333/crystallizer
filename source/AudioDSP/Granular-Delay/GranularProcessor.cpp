//
// Created by smoke on 6/2/2025.
//

#include "GranularProcessor.h"

GranularProcessor::GranularProcessor()
{
    // empty constructor
}

GranularProcessor::~GranularProcessor()
{
    // empty destructor
}

void GranularProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // calculate buffer size for max delay in samples
    bufferSize = static_cast<int>(spec.sampleRate * 5.0f); // 5 seconds max delay

    // prepare AudioBuffer for delay
    delayBuffer.setSize(2, bufferSize); // allocate for a stereo buffer

    // reset to clear buffers and vectors
    reset();
}

void GranularProcessor::reset()
{
    // clear delay buffer
    delayBuffer.clear();
    writePos = 0;

    // clear all active grains and initialize grain vector with 200 grains
    grains.clear();
    grains.reserve(200);

    // reset grain trigger timer and update grain timing
    updateGrainTiming();
    grainTriggerTimer = 0.0f;
}

void GranularProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (!context.isBypassed)
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        const auto numChannels = juce::jmin(
            inputBlock.getNumChannels(),
            outputBlock.getNumChannels(),
            static_cast<size_t> (2)
        );


        const auto numSamples = inputBlock.getNumSamples();

        for (int i = 0; i < static_cast<int>(numSamples); ++i)
        {
            // read clean signal
            float inputL = inputBlock.getSample(0, i);
            float inputR;

            // handle mono and stereo signals
            if (numChannels > 1)
                inputR = inputBlock.getSample (1, i);
            else
                inputR = inputL;

            // store clean signal for mixing later
            float cleanL = inputL;
            float cleanR = inputR;

            // check if we should trigger new grains
            grainTriggerTimer += 1.0f;
            if (grainTriggerTimer >= samplesPerGrainTrigger)
            {
                triggerNewGrain();
                grainTriggerTimer = 0.0f;
            }

            // process all active grains to get delayed signal
            // also handle stereo channels
            float delayedL = processActiveGrains(0);
            float delayedR;
            if (numChannels > 1)
                delayedR = processActiveGrains (1);
            else
                delayedR = processActiveGrains (0);

            // write input + feedback to delay buffer
            delayBuffer.setSample(0,
                writePos,
                inputL + (delayedL * currentFeedback)
            );

            if (numChannels > 1)
            {
                delayBuffer.setSample(1,
                    writePos,
                    inputR + (delayedR * currentFeedback)
                );
            }

            else
            {
                delayBuffer.setSample(1,
                    writePos,
                    inputL + (delayedL * currentFeedback)
                );
            }

            // mix clean and delayed signals
            float outputL = (delayedL * currentWetDryMix) + (cleanL * (1.0f - currentWetDryMix));
            float outputR = (delayedR * currentWetDryMix) + (cleanR * (1.0f - currentWetDryMix));

            // write output to the output block
            outputBlock.setSample(0, i, outputL);
            if (numChannels > 1)
            {
                outputBlock.setSample(1, i, outputR);
            }

            // advance write position in the delay buffer
            writePos = (writePos + 1) % bufferSize;
        }
    }

}

void GranularProcessor::setDelayTime(float newDelayTime)
{
    currentDelayTime = juce::jlimit(0.01f, 5.0f, newDelayTime);
}

void GranularProcessor::setGrainSize(float newGrainSize)
{
    currentGrainSize = juce::jlimit(0.01f, 1.0f, newGrainSize);
}

void GranularProcessor::setGrainDensity(float newDensity)
{
    currentGrainDensity = juce::jlimit(0.5f, 50.0f, newDensity);
    updateGrainTiming();
}

void GranularProcessor::setPitchShift(float newPitchShift)
{
    currentPitchShift = juce::jlimit(-4.0f, 4.0f, newPitchShift);
}

void GranularProcessor::setFeedback(float newFeedback)
{
    currentFeedback = juce::jlimit(0.0f, 1.0f, newFeedback);
}

void GranularProcessor::setWetDryMix(float newWetDryMix)
{
    currentWetDryMix = juce::jlimit(0.0f, 1.0f, newWetDryMix);
}

void GranularProcessor::setSpread(float newSpread)
{
    currentSpread = juce::jlimit(0.0f, 1.0f, newSpread);
}

// legacy parameter update method
// void GranularProcessor::updateParameters(float delayTime, float grainSize, float density, float pitchShift, float feedback, float wetDryMix, float spread)
// {
//     setDelayTime(delayTime);
//     setGrainSize(grainSize);
//     setGrainDensity(density);
//     setSpread(spread);
//     setPitchShift(pitchShift);
//     setFeedback(feedback);
//     setWetDryMix(wetDryMix);
//
//     // update grain timing after changing density
//     updateGrainTiming();
// }

void GranularProcessor::updateParameters(const juce::AudioProcessorValueTreeState& apvts)
{
    auto delayTimeParam = apvts.getRawParameterValue("granularDelayTime");
    auto grainSizeParam = apvts.getRawParameterValue("grainSize");
    auto densityParam = apvts.getRawParameterValue("grainDensity");
    auto pitchShiftParam = apvts.getRawParameterValue("pitchShift");
    auto feedbackParam = apvts.getRawParameterValue("granularFeedback");
    auto wetDryMixParam = apvts.getRawParameterValue("wetDryMix");
    auto spreadParam = apvts.getRawParameterValue("spread");

    if (delayTimeParam) setDelayTime(*delayTimeParam);
    if (grainSizeParam) setGrainSize(*grainSizeParam);
    if (densityParam) setGrainDensity(*densityParam);
    if (pitchShiftParam) setPitchShift(*pitchShiftParam);
    if (feedbackParam) setFeedback(*feedbackParam);
    if (wetDryMixParam) setWetDryMix(*wetDryMixParam);
    if (spreadParam) setSpread(*spreadParam);
}

void GranularProcessor::triggerNewGrain()
{
    // create new grain
    Grain newGrain;
    newGrain.active = true;

    // calculate grain parameters
    newGrain.totalSamples = static_cast<int>(currentGrainSize * sampleRate);
    newGrain.currentSample = 0;
    newGrain.pitchRatio = currentPitchShift;
    newGrain.grainAmplitude = 0.5f; // base amplitude

    // set grain delay position with spread
    float baseDelayTime = currentDelayTime;
    float spreadAmount = currentSpread * currentDelayTime * spreadDist(rng);
    float grainDelayTime = baseDelayTime + spreadAmount;

    // ensure positive delay time
    grainDelayTime = juce::jmax(0.01f, grainDelayTime);

    newGrain.delayPosition = static_cast<float>(grainDelayTime * sampleRate);
    newGrain.readPosition = newGrain.delayPosition;

    // add the new grain to the vector
    grains.push_back(newGrain);

    // clean up finished grains to prevent unlimited growth
    std::erase_if (grains,
        [] (const Grain& g) { return !g.active; });
}

void GranularProcessor::updateGrainTiming()
{
    samplesPerGrainTrigger = static_cast<float>(sampleRate / currentGrainDensity);
}

float GranularProcessor::processActiveGrains(int channel)
{
    float output = 0.0f;

    // process each active grain
    for (auto& grain : grains)
    {
        if (!grain.active || grain.currentSample >= grain.totalSamples)
        {
            grain.active = false;
        }

        // apply window based on grain position within its length
        float grainPhase = static_cast<float>(grain.currentSample) / static_cast<float>(grain.totalSamples);
        float envelope = applyWindow(grainPhase);

        // get sample from delay buffer with pitch shifting
        float sample = getDelayedSample(channel, grain.readPosition);

        // apply amplitude and envelope
        output += sample * grain.grainAmplitude * envelope;

        // advance grain
        grain.readPosition += grain.pitchRatio;
        grain.currentSample++;

        // wrap read position around the delay buffer
        while (grain.readPosition >= bufferSize)
            grain.readPosition -= bufferSize;
        while (grain.readPosition < 0)
            grain.readPosition += bufferSize;

        // deactivate finished grains
        if (grain.currentSample >= grain.totalSamples)
        {
            grain.active = false;
        }
    }

    return output;
}

float GranularProcessor::getDelayedSample(int channel, float position)
{
    // ensure position is within bounds
    while (position < 0)
        position += bufferSize;
    while (position >= bufferSize)
        position -= bufferSize;

    // linear interpolation for fractional delay positions
    int pos1 = static_cast<int>(std::floor(position)) % bufferSize;
    int pos2 = (pos1 + 1) % bufferSize;
    float frac = position - std::floor(position);

    float sample1 = delayBuffer.getSample(channel, pos1);
    float sample2 = delayBuffer.getSample(channel, pos2);

    return sample1 * (1.0f - frac) + sample2 * frac;
}

float GranularProcessor::applyWindow(float phase)
{
    // hanning window
    phase = juce::jlimit(0.0f, 1.0f, phase);
    return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * phase));
}

int GranularProcessor::samplesToDelayPosition(float delaySamples)
{
    int delayPos = writePos - static_cast<int>(delaySamples);
    while (delayPos < 0)
        delayPos += bufferSize;
    return delayPos % bufferSize;
}

