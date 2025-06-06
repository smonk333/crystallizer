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

    // granular delay parameters
    granularDelayTimeParam = apvts.getRawParameterValue("granularDelayTime");
    grainSizeParam = apvts.getRawParameterValue ("grainSize");
    grainDensityParam = apvts.getRawParameterValue ("grainDensity");
    granularPitchShiftParam = apvts.getRawParameterValue("pitchShift");
    granularFeedbackParam = apvts.getRawParameterValue("granularFeedback");
    granularWetDryParam = apvts.getRawParameterValue("granularWetDry");
    granularSpreadParam = apvts.getRawParameterValue("spread");

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
        "Processing Mode", juce::StringArray{"Delay Only", "Reverb Only", "Serial", "Granular Only (test)", "Parallel"}, 0));

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
    granularChain.prepare(spec);

    //=reset ProcessorChain objects=============================================
    reverbChain.reset();
    delayChain.reset();
    serialChain.reset();
    parallelChain.reset();
    granularChain.reset();

    // Allocate parallel processing buffers to avoid reallocating in the audio thread
    parallelDelayBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    parallelReverbBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    // Set up initial parameters for all processors
    updateDelayParameters(delayTimeParam->load(), feedbackParam->load(), wetDryParam->load());
    updateReverbParameters(reverbRoomSizeParam->load(), reverbDampingParam->load(),
                         reverbMixParam->load(), 1.0f - reverbMixParam->load(),
                         reverbWidthParam->load(), reverbFreezeParam->load());

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

    //=update delay processor parameters========================================

    updateDelayParameters(delayTimeParam->load(), feedbackParam->load(),
        wetDryParam->load());

    //=update reverb processor parameters=======================================

    updateReverbParameters(reverbRoomSizeParam->load(), reverbDampingParam->load(),
        reverbMixParam->load(), 1.0f - reverbMixParam->load(),
        reverbWidthParam->load(), reverbFreezeParam->load());

    //=update granular processor parameters=====================================

    updateGranularParameters(granularDelayTimeParam->load(),
        grainSizeParam->load(), grainDensityParam->load(),
        granularPitchShiftParam->load(), granularFeedbackParam->load(),
        granularWetDryParam->load(), granularSpreadParam->load());

    //=REGISTER MORE UPDATE BLOCKS HERE AS WE ADD PROCESSORS====================

    //=create audio block and context===========================================

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing context(block);

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

        case 3: // granular only (testing)
            granularChain.process(context);
            break;

        case 4: // parallel (delay alongside reverb)
        {
            // For parallel processing, we need to:
            // 1. Create a copy of the input
            // 2. Process one copy through delay and one through reverb
            // 3. Mix the results together

            // Ensure our pre-allocated buffers have the right size
            if (parallelDelayBuffer.getNumChannels() != buffer.getNumChannels() ||
                parallelDelayBuffer.getNumSamples() != buffer.getNumSamples())
            {
                parallelDelayBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
                parallelReverbBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
            }

            // Copy input to both buffers
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                parallelDelayBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
                parallelReverbBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
            }

            // Process delay buffer
            juce::dsp::AudioBlock<float> delayBlock(parallelDelayBuffer);
            juce::dsp::ProcessContextReplacing<float> delayContext(delayBlock);
            delayChain.process(delayContext);

            // Process reverb buffer
            juce::dsp::AudioBlock<float> reverbBlock(parallelReverbBuffer);
            juce::dsp::ProcessContextReplacing<float> reverbContext(reverbBlock);
            reverbChain.process(reverbContext);

            // Mix the processed signals back together (equal mix)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* outPtr = buffer.getWritePointer(channel);
                auto* delayPtr = parallelDelayBuffer.getReadPointer(channel);
                auto* reverbPtr = parallelReverbBuffer.getReadPointer(channel);

                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    // Simple 50/50 mix of delay and reverb signals
                    outPtr[sample] = 0.5f * delayPtr[sample] + 0.5f * reverbPtr[sample];
                }
            }
            break;
        }

        default:
            serialChain.process(context);
            break;
    }

}

//==============================================================================
void PluginProcessor::updateDelayParameters(float time, float feedback, float mix)
{
    // Update parameters in each processing chain that contains the delay processor
    serialChain.get<delayIndex>().updateParameters(time, feedback, mix);
    parallelChain.get<delayIndex>().updateParameters(time, feedback, mix);
    delayChain.get<0>().updateParameters(time, feedback, mix);
}

void PluginProcessor::updateReverbParameters(float roomSize, float damping, float wet,
                                           float dry, float width, float freeze)
{
    // Update parameters in each processing chain that contains the reverb processor
    serialChain.get<reverbIndex>().updateParameters(roomSize, damping, wet, dry, width, freeze);
    parallelChain.get<reverbIndex>().updateParameters(roomSize, damping, wet, dry, width, freeze);
    reverbChain.get<0>().updateParameters(roomSize, damping, wet, dry, width, freeze);
}

void PluginProcessor::updateGranularParameters(float delayTime, float grainSize,
    float grainDensity, float pitchShift, float feedback, float wetDry,
    float spread)
{
    // update parameters in the granular processor chain +
    // other chains that implement the granular processor
    granularChain.get<0>().updateParameters(
        delayTime,
        grainSize,
        grainDensity,
        pitchShift,
        feedback,
        wetDry,
        spread
    );
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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

