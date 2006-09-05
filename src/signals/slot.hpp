/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

//! clanSignal="System"
//! header=signals.h

#ifndef HEADER_SIGNALS_SLOT_HPP
#define HEADER_SIGNALS_SLOT_HPP

#if _MSC_VER > 1000
#pragma once
#endif

class Slot_Generic;

//: Slot class
//- !group=Signal/System!
//- !header=signals.h!
class Slot
{
//! Construction:
public:
	//: Create an unconnected slot.
	Slot();

	//: Copy a slot.
	Slot(const Slot &copy);

	//: Slot destructor
	~Slot();

//! Attributes:
public:
	//: Returns the number of Slot references still pointing at this slot.
	int get_slot_ref() const;

	//: Returns the number of signals this slot is connected to.
	int get_signal_ref() const;

	//: Returns true if slot is persistent.
	//- <p>If the slot is persistent, it will remain connected to
	//- signals even when all references to Slot are gone.</p>
	bool is_persistent() const;

//! Operations:
public:
	//: Slot copy operator
	void operator =(const Slot &copy);

	//: Sets the persistent flag.
	//- <p>If the slot is persistent, it will remain connected to
	//- signals even when all references to Slot are gone.</p>
	void set_persistent(bool flag = true);

//! Implementation:
public:
	//: Slot constructor
	Slot(Slot_Generic *impl);

	//: Slot Generic Implementation
	Slot_Generic *impl;
};

#endif
