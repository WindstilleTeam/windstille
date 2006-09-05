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

#ifndef HEADER_SIGNALS_SLOT_V4_HPP
#define HEADER_SIGNALS_SLOT_V4_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "slot_generic.hpp"

//: Slot V4 Class
//- !group=Signal/System!
//- !header=signals.h!
template <class PARAM1, class PARAM2, class PARAM3, class PARAM4>
class Slot_v4 : public Slot_Generic
{
public:
	//: Call
	virtual void call(PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4)=0;
};

//: FunctionSlot V4 Class
//- !group=Signal/System!
//- !header=signals.h!
template <class PARAM1, class PARAM2, class PARAM3, class PARAM4>
class FunctionSlot_v4 : public Slot_v4<PARAM1, PARAM2, PARAM3, PARAM4>
{
public:
	typedef void (*Callback)(PARAM1, PARAM2, PARAM3, PARAM4);

	//: FunctionSlot V4 Constructor
	FunctionSlot_v4(Callback callback) : callback(callback) { return; }

	//: Call
	virtual void call(PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) { callback(param1, param2, param3, param4); }

private:
	Callback callback;
};

//: FunctorSlot V4 Class
//- !group=Signal/System!
//- !header=signals.h!
template<class Functor, class PARAM1, class PARAM2, class PARAM3, class PARAM4>
class FunctorSlot_v4 : public Slot_v4<PARAM1, PARAM2, PARAM3, PARAM4>
{
public:
	//: FunctorSlot V4 Constructor
	FunctorSlot_v4(const Functor &functor)
	: functor(functor) { return; }

	//: Call
	virtual void call(PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) { functor(param1, param2, param3, param4); }

private:
	Functor functor;
};

//: Methane Slot V4 class
//- !group=Signal/System!
//- !header=signals.h!
template <class CallbackClass, class PARAM1, class PARAM2, class PARAM3, class PARAM4>
class MethodSlot_v4 : public Slot_v4<PARAM1, PARAM2, PARAM3, PARAM4>
{
public:
	typedef void (CallbackClass::*Callback)(PARAM1, PARAM2, PARAM3, PARAM4);

	//: Methane Slot V4 constructor
	MethodSlot_v4(CallbackClass *cb_class, Callback callback)
	: cb_class(cb_class), callback(callback) { return; }

	//: Call
	virtual void call(PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) { (cb_class->*callback)(param1, param2, param3, param4); }

private:
	CallbackClass *cb_class;
	Callback callback;
};

//: User Data Methane Slot V4 Class
//- !group=Signal/System!
//- !header=signals.h!
template <class CallbackClass, class PARAM1, class PARAM2, class PARAM3, class PARAM4, class UserData>
class UserDataMethodSlot_v4 : public Slot_v4<PARAM1, PARAM2, PARAM3, PARAM4>
{
public:
	typedef void (CallbackClass::*Callback)(PARAM1, PARAM2, PARAM3, PARAM4, UserData);

	//: User Data Methane Slot V4 Constructor
	UserDataMethodSlot_v4(CallbackClass *cb_class, Callback callback, UserData user_data)
	: cb_class(cb_class), callback(callback), user_data(user_data) { return; }

	//: Call
	virtual void call(PARAM1 param1, PARAM2 param2, PARAM3 param3, PARAM4 param4) { (cb_class->*callback)(param1, param2, param3, param4, user_data); }

private:
	CallbackClass *cb_class;
	Callback callback;
	UserData user_data;
};

#endif
