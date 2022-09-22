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
class AtticAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AtticAudioProcessorEditor (AtticAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~AtticAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to access processor object that created it..
    AtticAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& treeState;

    // Initiation of User Interface controls here..
    juce::Slider cutoffDial;
    juce::Slider resonanceDial;
    juce::Slider driveDial;
    juce::ComboBox modeSel;

    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> cutoffValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> resonanceValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> driveValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtticAudioProcessorEditor)
};