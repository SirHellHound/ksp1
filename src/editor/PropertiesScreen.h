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

#ifndef KSP1_KIT_SCREEN_H
#define KSP1_KIT_SCREEN_H

#include "editor/Screens.h"

namespace KSP1 {
namespace Gui {

    class KitScreen :  public Screen
    {
    public:
        KitScreen (SamplerDisplay& d);
        ~KitScreen();
        void keySelectedEvent (const KeyItem& key);
        Screen::ID type() const { return Screen::kitScreen; }
    };

}}

#endif /* KSP1_KIT_SCREEN_H */