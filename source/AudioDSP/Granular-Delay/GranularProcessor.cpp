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
    bufferSize = static_cast<int>(sampleRate * 5.0f); // 5 seconds max delay

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

    // preallocate grain pool and mark all as inactive
    grains.clear();
    for (int i = 0; i < maxGrains; ++i)
    {
        Grain g;
        g.active = false;
        grains.add(g);
    }

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
            float feedbackL = delayBuffer.getSample(0, (writePos - 1 + bufferSize) % bufferSize);
            float safeFeedback = juce::jlimit(0.0f, 0.95f, granularParams.feedback);
            delayBuffer.setSample(0,
                writePos,
                inputL + (feedbackL * safeFeedback)
            );

            if (numChannels > 1)
            {
                float feedbackR = delayBuffer.getSample(1, (writePos - 1 + bufferSize) % bufferSize);
                delayBuffer.setSample(1,
                    writePos,
                    inputR + (feedbackR * safeFeedback)
                );
            }
            else
            {
                delayBuffer.setSample(1,
                    writePos,
                    inputL + (feedbackL * safeFeedback)
                );
            }

            // mix clean and delayed signals
            float outputL = (delayedL * granularParams.wetDryMix) + (cleanL * (1.0f - granularParams.wetDryMix));
            float outputR = (delayedR * granularParams.wetDryMix) + (cleanR * (1.0f - granularParams.wetDryMix));

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

void GranularProcessor::updateParameters(const GranularParams& params)
{
    granularParams = params;
    // ensure grainDensity is always positive and reasonable
    granularParams.grainDensity = juce::jlimit(0.01f, 100.0f, granularParams.grainDensity);
    granularParams.grainSize = juce::jlimit(0.001f, 2.0f, granularParams.grainSize);
    updateGrainTiming();
}

void GranularProcessor::triggerNewGrain()
{
    // find an inactive grain to reuse
    for (int i = 0; i < grains.size(); ++i)
    {
        if (!grains.getReference(i).active)
        {
            resetGrain(grains.getReference(i));
            return;
        }
    }
    // If all grains are active, do nothing (or could replace oldest)
}

void GranularProcessor::updateGrainTiming()
{
    samplesPerGrainTrigger = static_cast<float>(sampleRate / granularParams.grainDensity);
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

void GranularProcessor::resetGrain(Grain& grain)
{
    grain.active = true;
    grain.totalSamples = static_cast<int>(granularParams.grainSize * sampleRate);
    grain.currentSample = 0;
    grain.pitchRatio = granularParams.pitchShift;
    grain.grainAmplitude = 0.5f;
    float baseDelayTime = granularParams.delayTime;
    float spreadAmount = granularParams.spread * granularParams.delayTime * spreadDist(rng);
    float grainDelayTime = baseDelayTime + spreadAmount;
    grainDelayTime = juce::jmax(0.01f, grainDelayTime);
    grain.delayPosition = static_cast<float>(grainDelayTime * sampleRate);
    grain.readPosition = grain.delayPosition;
}
