#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

AudioUploadProcessor::AudioUploadProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
}

AudioUploadProcessor::~AudioUploadProcessor() {}

//==============================================================================

const juce::String AudioUploadProcessor::getName() const
{
	return JucePlugin_Name;
}

bool AudioUploadProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool AudioUploadProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool AudioUploadProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double AudioUploadProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int AudioUploadProcessor::getNumPrograms()
{
	return 1;
}

int AudioUploadProcessor::getCurrentProgram()
{
	return 0;
}

void AudioUploadProcessor::setCurrentProgram(int index)
{
}

const juce::String AudioUploadProcessor::getProgramName(int index)
{
	return {};
}

void AudioUploadProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================

void AudioUploadProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	const int maxBufferSize = static_cast<int>(sampleRate * 30.0); // 30 seconds max recording time
	recordedBuffer.setSize(2, maxBufferSize); // Stereo buffer
	recordedSamples = 0;

	DBG("Buffer allocated with size: " + juce::String(maxBufferSize));
}

void AudioUploadProcessor::releaseResources()
{
	recordedBuffer.setSize(0, 0);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioUploadProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;

	return true;
#endif
}
#endif

void AudioUploadProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;

	// Ensure we don't exceed buffer size
	jassert(recordedSamples <= recordedBuffer.getNumSamples());

	if (isRecording)
	{
		const int bufferSamples = buffer.getNumSamples();
		const int remainingSpace = recordedBuffer.getNumSamples() - recordedSamples;

		if (remainingSpace >= bufferSamples)
		{
			for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
				recordedBuffer.copyFrom(channel, recordedSamples, buffer, channel, 0, bufferSamples);

			recordedSamples += bufferSamples;
		}
		else
		{
			DBG("Recording buffer full. Stopping recording.");
			stopRecording();
		}
	}
	else if (recordedSamples > recordedBuffer.getNumSamples())
	{
		DBG("Error: Recorded samples exceed buffer size. Resetting.");
		recordedSamples = recordedBuffer.getNumSamples(); // Reset to max valid samples
	}
}

//==============================================================================

bool AudioUploadProcessor::hasEditor() const
{
	return true;
}

juce::AudioProcessorEditor* AudioUploadProcessor::createEditor()
{
	return new AudioUploadEditor(*this);
}

//==============================================================================

void AudioUploadProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void AudioUploadProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

//==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new AudioUploadProcessor();
}

void AudioUploadProcessor::startRecording()
{
	juce::ScopedLock sl(recordLock);  // Thread synchronization for safe access to recorded buffer
	isRecording = true;
	recordedBuffer.clear();
	recordedSamples = 0;
	DBG("Recording started.");
}

void AudioUploadProcessor::stopRecording()
{
	juce::ScopedLock sl(recordLock);  // Thread synchronization
	isRecording = false;
	DBG("Recording stopped. Recorded samples: " + juce::String(recordedSamples));
}

void AudioUploadProcessor::uploadAudioToServer()
{
	juce::MemoryOutputStream outputStream;
	juce::WavAudioFormat wavFormat;

	auto writer = wavFormat.createWriterFor(
		&outputStream,
		getSampleRate(),
		recordedBuffer.getNumChannels(),
		16,
		{},
		0
	);

	if (writer)
	{
		writer->writeFromAudioSampleBuffer(recordedBuffer, 0, recordedSamples);
		//delete writer;

		juce::MemoryBlock postData(outputStream.getData(), outputStream.getDataSize());
		performAsyncUpload(postData);
	}
	else
	{
		DBG("Failed to create WAV writer.");
	}
}

void AudioUploadProcessor::performAsyncUpload(const juce::MemoryBlock& postData)
{
	juce::URL url("http://localhost:3000/upload");

	// Pass postData directly without using std::shared_ptr
	juce::Thread::launch([postData, url]()
		{
			DBG("Uploading audio...");
			auto stream = url.withPOSTData(postData).createInputStream(false);

			if (stream)
			{
				DBG("Audio uploaded successfully!");
			}
			else
			{
				DBG("Audio upload failed!");
			}
		});
}
