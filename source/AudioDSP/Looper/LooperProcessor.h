//
// Created by smoke on 6/4/2025.
//

#pragma once

#ifndef LOOPERPROCESSOR_H
#define LOOPERPROCESSOR_H

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class LooperProcessor : public juce::dsp::ProcessorBase
{
public:

    LooperProcessor();
    ~LooperProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    // looper state management
    void startRecording();
    void startPlayback();
    void startOverdubbing();
    void stop();
    void clear();

    // enum for State
    enum State
    {
        Stopped,        // looper is not active
        Recording,      // capturing new audio
        Playing,        // replaying the captured audio
        Overdubbing     // adding new audio to the existing loop
    };

    // parameter getters
    State getState() const noexcept { return currentState; }
    float getLoopPosition() const noexcept;

    void updateParameters(const juce::AudioProcessorValueTreeState& apvts);

private:
    juce::AudioBuffer<float> loopBuffer;
    int loopLength = 0;
    int position = 0;
    State currentState = Stopped;
    double sampleRate = 44100.0f;
    int maxBufferSize = 0; // Will be set in prepare() based on sampleRate

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperProcessor)
};



#endif //LOOPERPROCESSOR_H
