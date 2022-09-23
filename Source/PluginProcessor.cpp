/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AtticAudioProcessor::AtticAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       // Parameters will be added using Value Tree State via std::make_unique..
                       ), treeState(*this, nullptr, juce::Identifier("PARAMETERS"),
                           { std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", 20.0f, 20000.0f, 20000.0f),
                           std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance", 0.0f, 1.0f, 0.1f),
                           std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 1.0f, 25.0f, 1.0f),
                           std::make_unique<juce::AudioParameterChoice>("mode", "Filter Type",
                           juce::StringArray("LPF12", "LPF24", "HPF12", "HPF24", "BPF12", "BPF24"), 0) })
#endif
{
    const juce::StringArray params = { "cutoff", "resonance", "drive", "mode" }; // Adds each parameter into a string array called 'params'..
    for (int i = 0; i <= 3; ++i)
    {
        // Adds a listener to each parameter in the array..
        treeState.addParameterListener(params[i], this);
    }

}

AtticAudioProcessor::~AtticAudioProcessor()
{
}

//==============================================================================
const juce::String AtticAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AtticAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AtticAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AtticAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AtticAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AtticAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs.
}

int AtticAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AtticAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AtticAudioProcessor::getProgramName (int index)
{
    return {};
}

void AtticAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AtticAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    ladderFilter.reset();
    ladderFilter.prepare(spec);
    ladderFilter.setEnabled(true);
}

void AtticAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AtticAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
#endif

void AtticAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    auto processingContext = juce::dsp::ProcessContextReplacing<float>(block);
    ladderFilter.process(processingContext);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
    }
}

//==============================================================================
bool AtticAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AtticAudioProcessor::createEditor()
{
    return new AtticAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void AtticAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AtticAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(treeState.state.getType()))
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AtticAudioProcessor();
}

// This function is called when a parameter is changed..
void AtticAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "cutOff")
        ladderFilter.setCutoffFrequencyHz(newValue);

    else if (parameterID == "resonance")
        ladderFilter.setResonance(newValue);

    else if (parameterID == "drive")
        ladderFilter.setDrive(newValue);

    else if (parameterID == "mode")
    {
        switch ((int)newValue)
        {
            case 0: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF12);
                break;
            case 1: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);
                break;
            case 2: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::HPF12);
                break;
            case 3: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::HPF24);
                break;
            case 4: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::BPF12);
                break;
            case 5: ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::BPF24);
                break;
        }
    }
}
