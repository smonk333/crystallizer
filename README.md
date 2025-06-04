# crystallizer
This is my 5th attempt at writing a VST3 plugin with the PampleJUCE template + JUCE framework.
This README file is a work in progress, so it's gonna be a mess for a while.

# How to contribute / project structure:

### Process Flow for Adding New Parameters:

When adding a new parameter/processor, you need to:

1. Add atomic parameter pointers in [PluginProcessor.h](https://github.com/smonk333/crystallizer/tree/main/source/PluginProcessor.h)
2. Define parameters in createParams() in [PluginProcessor.cpp](https://github.com/smonk333/crystallizer/tree/main/source/PluginProcessor.cpp)
3. Initialize in the constructor (apvts.getRawParameterValue(), at the top of the PluginProcessor constructor)
4. Create an update method in [PluginProcessor.cpp](https://github.com/smonk333/crystallizer/tree/main/source/PluginProcessor.cpp) (similar to updateDelayParameters())
5. Call the update method in processBlock() (also in [PluginProcessor.cpp](https://github.com/smonk333/crystallizer/tree/main/source/PluginProcessor.cpp)
6. Create or update a layout class for the UI controls (add a new folder under [LayoutHelpers](https://github.com/smonk333/crystallizer/tree/main/source/LayoutHelpers), and add a new class that is named appropriately for the effect you're adding a layout for)
7. Connect UI controls to parameters using attachments (this is handled within 
the new layout class you just created, via the 
[AttachmentSetup](https://github.com/smonk333/crystallizer/tree/main/source/LayoutHelpers/ControlSetupHelpers/AttachmentSetup),
[LabelSetup](https://github.com/smonk333/crystallizer/tree/main/source/LayoutHelpers/ControlSetupHelpers/LabelSetup),
[SliderSetup](https://github.com/smonk333/crystallizer/tree/main/source/LayoutHelpers/ControlSetupHelpers/SliderSetup),
and [ToggleSetup](https://github.com/smonk333/crystallizer/tree/main/source/LayoutHelpers/ControlSetupHelpers/ToggleSetup) 
helper classes).