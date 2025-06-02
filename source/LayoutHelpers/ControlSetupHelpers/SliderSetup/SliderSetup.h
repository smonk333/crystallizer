//
// Created by smoke on 6/2/2025.
//

#pragma once

#ifndef SLIDERSETUP_H
#define SLIDERSETUP_H

#include <juce_gui_basics/juce_gui_basics.h>

class SliderSetup {
public:
    // sets up a rotary slider with standard appearance
    static void setupRotarySlider(juce::Slider& slider, juce::Component* parent);
};

#endif //SLIDERSETUP_H

