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

#ifndef HEADER_SIGNALS_SLOT_V0_HPP
#define HEADER_SIGNALS_SLOT_V0_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "slot_generic.hpp"
#include <list>

//: Slot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
class Slot_v0 : public Slot_Generic
{
public:
	//: Call
	virtual void call(class SlotParent_v0 &super)=0;
};

//: Slot Parent V0 Class
//- !group=Signal/System!
//- !header=signals.h!
class SlotParent_v0
{
public:
	typedef Slot_v0 *Slot;
	typedef std::list<Slot>::iterator SlotIterator;

//! Construction:
public:
	
	//: Slot Parent V0 Constructor
	SlotParent_v0(const SlotIterator &pos, const SlotIterator &end)
	: pos(pos), end(end)
	{
	}

//! Operations:
public:
	//: Operator
	void operator() ()
	{
		if (pos == end) return;

		SlotIterator next = pos;
		next++;

		if ((*pos)->get_slot_ref() > 0)
		{
			SlotParent_v0 super(next, end);
			(*pos)->call(super);
		}
	}

//! Implementation:
private:
	SlotIterator pos, end;
};

//: FunctionSlot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
class FunctionSlot_v0 : public Slot_v0
{
public:
	typedef void (*Callback)();
	typedef void (*VirtualCallback)(SlotParent_v0 &);

	//: FunctionSlot V0 Constructor
	FunctionSlot_v0(Callback callback, VirtualCallback virtual_callback)
	: callback(callback), virtual_callback(virtual_callback) { return; }

	//: Call
	virtual void call(SlotParent_v0 &super)
	{
		if (callback) { super(); callback(); }
		else virtual_callback(super);
	}

private:
	Callback callback;
	VirtualCallback virtual_callback;
};

//: FunctorSlot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
template<class Functor>
class FunctorSlot_v0 : public Slot_v0
{
public:
	//: FunctorSlot V0 Constructor
	FunctorSlot_v0(const Functor &functor)
	: functor(functor) { return; }

	//: Call
	virtual void call(SlotParent_v0 &super)
	{
		super();
		functor();
	}

private:
	Functor functor;
};

//: FunctorSlot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
template<class Functor>
class FunctorVirtualSlot_v0 : public Slot_v0
{
public:
	//: FunctorVirtualSlot V0 Constructor
	FunctorVirtualSlot_v0(const Functor &functor)
	: functor(functor) { return; }

	//: Call
	virtual void call(SlotParent_v0 &super)
	{
		functor(super);
	}

private:
	Functor functor;
};

//: Method Slot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
template <class CallbackClass>
class MethodSlot_v0 : public Slot_v0
{
public:
	typedef void (CallbackClass::*Callback)();
	typedef void (CallbackClass::*VirtualCallback)(SlotParent_v0 &);

	//: Method Slot V0 Constructor
	MethodSlot_v0(CallbackClass *cb_class, Callback callback, VirtualCallback virtual_callback)
	: cb_class(cb_class), callback(callback), virtual_callback(virtual_callback) { return; }

	//: Call
	virtual void call(SlotParent_v0 &super)
	{
		if (callback) { super(); (cb_class->*callback)(); }
		else (cb_class->*virtual_callback)(super);
	}

private:
	CallbackClass *cb_class;
	Callback callback;
	VirtualCallback virtual_callback;
};

//: User Data Methane Slot V0 Class
//- !group=Signal/System!
//- !header=signals.h!
template <class CallbackClass, class UserData>
class UserDataMethodSlot_v0 : public Slot_v0
{
public:
	typedef void (CallbackClass::*Callback)(UserData);
	typedef void (CallbackClass::*VirtualCallback)(SlotParent_v0 &, UserData);

	//: User Data Methane Slot V0 Constructor
	UserDataMethodSlot_v0(CallbackClass *cb_class, Callback callback, VirtualCallback virtual_callback, UserData user_data)
	: cb_class(cb_class), callback(callback), virtual_callback(virtual_callback), user_data(user_data) { return; }

	//: Call
	virtual void call(SlotParent_v0 &super)
	{
		if (callback) { super(); (cb_class->*callback)(user_data); }
		else (cb_class->*virtual_callback)(super, user_data);
	}

private:
	CallbackClass *cb_class;
	Callback callback;
	VirtualCallback virtual_callback;
	UserData user_data;
};

#endif
