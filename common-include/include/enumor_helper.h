#ifndef __CHHI__enumor_helper_h_
#define __CHHI__enumor_helper_h_
#define __CHHI__enumor_helper_h_created_ 20260428
#define __CHHI__enumor_helper_h_updated_ 20260428


////////////////////////////////////////////////////////////////////////////
namespace chjds { 
////////////////////////////////////////////////////////////////////////////


enum EH_GoOnAction_et
{
	TellEnd = 0,
	GoOnFirst = 1,
	GoOnMore = 2,
};

enum InProgress_et
{
	InProgress_TurnOff = 0,
	InProgress_TurnOn = 1,
	InProgress_NoChange = 2, 
};

struct enumor_helper_st
{
	// Three roles:
	// [User, who does enumerating action]
	// [Enumor] the object that provides .next(), .reset() to user.
	// [EHelper] this class, who maintain common states for any Enumor actions.

	int enum_outputs;
	bool is_enum_end;

	enumor_helper_st()
	{
		enum_outputs = 0;
		is_enum_end = false;
	}

	EH_GoOnAction_et ui_next()
	{
		// ui: user-input
		// Enumor.next() must call this at entrance, check ret-val to determine
		// whether to go on concrete enumerating action(walk through a linked-list etc).
		// > TellEnd: No concrete action needed, just tell user this round is done.
		// > GoOnFirst: First item is to be produced for user.
		// > GoOnMore:  Second or later item is to be produced for user.

		if(is_enum_end)
		{
			return TellEnd;

			// If user wants to restart enumeration, he must call Enumor.reset() once,
			// then Enumor.next() .
		}
		else if(enum_outputs==0)
			return GoOnFirst;
		else
			return GoOnMore;
	}

	InProgress_et ui_reset()
	{
		// Enumor.reset() must call this.

		int outputs_was = enum_outputs;
		enum_outputs = 0;
		is_enum_end = false;

		if(outputs_was==0)
			return InProgress_NoChange;
		else
		{
			// The enum-progress is interrupted halfway, so Enumor can know 
			// to discard the enum-object-lock state.
			return InProgress_TurnOff;
		}
	}

	InProgress_et uo_yes()
	{
		// uo: user-output
		// Enumor calls this if he generates an output for user.
		enum_outputs++;

		if(enum_outputs==1)
			return InProgress_TurnOn;
		else
			return InProgress_NoChange; // still in-progress
	}

	InProgress_et uo_end()
	{
		// uo: user-output
		// Enumor calls this if he finds that enumeration has ended.
		is_enum_end = true;
		
		if(enum_outputs==0)
			return InProgress_NoChange; // still no-progress
		else
			return InProgress_TurnOff;
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
