#ifndef __CHHI__enumor_helper_h_
#define __CHHI__enumor_helper_h_
#define __CHHI__enumor_helper_h_created_ 20260428
#define __CHHI__enumor_helper_h_updated_ 20260520

//[2026-05-20] Tested with D:\gitw\trivials\chjds\test_enumor_helper\test_enumor_helper.cpp

////////////////////////////////////////////////////////////////////////////
namespace chjds { 
////////////////////////////////////////////////////////////////////////////


enum EnumorGo_et
{
	EnumorGo_End = 0,
	EnumorGo_First = 1,
	EnumorGo_SecondOrMore = 2,
};

enum EnumorLock_et
{
	EnumorLock_NoChange = 0,
	EnumorLock_On = 1,
	EnumorLock_Off = 2, 
};

struct enumor_helper_st
{
	// Three roles:
	// [EnUser] he who wants to enumerate the main object.
	// [Enumor] the object that provides .next(), .reset() to EnUser.
	// [EnHelper] this class, who maintains common states for any Enumor actions.

	int enum_outputs;
	bool is_enum_end;

	enumor_helper_st()
	{
		enum_outputs = 0;
		is_enum_end = false;
	}

	EnumorGo_et ui_next()
	{
		// ui: user come-in
		// Enumor.next() must call this at entrance, check ret-val to determine
		// whether to go on concrete enumerating action(walk through a linked-list etc).
		// > EnumorGo_End  : No concrete action needed, just tell EnUser this round is done.
		// > EnumorGo_First: First item is to be produced for EnUser.
		// > EnumorGo_SecondOrMore: Second or later item is to be produced for EnUser.

		if(is_enum_end)
		{
			return EnumorGo_End;

			// If EnUser wants to restart enumeration, he must call Enumor.reset() once,
			// then Enumor.next() .
		}
		else if(enum_outputs==0)
			return EnumorGo_First;
		else
			return EnumorGo_SecondOrMore;
	}

	EnumorLock_et ui_reset()
	{
		// ui: user come-in
		// Enumor.reset() must call this.

		bool was_in_progress = in_progress();

		enum_outputs = 0;
		is_enum_end = false;

		if(was_in_progress)
		{
			// The enum-progress is interrupted halfway, so Enumor can know 
			// to discard the enum-object-lock state.
			return EnumorLock_Off;
		}
		else
		{
			return EnumorLock_NoChange;
		}
	}

	EnumorLock_et uo_yes()
	{
		// uo: user go-out
		// Enumor calls this if he generates an output for EnUser.
		enum_outputs++;

		if(enum_outputs==1)
			return EnumorLock_On;
		else
			return EnumorLock_NoChange;
	}

	EnumorLock_et uo_end()
	{
		// uo: user go-out
		// Enumor calls this if he finds that enumeration has ended.
		bool was_in_progress = in_progress();
		is_enum_end = true;
		
		if(was_in_progress)
			return EnumorLock_Off; // First-time enum end.
		else
			return EnumorLock_NoChange; // Enumor calls uo_end() multiple-times
	}

	bool in_progress() const
	{
		// Enumor can call this anytime.
		// In-progress state implies Enumor should apply some "lock-object" to
		// avoid object/state changing so that further Enumor output is not distorted.

		if(enum_outputs>0 && !is_enum_end)
			return true;
		else
			return false;
	}

	int get_outputs() const 
	{
		return enum_outputs;
	}
};


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


#if defined(enumor_helper_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_enumor_helper) // [IMPL]


// empty for this file


#endif // [IMPL]


#endif // include once guard
