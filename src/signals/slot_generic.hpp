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

#ifndef HEADER_SIGNALS_SLOT_GENERIC
#define HEADER_SIGNALS_SLOT_GENERIC

#if _MSC_VER > 1000
#pragma once
#endif

#include <list>
#include "signal.hpp"

class Slot;

//: Slot Generic Class
//- !group=Signal/System!
//- !header=signals.h!
class Slot_Generic
{
//! Construction:
public:
	//: Slot Generic Constructor
	Slot_Generic();

	//: Slot Generic Destructor
	virtual ~Slot_Generic();

//! Attributes:
public:
	//: Get slot ref
	int get_slot_ref() const;

	//: Get signal ref
	int get_signal_ref() const;

	//: Returns true if persistent.
	//- <p>If the slot is persistent, it will remain connected to
	//- signals even when all references to Slot are gone.</p>
	bool is_persistent() const;

//! Operations:
public:
	//: Add slot ref
	void add_slot_ref();

	//: Release slot ref
	void release_slot_ref();

	//: Add signal ref
	void add_signal_ref(Signal *signal);

	//: Release signal ref
	void release_signal_ref(Signal *signal);

	//: Sets the persistent flag.
	//- <p>If the slot is persistent, it will remain connected to
	//- signals even when all references to Slot are gone.</p>
	void set_persistent(bool flag = true);

//! Implementation:
private:
	void check_delete();

	bool persistent;

	int slot_ref_count;

	int signal_ref_count;

	std::list<Signal *> connected_signals;
};

#endif
