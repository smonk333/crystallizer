//
// Created by smoke on 6/2/2025.
//
#pragma once

#ifndef TOGGLESETUP_H
#define TOGGLESETUP_H

#include <juce_gui_basics/juce_gui_basics.h>

class ToggleSetup {
public:
    // sets up a standard toggle button
    static void setupToggleButton(juce::TextButton& button, const juce::String& text, juce::Component* parent);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleSetup)
};

#endif //TOGGLESETUP_H

