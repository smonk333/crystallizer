//
// Created by smoke on 6/6/2025.
//

#include "SignalPathManager.h"

SignalPathManager::SignalPathManager()
{
    // empty constructor
}

SignalPathManager::~SignalPathManager()
{
    // empty destructor
}

void SignalPathManager::prepare (const juce::dsp::ProcessSpec& spec)
{
    // lazily load processors here based on the processing mode selected
}

void SignalPathManager::reset()
{

}

void SignalPathManager::process (const juce::dsp::ProcessContextReplacing<float>& context)
{

}