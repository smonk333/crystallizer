//
// Created by smoke on 6/4/2025.
//

/**
 * @brief Phrase looper processor with 60 seconds of memory, as well as an overdubbing layer (overdubbing tba)
 *
 * Implements a looper with recording, playback, overdubbing, and clearing functionality.
 *
 * @param looperState State of the looper (0: Recording, 1: Playing, 2: Overdubbing, 3: Stopped, 4: Clear)
 */

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
    float getElapsedTimeSeconds() const noexcept { return position / static_cast<float>(sampleRate); }
    const char* getStateString() const noexcept {
        switch (currentState) {
            case Recording: return "Recording";
            case Playing: return "Playing";
            case Overdubbing: return "Overdubbing";
            case Stopped: return "Stopped";
            case Clear: return "Clear";
            default: return "Unknown";
        }
    }

    void updateParameters(const LooperParams& params);

    // add a method to set the looper state parameter pointer
    void setLooperStateParam(std::atomic<float>* param) { looperStateParam = param; }

    // add a method to check and clear the flag
    bool consumeBufferLimitReachedFlag() {
        return bufferLimitReached.exchange(false);
    }

    // add a method to check and clear the flag
    bool consumeClearProcessedFlag() {
        return clearProcessed.exchange(false);
    }
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

    std::atomic<float>* looperStateParam = nullptr;

    // thread-safe flag to notify UI/main thread that the buffer limit was reached
    std::atomic<bool> bufferLimitReached { false };

    // thread-safe flag to notify UI/main thread that clear was processed
    std::atomic<bool> clearProcessed { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperProcessor)
};

#endif // LOOPERPROCESSOR_H
