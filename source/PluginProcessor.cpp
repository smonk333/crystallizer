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

    // signal processing chain parameters
    processingModeParam = apvts.getRawParameterValue("processingMode");
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParams()
{
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

    // push processing mode parameter into the vector
    params.push_back(std::make_unique<juce::AudioParameterChoice>("processingMode",
        "Processing Mode", juce::StringArray{"Delay Only", "Reverb Only", "Serial", "Parallel"}, 0));

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

    // set up a ProcessSpec object to prepare the standard delay, reverb
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // commented out legacy processor logic
    // // prepare the standard delay processor
    // delay.prepare(spec);
    //
    // // prepare the reverb processor
    // reverb.prepare(spec);

    //=prepare ProcessorChain objects===========================================
    reverbChain.prepare(spec);
    delayChain.prepare(spec);
    serialChain.prepare(spec);
    parallelChain.prepare(spec);

    //=reset ProcessorChain objects=============================================
    reverbChain.reset();
    delayChain.reset();
    serialChain.reset();
    parallelChain.reset();

    // prepare more fx processors here as we add classes to handle processing
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

    // commented out legacy processing logic

//     // signal processing here
//     auto* delayLength = apvts.getRawParameterValue("delayTime");
//     auto* feedbackL = apvts.getRawParameterValue("feedback");
//     auto* feedbackR = apvts.getRawParameterValue("feedback");
//     auto* mix       = apvts.getRawParameterValue("wetDry");
//
//     float delayTime = *delayLength * getSampleRate(); // convert seconds to samples
//
//     // variables to read in the clean signal on both channels to use in wet/dry mixing
//     auto* cleanSignalL = new float[buffer.getNumSamples()];
//     auto* cleanSignalR = new float[buffer.getNumSamples()];
//
//     // grab a write pointer for both channels of audio
//     auto* channelDataL = buffer.getWritePointer(0);
//     auto* channelDataR = buffer.getWritePointer(1);
//
//     // store the clean signal before processing
//     for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
//     {
//         cleanSignalL[sample] = channelDataL[sample];
//         cleanSignalR[sample] = channelDataR[sample];
//     }
//
//     // add the delay processing to the output buffer
//     delay.process(
//         buffer, cleanSignalL, cleanSignalR,
//         delayTime,
//         feedbackL->load(),
//         feedbackR->load(),
//         mix->load()
//     );
//
// //==============================================================================
//     // THIS SECTION IS FOR PROCESSING THE SIGNAL IN SERIAL,
//     // WE DO NOT WANT TO KEEP THIS PERMANENTLY STUCK IN SERIAL
//     // TODO: IMPLEMENT LOGIC TO SWITCH BETWEEN PARALLEL PROCESSING AND SERIAL PROCESSING
//
//     // set up variables to hold the newly delayed audio for reverb processing
//     auto* delayedSignalL = new float[buffer.getNumSamples()];
//     auto* delayedSignalR = new float[buffer.getNumSamples()];
//
//     // copy input buffer to delayedSignal arrays for dry signal
//     auto* readL = buffer.getReadPointer(0);
//     auto* readR = buffer.getReadPointer(1);
//     for (int i = 0; i < buffer.getNumSamples(); ++i) {
//         delayedSignalL[i] = readL[i];
//         if (readR)
//             delayedSignalR[i] = readR[i];
//         else
//             delayedSignalR[i] = 0.0f;
//     }
//
//     // set up reverb parameters
//     auto* roomSize = apvts.getRawParameterValue("reverbRoomSize");
//     auto* damping = apvts.getRawParameterValue("reverbDamping");
//     auto* mixParam = apvts.getRawParameterValue("reverbMix");
//     auto* width = apvts.getRawParameterValue("reverbWidth");
//     auto* freeze = apvts.getRawParameterValue("reverbFreeze");
//
//     // combining the wet and dry parameter for ease of use
//     float mixed = mixParam->load();
//     float wetLevel = mixed;
//     float dryLevel = 1.0f - mixed;
//
//     // process reverb
//     reverb.process(
//         buffer, delayedSignalL, delayedSignalR,
//         roomSize->load(),
//         damping->load(),
//         wetLevel,
//         dryLevel,
//         width->load(),
//         freeze->load()
//     );
//
//     delete[] cleanSignalL;
//     delete[] cleanSignalR;
//
//     // clean up delayed signal buffers
//     delete[] delayedSignalL;
//     delete[] delayedSignalR;

    //=update delay processor parameters========================================

    updateDelayParameters(delayTimeParam->load(), feedbackParam->load(),
        wetDryParam->load());

    //=update reverb processor parameters=======================================

    updateReverbParameters(reverbRoomSizeParam->load(), reverbDampingParam->load(),
        reverbMixParam->load(), 1.0f - reverbMixParam->load(),
        reverbWidthParam->load(), reverbFreezeParam->load());

    //=create audio block and context===========================================

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    //=select chosen processing chain===========================================

    int mode = static_cast<int>(processingModeParam->load());

    switch (mode)
    {
        case 0: // delay only
            delayChain.process(context);
            break;

        case 1: // reverb only
            reverbChain.process(context);
            break;

        case 2: // serial (delay -> reverb)
            serialChain.process(context);
            break;

        case 3: // parallel (delay alongside reverb)
            parallelChain.process(context);
            break;

        default:
            serialChain.process(context);
            break;
    }


    parallelChain.process(context);

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
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
