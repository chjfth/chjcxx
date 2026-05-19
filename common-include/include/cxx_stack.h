#ifndef __CHHI__cxx_stack_h_
#define __CHHI__cxx_stack_h_
#define __CHHI__cxx_stack_h_created_ 20260506
#define __CHHI__cxx_stack_h_updated_ 20260506


#include <CxxVerCheck.h>
#ifdef _MSC_VER
#ifndef VC2015_OR_NEWER
#error This libcode needs VC2015 or newer. VC2010 has bug on move-assignment code generation.
#endif
#endif

#include <new>
#include <utility>
#include <assert.h>
#include <ps_TCHAR.h>
#include <commdefs.h>
#include <sdring.h>
#include <enumor_helper.h>
#include <TScalableArray.h>
#include <vaDbgTs_util.h>


////////////////////////////////////////////////////////////////////////////
namespace chjds { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

template<typename TU>
	// TU is user's datatype(should support C++-delete)
class stack
{
#ifdef CXX11_OR_NEWER
	static_assert(std::is_copy_constructible<TU>::value,
		"hashdict: TU must be copy constructible");

	static_assert(std::is_move_constructible<TU>::value,
		"hashdict: TU must be move constructible");
#endif

public:
	enum { E_Unknown=-1, E_Empty=-2, E_Locked=-3 };

public:
	int push(TU&& value); // return index of the new value in the stack

	int push(const TU& value)
	{
		TU copy = value;
		return push(std::move(copy));
	}

	int pop(TU& out_value); // return the index of the new value, -1 if fail.

	int clear();

	int count(){ return msa_data.CurrentEles(); }

	TU& operator[](int pos);

	bool SetTrait(int MaxEle, int IncSize, int DecSize)
	{
		auto err = msa_data.SetTrait(MaxEle, IncSize, DecSize);
		return err ? false : true;
	}


public:
	// boilerplate code, no need to modify >>>
	stack() { _ct0r(); }              //////////////
	virtual ~stack()                  //////////////
	{                                 //////////////
		_dtor();                      //////////////
		_ct0r();                      //////////////
	}                                 //////////////
	stack(const stack& old)           // copy-ctor
	{                                 //////////////
		_copy_from_old(old);          //////////////
	}                                 //////////////
	stack& operator=(const stack& old) // copy-assign
	{                                  //////////////
		if (this != &old) {            //////////////
			_dtor();                   //////////////
			_copy_from_old(old);       //////////////
		}                              //////////////
		return *this;                  //////////////
	}                                  //////////////
	stack(stack&& old)            // move-ctor
	{                             //////////////
		_steal_from_old(old);     //////////////
		old._ct0r();              //////////////
	}                             //////////////
	stack& operator=(stack&& old) // move-assign
	{                             //////////////
		if (this != &old) {       //////////////
			_dtor();              //////////////
			_steal_from_old(old); //////////////
			old._ct0r();          //////////////
		}                         //////////////
		return *this;             //////////////
	}                             //////////////
	// boilerplate code, no need to modify <<<

private:
	void _copy_from_old(const stack& old) {
		new(&msa_data) TU(old.msa_data); // to-test
	}

	void _steal_from_old(stack& old) {
		new(&msa_data) TU(std::move(old.msa_data)); // to-test
	}

public:
	// ... more public methods here ...
	class enumor
	{
	public:
		enumor(stack &stk, int from_idx=0)
			: m_stk(stk), m_from_idx(from_idx>=0?from_idx:0)
		{
			m_next_idx = -1; // casual ok
		}
		~enumor()
		{
			reset();
		}

		void reset()
		{
			if (m_eh.ui_reset() == InProgress_TurnOff)
				m_stk.m_enuming_sessions--;

			m_next_idx = -1;
		}

		int next(TU **ppValue);
		// -- Return value is the item index of the output TU, -1 on end. 
		// (Output) *ppValue points to stack-managed TU object, user must not `delete *ppValue`.

	private:
		stack& m_stk;
		enumor_helper_st m_eh;
		int m_from_idx;
		int m_next_idx;
	};

	enumor get_enumor(int from_idx=0)
	{
		return enumor(*this, from_idx);
	}


	//
	// Leave below at end of class body
	//
private: // data members
	TScalableArray<TU> msa_data;
	int m_enuming_sessions;

private:
	void _ct0r() {
		m_enuming_sessions = 0;
	}

	void _dtor() {
	}
};


template<typename TU>
int stack<TU>::push(TU&& value)
{
	if(m_enuming_sessions>0)
		return E_Locked;

	int count_bfr = msa_data.CurrentEles();
	int count_aft = count_bfr + 1;

	msa_data.SetEleQuan(count_aft, true);

	new(&msa_data[count_bfr]) TU(std::move(value));
	return count_bfr;
}


template<typename TU>
int stack<TU>::pop(TU& out_value)
{
	if(m_enuming_sessions>0)
		return E_Locked;

	int count = msa_data.CurrentEles();
	if(count<=0)
		return E_Empty;

	count--;
	out_value = std::move(msa_data[count]);

	msa_data.SetEleQuan(count);
	return count;
}


template<typename TU>
int stack<TU>::clear()
{
	if(m_enuming_sessions>0)
		return E_Locked;

	msa_data.SetEleQuan(0);
	return 0;
}

template<typename TU>
TU& stack<TU>::operator[](int pos)
{
	int count = msa_data.CurrentEles();
	if(pos>=0 && pos<count)
	{
		return msa_data[pos];
	}
	else if(pos<0 && pos>=-count)
	{
		// Python style, index from array tail
		return msa_data[count+pos];
	}
	else
	{
		// User can determine whether the element returned is valid
		// by checking whether its address is NULL.
		return *(TU*)0; 
	}
}


////////

template<typename TU>
int stack<TU>::enumor::next(TU **ppValue)
{
	// Note: Immediately before returning to user, we(Enumor code) should call
	// uo_yes() or uo_end() to tell enum_helper the result.

	EH_GoOnAction_et goact = m_eh.ui_next();
	if (goact == TellEnd)
	{
		m_eh.uo_end(); // optional for TellEnd.
		return -1;
	}
	else if (goact == GoOnFirst)
	{
		m_next_idx = m_from_idx;
	}
	else
	{
		assert(goact == GoOnMore);
		// Use m_next_idx from previous time
	}

	if (m_next_idx < m_stk.msa_data.CurrentEles())
	{
		// Still some TU to produce.

		*ppValue = &m_stk.msa_data[m_next_idx];
		m_next_idx++;

		if(m_eh.uo_yes() == InProgress_TurnOn)
			m_stk.m_enuming_sessions++;

		return m_next_idx-1;
	}
	else
	{
		// Nothing more to produce.

		if(m_eh.uo_end()==InProgress_TurnOff)
			m_stk.m_enuming_sessions--;

		return -1;
	}
}




////////////////////////////////////////////////////////////////////////////
} // namespace chjds
////////////////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++


#if defined(cxx_stack_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_cxx_stack) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef cxx_stack_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace chjds {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


// int getversion()
// {
// 	return 1;
// }


////////////////////////////////////////////////////////////////////////////
} // namespace chjds
////////////////////////////////////////////////////////////////////////////



#ifndef cxx_stack_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
