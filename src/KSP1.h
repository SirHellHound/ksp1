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

#ifndef KSP1_H_INCLUDED
#define KSP1_H_INCLUDED

#if KSP1_STANDALONE && KSP1_BUILD_LV2
 #error only one of these can be defined
#endif

#if HAVE_JUCE_CORE
 #include <juce/juce.h>
 #if KSP1_STANDALONE
  #include <element/element.h>
 #elif KSP1_BUILD_LV2
  #include <element/modules/config.h>
  #include <element/modules/element_base/element_base.h>
 #endif

#else
 #if KSP1_STANDALONE
  #include "../standalone/JuceLibraryCode/JuceHeader.h"
 #else
  #include "../jucer/JuceLibraryCode/JuceHeader.h"
 #endif
#endif

namespace KSP1
{
    using namespace Element;
    using Element::TimeScale;

    inline static int generateObjectID (int salt = 0)
    {
        Random r (salt == 0 ? Time::currentTimeMillis() : salt);

        static int lastGenerated = 0;
        if (lastGenerated == 0) {
            lastGenerated = r.nextInt (Range<int> (1, std::numeric_limits<int>::max()));
            return lastGenerated;
        }

        int newId = r.nextInt (Range<int> (1, std::numeric_limits<int>::max()));
        while (newId == lastGenerated) {
            newId = r.nextInt (Range<int> (1, std::numeric_limits<int>::max()));
        }
        lastGenerated = newId;
        return newId;
    }
}
#endif /* KSP1_H_INCLUDED */
