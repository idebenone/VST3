/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioUploadEditor::AudioUploadEditor(AudioUploadProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    recordButton.onClick = [&]() { audioProcessor.startRecording(); };
    addAndMakeVisible(recordButton);

    // Stop button
    stopButton.onClick = [&]() { audioProcessor.stopRecording(); };
    addAndMakeVisible(stopButton);

    // Upload button
    uploadButton.onClick = [&]() { audioProcessor.uploadAudioToServer(); };
    addAndMakeVisible(uploadButton);
}

AudioUploadEditor::~AudioUploadEditor()
{
}

//==============================================================================
void AudioUploadEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
    g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioUploadEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    recordButton.setBounds(10, 40, 80, 30);
    stopButton.setBounds(110, 40, 80, 30);
    uploadButton.setBounds(210, 40, 80, 30);
}