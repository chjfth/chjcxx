#ifndef __CHHI__ospath_h_
#define __CHHI__ospath_h_
#define __CHHI__ospath_h_created_ 20260418
#define __CHHI__ospath_h_updated_ 20260418

#include <ps_TCHAR.h>
#include <sdring.h>

////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////

// Mimic Python os.path functions.

Sdring split(const TCHAR* inputpath, Sdring& outfilenam);
// -- dir part in return-value.

inline Sdring split_dir(const TCHAR* inputpath)
{
	Sdring filenam;
	return split(inputpath, filenam);
}

inline Sdring split_filenam(const TCHAR* inputpath)
{
	Sdring filenam;
	split(inputpath, filenam);
	return filenam;
}


////////////////////////////////////////////////////////////////////////////
} // namespace ospath
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


#if defined(ospath_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_ospath) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
//#include <snTprintf.h>

#ifdef _MSC_VER
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 
#endif

// <<< Include headers required by this lib's implementation



#ifndef ospath_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace ospath {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


Sdring split(const TCHAR* inputpath, Sdring& outfilenam)
{
	if(!inputpath || !inputpath[0])
	{
		outfilenam = nullptr;
		return nullptr;
	}

	const TCHAR *pfilenam = 
#ifdef _MSC_VER
		StrRChr(inputpath, NULL, '\\');
#else
		strrchr(inputpath, '\\');
#endif
	if (pfilenam)
	{
		// found a \ .
		pfilenam++;

		Sdring dirpart(inputpath, pfilenam-inputpath);

		if (*pfilenam) // some char after the \ .
		{
			outfilenam = Sdring(pfilenam);
		}
		else
		{	// \ is the trailing char, so filenam is empty
			outfilenam = nullptr;
		}

		return dirpart;
	}
	else
	{	// not found any \ 

		outfilenam = inputpath;
		return nullptr;
	}
}


////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////



#ifndef ospath_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
