#pragma once

#include "AudioDSP/SignalPathManager/SignalPathManager.h"
#include "AudioDSP/Granular-Delay/GranularProcessor.h"
#include "AudioDSP/Reverb/ReverbProcessor.h"
#include "AudioDSP/Standard-Delay/DelayProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==========================APVTS setup=====================================

    // TODO: PROCESSOR_ADDITION_CHAIN(1): add an atomic pointer here for each new parameter in the new processor

    // // standard delay parameters
    // std::atomic<float>* delayTimeParam; // delay length in seconds
    // std::atomic<float>* feedbackParam;  // feedback amount
    // std::atomic<float>* wetDryParam;    // wet/dry mix
    //
    // // reverb parameters
    // std::atomic<float>* reverbRoomSizeParam;    // sets the size of the "room"
    // std::atomic<float>* reverbDampingParam;     // sets the damping of high frequencies
    // std::atomic<float>* reverbMixParam;         // sets the wet/dry mix of the reverb
    // std::atomic<float>* reverbWidthParam;       // sets the stereo width of the reverb
    // std::atomic<float>* reverbFreezeParam;      // sets the freeze mode of the reverb (now bool, but keep pointer for compatibility)
    //
    // // granular delay parameters
    // std::atomic<float>* granularDelayTimeParam;     // delay time in seconds
    // std::atomic<float>* grainSizeParam;             // grain size in seconds
    // std::atomic<float>* grainDensityParam;          // grains per second
    // std::atomic<float>* granularPitchShiftParam;    // pitch shift ratio (1.0 = no shift)
    // std::atomic<float>* granularFeedbackParam;      // feedback level (0.0 to 1.0)
    // std::atomic<float>* granularWetDryParam;        // wet/dry mix ratio (0.0 to 1.0)
    // std::atomic<float>* granularSpreadParam;        // random position spread
    //
    // // looper state management parameter
    // std::atomic<float>* looperStateParam; // 0 = recording, 1 = playing, 2 = overdubbing, 3 = stopped, 4 = clear
    //
    // // signal processing chain parameters
     std::atomic<float>* signalChainParam;


    // more fx parameters below here as we add classes to handle processing

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();

    void updateSignalPathManager(int newMode);

    SignalPathManager& getSignalPathManager() { return signalPathManager; }

private:
    class SignalPathParameterListener : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        explicit SignalPathParameterListener(PluginProcessor& processor) : processor(processor) {}

        void parameterChanged(const juce::String& parameterID, float newValue) override
        {
            if (parameterID == "signalPath")
            {
                processor.signalPathManager.setProcessingMode(
                    static_cast<SignalPathManager::ProcessingMode>(static_cast<int>(newValue))
                );
            }
        }

    private:
        PluginProcessor& processor;
    };

    std::unique_ptr<SignalPathParameterListener> signalPathListener;

    SignalPathManager signalPathManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
