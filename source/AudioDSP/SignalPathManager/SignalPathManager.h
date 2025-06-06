//
// Created by smoke on 6/6/2025.
//

#pragma once

#ifndef SIGNALPATHMANAGER_H
#define SIGNALPATHMANAGER_H

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

// include processor headers here
#include "../Granular-Delay/GranularProcessor.h"
#include "../Reverb/ReverbProcessor.h"
#include "../Standard-Delay/DelayProcessor.h"
#include "../Looper/LooperProcessor.h"

// add #include directives above for additional processors as we add them


class SignalPathManager : public juce::dsp::ProcessorBase
{
public:
    SignalPathManager();
    ~SignalPathManager() override;

    void prepare(const juce::dsp::ProcessSpec& spec, ) override;
    void reset() override;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalPathManager)
};



#endif //SIGNALPATHMANAGER_H
