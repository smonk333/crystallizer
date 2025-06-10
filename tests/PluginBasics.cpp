#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("one is equal to one", "[dummy]")
{
    REQUIRE (1 == 1);
}

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("smoke!Crystallizer"));
    }
}


#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.0.0 (r0x131e93b0)"));
}
#endif

TEST_CASE ("Plugin construction and destruction", "[lifecycle]")
{
    REQUIRE_NOTHROW(PluginProcessor{});
}

TEST_CASE ("Parameter initialization", "[parameters]")
{
    PluginProcessor p;
    REQUIRE(p.delayTimeParam != nullptr);
    REQUIRE(p.feedbackParam != nullptr);
    REQUIRE(p.wetDryParam != nullptr);
    REQUIRE(p.reverbRoomSizeParam != nullptr);
    REQUIRE(p.reverbDampingParam != nullptr);
    REQUIRE(p.reverbMixParam != nullptr);
    REQUIRE(p.reverbWidthParam != nullptr);
    REQUIRE(p.reverbFreezeParam != nullptr);
    REQUIRE(p.granularDelayTimeParam != nullptr);
    REQUIRE(p.grainSizeParam != nullptr);
    REQUIRE(p.grainDensityParam != nullptr);
    REQUIRE(p.granularPitchShiftParam != nullptr);
    REQUIRE(p.granularFeedbackParam != nullptr);
    REQUIRE(p.granularWetDryParam != nullptr);
    REQUIRE(p.granularSpreadParam != nullptr);
    REQUIRE(p.looperStateParam != nullptr);
    REQUIRE(p.signalChainParam != nullptr);
}

TEST_CASE ("Parameter value ranges", "[parameters]")
{
    PluginProcessor p;
    // Example: delay time should be >= 0
    REQUIRE(*p.delayTimeParam >= 0.0f);
    REQUIRE(*p.feedbackParam >= 0.0f);
    REQUIRE(*p.feedbackParam <= 1.0f);
    REQUIRE(*p.wetDryParam >= 0.0f);
    REQUIRE(*p.wetDryParam <= 1.0f);
    // Add more as needed for other parameters
}

TEST_CASE ("Audio processing smoke test", "[processBlock]")
{
    PluginProcessor p;
    p.prepareToPlay(44100.0, 512);
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    juce::MidiBuffer midi;
    REQUIRE_NOTHROW(p.processBlock(buffer, midi));
    p.releaseResources();
}

TEST_CASE ("State save and restore", "[state]")
{
    PluginProcessor p;
    juce::MemoryBlock state;
    p.getStateInformation(state);
    PluginProcessor p2;
    REQUIRE_NOTHROW(p2.setStateInformation(state.getData(), (int)state.getSize()));
}

TEST_CASE ("MIDI and tail length", "[midi][tail]")
{
    PluginProcessor p;
    REQUIRE_FALSE(p.acceptsMidi());
    REQUIRE_FALSE(p.producesMidi());
    REQUIRE_FALSE(p.isMidiEffect());
    REQUIRE(p.getTailLengthSeconds() >= 0.0);
}

TEST_CASE ("Editor creation", "[editor]")
{
    PluginProcessor p;
    if (p.hasEditor())
    {
        auto* ed = p.createEditor();
        REQUIRE(ed != nullptr);
        delete ed;
    }
}

TEST_CASE ("Signal path management", "[signalPath]")
{
    PluginProcessor p;
    REQUIRE_NOTHROW(p.updateSignalPathManager(0));
    REQUIRE_NOTHROW(p.updateSignalPathManager(1));
}
