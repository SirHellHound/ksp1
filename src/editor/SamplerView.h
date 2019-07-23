/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "PortWriter.h"
#include "editor/Keyboard.h"
#include "editor/LayersListBox.h"
#include "editor/LevelMeter.h"
#include "editor/Banks.h"
#include "editor/Panels.h"
//[/Headers]

#include "ArticulationControls.h"
#include "SamplerDisplay.h"


namespace KSP1 {

//==============================================================================
/**
                                                                    //[Comments]
    This is the main view used when the sampler is run as a tone module sampler.
    E.g.  No composer and pattern editing.   Just a sampler that plays Instruments
                                                                    //[/Comments]
*/
class SamplerView  : public Panel,
                     public DragAndDropContainer,
                     public Button::Listener,
                     public Slider::Listener,
                     public ComboBox::Listener
{
public:
    //==============================================================================
    SamplerView ();
    ~SamplerView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void setVolume (float db, bool notify = false);

    inline SamplerDisplay* getDisplay() const { return display; }

    inline int32 type() const { return Panel::samplerPanel; }

    LevelMeter* levelMeter() const;

    InstrumentPtr getInstrument (const int track = 0) const;
    void setInstrment (InstrumentPtr i);
    void setInterface (SamplerInterface* iface);

    // signal/callback handlers
    void loadFile (const File&);
    void onDisplayUpdate();
    void onKeyboardMidi (const MidiMessage& midi);
    void onKeySelected (int k);
    void buttonStateChanged (Button*);
    void stabilizeView();
    void setMainRMS (const float rmsL, const float rmsR);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SamplerInterface* interface;
    MidiKeyboardState keyboardState;

    //Globals& world;
    //SamplerEditor& editor;

    class Updater; friend class Updater;
    ScopedPointer<Updater> updater;

    ScopedPointer<AssetTree> tree;

    /** @internal load a program into the current instrument */
    void loadProgram (const Programming::Item&);
    /** @internal Called when a layer is chosen in the listbox */
    void layerChosen();
    /** @internal Update all of the controls in this view */
    void updateControls (NotificationType n = dontSendNotification);

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextButton> layerKeyButton;
    std::unique_ptr<GroupComponent> articulationGroup;
    std::unique_ptr<Label> volLabel;
    std::unique_ptr<Slider> layerVolume;
    std::unique_ptr<Slider> voiceGroup;
    std::unique_ptr<Label> layersLabel;
    std::unique_ptr<LayersListBox> layersListBox;
    std::unique_ptr<ComboBox> triggerMode;
    std::unique_ptr<LevelMeter> meter;
    std::unique_ptr<DecibelScaleComponent> dbScale;
    std::unique_ptr<Slider> volume;
    std::unique_ptr<Slider> layerPan;
    std::unique_ptr<Slider> layerPitch;
    std::unique_ptr<Slider> fxSend1;
    std::unique_ptr<Slider> fxSend2;
    std::unique_ptr<TabbedComponent> mediaTabs;
    std::unique_ptr<ArticulationControls> articulationControls;
    std::unique_ptr<Slider> fxSend3;
    std::unique_ptr<Slider> fxSend4;
    std::unique_ptr<Label> layerVolLabel;
    std::unique_ptr<Label> layerPanLabel;
    std::unique_ptr<Label> layerPitchLabel;
    std::unique_ptr<Label> fx1Label;
    std::unique_ptr<Label> fx1Label2;
    std::unique_ptr<Label> fx1Label3;
    std::unique_ptr<Label> fx1Label4;
    std::unique_ptr<KeyboardWidget> keyboard;
    std::unique_ptr<SamplerDisplay> display;
    std::unique_ptr<Slider> keyLength;
    std::unique_ptr<TextButton> propsButton;
    std::unique_ptr<TextButton> editButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerView)
};

} /* KSP1 */

//[EndFile] You can add extra defines here...

//[/EndFile]

