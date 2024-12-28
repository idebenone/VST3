#pragma once

#include <JuceHeader.h>

class AudioUploadProcessor : public juce::AudioProcessor
{
public:
    AudioUploadProcessor();
    ~AudioUploadProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void startRecording();
    void stopRecording();
    void uploadAudioToServer();

private:
    juce::CriticalSection recordLock;
    bool isRecording = false;
    juce::AudioBuffer<float> recordedBuffer;
    int recordedSamples = 0;

    void performAsyncUpload(const juce::MemoryBlock& postData);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioUploadProcessor)
};
