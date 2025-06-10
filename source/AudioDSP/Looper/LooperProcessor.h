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
    struct LooperParams {
        int looperState = 0; // Use int for enum State
    };

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
        Recording = 0,      // capturing new audio
        Playing = 1,        // replaying the captured audio
        Overdubbing = 2,    // adding new audio to the existing loop
        Stopped = 3,        // looper is not active
        Clear = 4           // special command to clear the loop
    };

    // parameter getters
    State getState() const noexcept { return currentState; }
    float getLoopPosition() const noexcept;

    void updateParameters(const LooperParams& params);

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
