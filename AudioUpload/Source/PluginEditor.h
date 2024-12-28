/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AudioUploadEditor : public juce::AudioProcessorEditor
{
public:
    AudioUploadEditor(AudioUploadProcessor&);
    ~AudioUploadEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioUploadProcessor& audioProcessor;

    juce::TextButton recordButton{ "Start Recording" };
    juce::TextButton stopButton{ "Stop Recording" };
    juce::TextButton uploadButton{ "Upload to Server" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioUploadEditor)
};