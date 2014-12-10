/*
    This file is part of KSP1
    Copyright (C) 2014  Kushview, LLC. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef KSP1_INSTRUMENT_H
#define KSP1_INSTRUMENT_H

#include "KSP1.h"
#include "Forge.h"
#include "MediaObject.h"
#include "InstrumentLoader.h"
#include "Programming.h"
#include "engine/SamplerKeys.h"

namespace KSP1 {

    static inline Identifier fxSendIdentifier (int sendNumber)
    {
        static const Identifier fxSendSlug      = "fxSend";
        String s (fxSendSlug);
        s << String("-") << String (sendNumber);
        return Identifier (s);
    }

    /** Writes all properties in a ValueTree into an LV2 Atom Object */
    struct PropertyWriter
    {
        typedef std::initializer_list<Identifier> Containment;

        PropertyWriter (Forge& f, const ValueTree& n, uint32_t otype,
                        uint32_t oid = 0,
                        uint32_t parent = 0,
                        const String& _ns = NS_SLUGS,
                        bool recurse = false)
            : ns(_ns), node (n), recursive (recurse),
              forge (f), objectType (otype),
              objectID (oid), parentID (parent)
        {
            slugUsedAsId = Slugs::id;
        }

        ForgeRef writeAtomObject()
        {
            ForgeFrame frame;
            ForgeRef r (forge.write_object (frame, objectID, objectType));

            if (0 != parentID) {
                forge.write_key (forge.uris.slugs_parent);
                forge.write_urid (parentID);
            }

            for (int i = node.getNumProperties(); --i >= 0;)
            {
                const Identifier slug (node.getPropertyName (i));

                if (slug == slugUsedAsId) {
                    continue;
                }

                String uri (ns); uri << slug.toString();
                const uint32 key = forge.map (uri.toRawUTF8());
                const var& value = node.getProperty (slug);

                if (key == 0) {
                    // invalid URID encountered
                    continue;
                }

                if (var::null == value) {
                    // nothing to do???
                    // lots of times a null var just means 0 or an empty string
                }

                // not in the white list
                if (whiteList.size() > 0 && ! whiteList.contains (slug)) {
                    continue;
                }

                if (value.isBool()) {
                    forge.write_key (key);
                    forge.write_bool ((bool) value);
                } else if (value.isInt()) {
                    forge.write_key (key);
                    forge.write_int ((int) value);
                } else if (value.isInt64()) {
                    forge.write_key (key);
                    forge.write_long ((int64) value);
                } else if (value.isDouble()) {
                    forge.write_key (key);
                    forge.write_double ((double) value);
                } else if (value.isString() && slug == Slugs::file) {
                    forge.write_key (key);
                    forge.write_path (value.toString().toRawUTF8());
                } else if (value.isString()) {
                    forge.write_key (key);
                    forge.write_string (value.toString().toRawUTF8());
                }
                else
                {

                }
            }

            forge.pop_frame (frame);
            return r;
        }

        /** Write an atom only allowing the specified properties
            @param props The property list to include when writing
            @return ForgeRef a reference to forge-written data

            @code
                PropertyWriter writer (...);
                Identifier p1 ("p1");
                Identifier p2 ("p2");
                writer.writeAtomRestrictedTo ({ p1, p2 });
            @endcode
         */
        ForgeRef writeAtomObjectIncluding (const Containment& props)
        {
            setWhiteList (props);
            return writeAtomObject();
        }

        void setWhiteList (const Containment& props)
        {
            whiteList.clearQuick();
            for (const Identifier& p : props) {
                whiteList.add (p);
            }
        }

        void setBlackList (const Containment& props)
        {
            blackList.clearQuick();
            for (const Identifier& p : props) {
                blackList.add (p);
            }
        }

        void setIdSlug (const Identifier& slug) {
            slugUsedAsId = slug;
        }

    protected:
        const String ns;
        const ValueTree node;
        const bool recursive;
        Forge& forge;
        const uint32_t objectID, objectType, parentID;
        Array<Identifier> whiteList, blackList;
        Identifier slugUsedAsId;
    };

    class SamplerInterface;
    class SamplerSynth;

    class LayerItem :  public ClipModel
    {
    public:

        LayerItem();
        LayerItem (const ValueTree& data);
        ~LayerItem();

        inline bool isEmpty() const { return ! objectData.hasProperty (Slugs::file); }
        inline bool isValid() const { return objectData.isValid() && objectData.hasType (Tags::layer)
                                        && (int32) objectData.getProperty(Slugs::index, -1) >= 0
                                        && (int32) objectData.getProperty (Slugs::note, -1) >= 0; }
        int32 index() const;
        int32 getNote() const;

        inline String fileString() const { return get (Slugs::file); }

        inline void setAtomObject (const URIs& uris, const lvtk::AtomObject& object)
        {
            setProperty (Slugs::id, static_cast<int> (object.id()));
            for (const auto& p : object)
            {
                const lvtk::Atom value (&p.value);
                setProperty (uris, p.key, value);
            }
        }

        inline void setProperty (const URIs& uris, uint32 prop, const lvtk::Atom& value)
        {
            if (prop == uris.slugs_index) {
                setProperty (Slugs::index, value.as_int());
            } else if (prop == uris.slugs_note) {
                setProperty (Slugs::note, value.as_int());
            } else if (prop == uris.slugs_volume) {
                setProperty (Slugs::volume, value.as_double());
            } else if (prop == uris.slugs_pitch) {
                setProperty (Slugs::pitch, value.as_double());
            } else if (prop == uris.slugs_start) {
                setProperty (Slugs::start, value.as_double());
            } else if (prop == uris.slugs_length) {
                setProperty (Slugs::length, value.as_double());
            } else if (prop == uris.slugs_offset) {
                setProperty (Slugs::offset, value.as_double());
            } else if (prop == uris.slugs_file) {
                const File file (String::fromUTF8 (value.as_string()));
                setFile (file);
            }
        }

        inline void
        setFile (const File& file)
        {
            set (Slugs::file, file.getFullPathName());
            set (Slugs::name, file.getFileNameWithoutExtension());
        }

        inline void setStartTime (double time)  { set (Slugs::start,  time); }
        inline void setEndTime (double time)    { set (Slugs::length, time - startTime()); }
        inline double startTime() const { return (double) get (Slugs::start); }
        inline double endTime()   const { return startTime() + (double)get(Slugs::length); }

        inline double gain() const
        {
            double vol = get (Slugs::volume);
            return Decibels::decibelsToGain (vol);
        }

        inline double panning()   const { return get (Tags::panning); }
        inline double pitch()     const { return get (Slugs::pitch); }
        inline double cutoff()    const { return get (Tags::cutoff);  }
        inline double resonance() const { return get (Tags::resonance); }

        inline void
        getVelocityRange (Range<double>& vel) const
        {
            vel.setStart (get (Tags::velocityLower));
            vel.setEnd (get (Tags::velocityUpper));
        }

        inline void
        setRange (const Range<double>& range)
        {
            setEndTime (range.getEnd());
            setStartTime (range.getStart());
        }

        inline void
        getRange (Range<double>& range) const
        {
            range.setStart (startTime());
            range.setEnd (endTime());
        }

        inline double duration() const
        {
            Range<double> r;
            getRange (r);
            return r.getLength();
        }

        inline ValueTree getParentValueTree() const {
            return objectData.getParent();
        }

        inline const var&
        getProperty (const Identifier& prop) const {
            return get (prop);
        }

        inline void
        setProperty (const Identifier& prop, const var& val)
        {
            set (prop, val);
        }

        inline LayerItem&
        operator= (const LayerItem& other)
        {
            objectData = other.node();
            return *this;
        }

        inline ForgeRef writeAtomObject (Forge& forge) const
        {
            const ValueTree parent = getParentValueTree();
            const uint32 oid = (uint32)(int) getProperty (Slugs::id);
            const uint32 pid = (uint32)(int) parent.getProperty (Slugs::id, 0);
            PropertyWriter writer (forge, objectData, forge.uris.ksp1_Layer, oid, pid);
            return writer.writeAtomObject();
        }

        inline ForgeRef writeAtomSubject (Forge& forge) const
        {
            const ValueTree parent = getParentValueTree();
            const uint32 oid = (uint32)(int) getProperty (Slugs::id);
            const uint32 pid = (uint32)(int) parent.getProperty (Slugs::id, 0);
            PropertyWriter writer (forge, objectData, forge.uris.ksp1_Layer, oid, pid);
            return writer.writeAtomObjectIncluding ({ Slugs::parent, Slugs::index });
        }

    private:
        const var& get (const Identifier& prop) const;
        void set (const Identifier& prop, const var& val);
        void setMissingProperties();
        void fixPropertyTypes();

        friend class KeyItem;
        friend class Instrument;
    };

    /** The model representation of a key on the sampler */
    class KeyItem :  public ObjectModel
    {
    public:

        explicit KeyItem (bool setMissing = true);
        KeyItem (const int note, const bool setMissing = true);
        KeyItem (const ValueTree& k);

        bool isValid()      const { return objectData.isValid(); }
        bool isNotValid()   const { return ! objectData.isValid(); }

        LayerItem addLayer (const File& file);
        LayerItem addLayer (const LayerItem& layer);
        LayerItem getLayer (int index) const;
        void removeLayer (int index);

        void clearLayers();
        int countLayers() const;

        double getVolume()     const { return (double) objectData.getProperty (Slugs::volume); }
        Value getVolumeValue()       { return objectData.getPropertyAsValue (Slugs::volume, nullptr); }

        int32 getTriggerMode() const { return (int32) objectData.getProperty (Tags::triggerMode); }
        Value getTriggerModeValue()  { return objectData.getPropertyAsValue (Tags::triggerMode, nullptr); }
        void setTriggerMode (const int t) { objectData.setProperty (Tags::triggerMode, t, 0); }

        int32 getVoiceGroup()  const { return (int32) objectData.getProperty (Tags::voiceGroup); }
        Value getVoiceGroupValue()   { return objectData.getPropertyAsValue (Tags::voiceGroup, nullptr); }

        int32 getNote()        const { return (int32) objectData.getProperty (Slugs::note, -1); }
        Value getNoteValue()         { return objectData.getPropertyAsValue (Slugs::note, nullptr); }

        KeyItem& operator= (const KeyItem& other);

        ForgeRef writeAtomObject (Forge& forge) const
        {
            const uint32 id = (uint32)(int) getProperty (Slugs::id, (int) 0);
            PropertyWriter writer (forge, objectData, forge.uris.ksp1_Key, id);
            return writer.writeAtomObject();
        }

        ForgeRef writeAtomSubject (Forge& forge) const
        {
            const uint32 id = (uint32)(int) getProperty (Slugs::id, (int) 0);
            PropertyWriter writer (forge, objectData, forge.uris.ksp1_Key, id);
            return writer.writeAtomObjectIncluding ({ Slugs::note });
        }

        void setAtomObject (const URIs& uris, const lvtk::AtomObject& object);
        void setProperty (const URIs& uris, uint32_t prop, const lvtk::Atom& value);

    private:

        int32 nlayers;

        void set (const Identifier& k, const var& v);
        void setAdsr (float a, float d, float s, float r);
        void setMissingProperties();

        friend class Instrument;
    };

    class Instrument :  public MediaObject
    {
    public:
        Instrument (const String& name);
        Instrument (const ValueTree& data);
        Instrument (const Instrument& other);

        ~Instrument();

        void clear();
        void setName (const String& name);
        String getName() const;
        void getSamples (StringArray& samples) const;

        KeyItem addKey (int32 noteNumber, int32 keySpan = 0);
        KeyItem getKey (int32 noteNumber, bool setMissing = false) const;
        KeyItem getOrCreateKey (int32 noteNumber);
        LayerItem getLayer (int32 key, int32 layer);

        XmlElement* createXml();
        void loadFromXml (const XmlElement& node);
        void load (const File& file, ProgressSink& sink);
        void load (const Programming::Item& file, ProgressSink& sink);

        inline float getVolume() const { return (float) node().getProperty (Slugs::volume, 0.0f); }
        inline void setVolume (float vol) { node().setProperty (Slugs::volume, vol, nullptr); }
        void setMissingProperties (bool recursive = true);

        Instrument& operator= (const Instrument& other);

        inline void selectNote (int note) {  selectedNote = note; }
        inline const int currentKeyId() const { return selectedNote; }

        inline void setFile (const File& f) { file = f; }
        inline File getFile() const { return file; }

        void updateLayerItem (const URIs& uris, const lvtk::AtomObject &object);
        void updateKeyItem (const URIs& uris, const lvtk::AtomObject &object);

        void print() {
            DBG (objectData.toXmlString());
        }

        void sortKeys();

    private:
        File file;
        friend class Pattern;
        int selectedNote;

    };

    typedef ReferenceCountedObjectPtr<Instrument> InstrumentPtr;
}

#endif /* KSP1_INSTRUMENT_H */
