/*
    PluginProcessor.h - This file is part of KSP1

    Copyright (C) 2013 Kushview, LLC  All rights reserved.
      * Michael Fisher <mfisher@kushview.net>
*/

#pragma once

#include "KSP1.h"

namespace KSP1 {

class PluginProcessor;
class PluginEditor;

class PluginWorld
{
public:
    PluginWorld();
    ~PluginWorld();

    void registerPlugin (PluginProcessor* plug);
    bool unregisterPlugin (PluginProcessor* plug);
    AudioProcessor* load (const String& uri);
    kv::WorkThread& getWorkThread();
    
private:
    friend class PluginProcessor;
    Array<PluginProcessor*> instances;
    ScopedPointer<kv::WorkThread> workThread;
    Array<File> factoryInstruments;
    
    void init();
};

class PluginProcessor  : public AudioProcessor,
                         private Timer
{
public:
    ~PluginProcessor();
    static PluginProcessor* create();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    void writeToPort (uint32 portIndex, uint32 bufferSize, uint32 portProtocol, const void* buffer);
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const String getName() const override;
    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;
    const String getParameterName (int index) override;
    const String getParameterText (int index) override;
    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void fillInPluginDescription (PluginDescription &description) const;
    void unregisterEditor (PluginEditor*);

protected:
    PluginProcessor();

private:
    std::unique_ptr<SamplerSynth> synth;
    
    bool useExternalData;
    ScopedPointer<kv::RingBuffer> ring, uiRing;
    HeapBlock<uint8> block;

    Array<PluginEditor*> editors;
    int currentProgram;
    
    friend class Timer;
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor);
};

}
