#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                     apvts (*this, nullptr,
                         "Parameters", createParams())
{
    // constructing parameter pointers

    // TODO: PROCESSOR_ADDITION_CHAIN(2): Assign apvts values for new parameters
    //       here for each new processor

    // standard delay parameters
    delayTimeParam = apvts.getRawParameterValue("delayTime");
    feedbackParam = apvts.getRawParameterValue("feedback");
    wetDryParam = apvts.getRawParameterValue("wetDry");

    // reverb parameters
    reverbRoomSizeParam = apvts.getRawParameterValue("reverbRoomSize");
    reverbDampingParam = apvts.getRawParameterValue("reverbDamping");
    reverbMixParam = apvts.getRawParameterValue("reverbMix");
    reverbWidthParam = apvts.getRawParameterValue("reverbWidth");
    reverbFreezeParam = apvts.getRawParameterValue("reverbFreeze");

    // granular delay parameters
    granularDelayTimeParam = apvts.getRawParameterValue("granularDelayTime");
    grainSizeParam = apvts.getRawParameterValue ("grainSize");
    grainDensityParam = apvts.getRawParameterValue ("grainDensity");
    granularPitchShiftParam = apvts.getRawParameterValue("pitchShift");
    granularFeedbackParam = apvts.getRawParameterValue("granularFeedback");
    granularWetDryParam = apvts.getRawParameterValue("granularWetDry");
    granularSpreadParam = apvts.getRawParameterValue("spread");

    // signal processing chain parameters
    signalChainParam = apvts.getRawParameterValue("processingMode");

    // looper state management parameter
    looperStateParam = apvts.getRawParameterValue("looperState");

    // signalPathManager parameter
    signalChainParam = apvts.getRawParameterValue("signalPath");

    // Initialize the signalPathListener
    signalPathListener = std::make_unique<SignalPathParameterListener>(*this);
    apvts.addParameterListener("signalPath", signalPathListener.get());
}

PluginProcessor::~PluginProcessor()
{
    // Remove the signalPathListener
    apvts.removeParameterListener("signalPath", signalPathListener.get());
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParams()
{

    // TODO: PROCESSOR_ADDITION_CHAIN(3): Add new parameters to the layout here
    //      for each new processor (ids are set in the processor's layout class)

    // create a vector of parameters
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // push standard delay parameters into the vector
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayTime",
        "Delay Time", 0.01f, 60.0f, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("feedback",
        "Feedback", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wetDry",
        "Wet/Dry Mix", 0.0f, 1.0f, 0.5f));

    // push reverb parameters into the vector
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbRoomSize",
        "Reverb Room Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbDamping",
        "Reverb Damping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbMix",
        "Reverb Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverbWidth",
        "Reverb Width", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("reverbFreeze",
        "Reverb Freeze Mode", false)); // binary toggle

    // TODO: PROCESSOR_ADDITION_CHAIN(18): Add the new processing mode here
    // push processing mode parameter into the vector
    params.push_back(std::make_unique<juce::AudioParameterChoice>("signalPath",
        "Processing Mode", juce::StringArray{
            "Delay Only",
            "Reverb Only",
            "Granular Only",
            "Looper Only",
            "Serial"}, 0));

    // push granular delay parameters into the vector
    params.push_back(std::make_unique<juce::AudioParameterFloat>("granularDelayTime",
        "Granular Delay Time", 0.01f, 5.0f, 0.5f));
    params.push_back( std::make_unique<juce::AudioParameterFloat>("grainSize",
        "Grain Size", 0.01f, 5.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("grainDensity",
        "Grain Density", 0.01f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pitchShift",
        "Pitch Shift", -4.0f, 4.0f, 1.0f)); // 1.0 = no shift))
    params.push_back(std::make_unique<juce::AudioParameterFloat>("granularFeedback",
        "Granular Feedback", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("granularWetDry",
        "Granular Wet/Dry Mix", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("spread",
        "Spread", 0.0f, 1.0f, 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("looperState",
            "Looper State", 0.0f, 4.0f, 0.0f)); // 0 = stopped, 1 = recording, 2 = playing, 3 = overdubbing, 4 = clear)

    // push more fx parameters here as we add classes to handle processing


    // return the vector of parameters
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // TODO: PROCESSOR_ADDITION_CHAIN(4): make sure the processor has been
    //       prepared either in its setup or perform the preparation here

    // set up a ProcessSpec object to prepare the standard delay, reverb
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // prepare the signal path manager for the current processing mode
    signalPathManager.setProcessingMode(
        static_cast<SignalPathManager::ProcessingMode>(
        static_cast<int>(signalChainParam->load())
        )
    );

    signalPathManager.prepare(spec);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
    }

    // TODO: PROCESSOR_ADDITION_CHAIN(5): add the processor to the processBlock,
    //       if necessary this will be refactored to use the new
    //       SignalPathManager, so these instructions may change

    signalPathManager.updateProcessorChainParameters(apvts);

    //=create audio block and context===========================================

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing context(block);

    //=select chosen processing chain===========================================

    // TODO: PROCESSOR_ADDITION_CHAIN(6) Add new options to this switch case to
    //       allow them to be selected from the GUI

    signalPathManager.process(context);

}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    // TODO: Implement Looper content saving
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // TODO: implement Looper content loading
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

void PluginProcessor::updateSignalPathManager(int newMode)
{
    signalPathManager.setProcessingMode(
        static_cast<SignalPathManager::ProcessingMode>(newMode)
    );
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
