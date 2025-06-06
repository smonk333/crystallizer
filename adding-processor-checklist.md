# checklist of things to do when adding a new processor to the plugin

- [ ] Add new processor class in AudioDSP folder (put the class in its own folder)
- [ ] Register and set processor order in ProcessorChain via the SignalPathManager class
- [ ] Define variables to be updated by the APVTS in the private section of the processor class header file
- [ ] Add atomic pointers for each parameter *from the new processor* in the PluginProcessor.h file
- [ ] Attach parameters from PluginProcessor.h to their respective slider/toggle/(whatever control scheme is necessary) in PluginEditor.cpp
- [ ] Add GUI controls (buttons, sliders) and layout using a new class under the ProcessorLayouts folder (create a new folder for the class to go in)
- [ ] Hook parameter values to processor behavior
- [ ] Test and confirm parameter state saving
