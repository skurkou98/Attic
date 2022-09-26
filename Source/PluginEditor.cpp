/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AtticAudioProcessorEditor::AtticAudioProcessorEditor (AtticAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), treeState(vts)
{
    // Make sure that before the constructor has finished, you've set the editor's size to whatever you need it to be..
    setSize (340, 280);

    // Cut-off Frequency
    cutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (treeState, "cutoff", cutoffDial);
    cutoffDial.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    cutoffDial.setRange(20.0f, 20000.0f, 0.0f);
    cutoffDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(&cutoffDial);

    // Resonance
    resonanceValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (treeState, "resonance", resonanceDial);
    resonanceDial.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceDial.setRange(0.0f, 1.20f, 0.0f);
    resonanceDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(&resonanceDial);

    // Drive
    driveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (treeState, "drive", driveDial);
    driveDial.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveDial.setRange(1.0f, 30.0f, 1.0f);
    driveDial.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    addAndMakeVisible(&driveDial);

    // Mode
    modeSel.addItem("LPF12", 1);
    modeSel.addItem("LPF24", 2);
    modeSel.addItem("HPF12", 3);
    modeSel.addItem("HPF24", 4);
    modeSel.addItem("BPF12", 5);
    modeSel.addItem("BPF24", 6);
    modeChoice = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        (treeState, "mode", modeSel);
    addAndMakeVisible(&modeSel);
}

AtticAudioProcessorEditor::~AtticAudioProcessorEditor()
{
}

//==============================================================================
void AtticAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::saddlebrown);
    g.setColour(juce::Colours::whitesmoke);
    // Header Text
    g.setFont(50);
    g.drawFittedText("ATTIC", 60, 30, 210, 10, juce::Justification::centred, 1, 0.0f);
    // Subheader Text
    g.setFont(15);
    g.drawFittedText("a ladder filter", 65, 60, 210, 10, juce::Justification::centred, 1, 0.0f);
    // Controls labels
    g.setFont(20);
    g.drawFittedText("C", 55, 145, 10, 10, juce::Justification::centred, 1, 0.0f);
    g.drawFittedText("R", 165, 145, 10, 10, juce::Justification::centred, 1, 0.0f);
    g.drawFittedText("D", 275, 145, 10, 10, juce::Justification::centred, 1, 0.0f);
}

void AtticAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any subcomponents in your editor..
    cutoffDial.setBounds(10, 100, 100, 100);
    resonanceDial.setBounds(120, 100, 100, 100);
    driveDial.setBounds(230, 100, 100, 100);
    modeSel.setBounds(130, 230, 75, 25);
}