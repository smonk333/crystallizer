//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef LABELSETUP_H
#define LABELSETUP_H

#include <juce_gui_basics/juce_gui_basics.h>

class LabelSetup {
public:
    // sets up a standard label with centered text
    static void setupLabel(juce::Label& label, const juce::String& text, juce::Component* parent);
};

#endif //LABELSETUP_H
