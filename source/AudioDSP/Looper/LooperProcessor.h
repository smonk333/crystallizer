//
// Created by smoke on 6/4/2025.
//

#pragma once

#ifndef LOOPERPROCESSOR_H
#define LOOPERPROCESSOR_H

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>

class LooperProcessor : public juce::dsp::ProcessorBase
{
public:
    struct LooperParams {
        // parameter for the looper, default to stopped (3, from the State enum)
        mutable int looperState = 3;
    };

    LooperProcessor();
    ~LooperProcessor() override;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void process (const juce::dsp::ProcessContextReplacing<float>& context) override;
    void handleRecording (float inputL, float inputR, float& outL, float& outR);
    void handlePlaying (float& outL, float& outR);
    void handleOverdubbing (float inputL, float inputR, float& outL, float& outR);

    // Looper state management
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
    int maxBufferSize = 0;

    // keep track of previous button states to detect changes
    LooperParams previousParams;

    // function pointer for per-sample processing
    std::function<void(float, float, float&, float&)> processSample = nullptr;
    void setState(State newState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperProcessor)
};

#endif // LOOPERPROCESSOR_H
