#ifndef __CHHI__util_commdlg_h_
#define __CHHI__util_commdlg_h_
#define __CHHI__util_commdlg_h_created_ 20260710
#define __CHHI__util_commdlg_h_updated_ 20260710


#include <windows.h>

#include <sdring.h>


Sdrings util_GetOpenFilenames(HWND hwndOwner,
	const Sdring filters[], int nfilters, // format a bit different to OPENFILENAME.lpstrFilter
	const TCHAR *pInitDir = nullptr,
	const TCHAR *pDefaultFilename = nullptr);
// -- Example:
//		Sdring filters[] = { _T("Audio files;*.wav;*.mp3"), _T("All files;*.*") };
// -- Use a *semicolon* between [file type string] and [wildcard format].
//    On return, ssret[0] is absolute dirpath, each of ssret[1...] is filenam in that dirpath.

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


#if defined(util_commdlg_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_util_commdlg) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>
#include <snTcat.h>
#include <StringHelper.h>
#include <ospath.h>
// <<< Include headers required by this lib's implementation


#ifndef util_commdlg_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


Sdrings util_GetOpenFilenames(HWND hwndOwner,
	const Sdring filters[], int nfilters, // format a bit different to OPENFILENAME.lpstrFilter
	const TCHAR *pInitDir,
	const TCHAR *pDefaultFilename)
{
	// This function brings up system's Open-file dialog for user to select
	// *multiple* files.

	TCHAR outfiles[32000] = _T("");
	const int outbufsize = ARRAYSIZE(outfiles);

	OPENFILENAME ofn = {sizeof(ofn)};
	ofn.hwndOwner = hwndOwner;
	//
	ofn.lpstrFile = outfiles;
	ofn.nMaxFile = outbufsize;
	//
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_NOCHANGEDIR
		| OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		
	
	Sdring onesFilters;
	if(nfilters>0)
	{
		// A filter is sth like "Audio files;*.wav;*.mp3". 
		// To cater ofn.lpstrFilter's stupid format, we need to replace first ';' to '\0',
		// resulting in "Audio files\0*.wav;*.mp3"
		// Replace first occurrence of ';' to be '\0'.

		Sdrings ssFilters(nfilters); // need a copy bcz filters[] are const

		for(int i=0; i<nfilters; i++)
		{
			ssFilters[i] = filters[i];
			TCHAR *pSemicolon = (TCHAR*)_tcschr(ssFilters[i].c_str(), ';');
			if(pSemicolon)
				*pSemicolon = '\0';
		}

		// Then join.
		onesFilters = SdringsJoin(ssFilters, _T("\0"), 1, 1);
		int jlen = onesFilters.rawlen();
		
		assert(onesFilters[jlen-1]=='\0');
		assert(onesFilters[jlen]  =='\0');

		ofn.lpstrFilter = onesFilters.c_str();
	}

	ofn.lpstrInitialDir = pInitDir;

	if(pDefaultFilename)
	{ 
		// ofn.lpstrFile[] is in/out param
		snTcat(ofn.lpstrFile, ofn.nMaxFile, _T("%s"), pDefaultFilename);
	}

	BOOL succ = GetOpenFileName(&ofn);
	if(succ)
	{
		Sdrings ssret = ZZSplitToSdrings(ofn.lpstrFile);
		
		if(ssret.count()==1)
		{	// User selected only one file, so I split it into dirpath and filenam.
			Sdrings ss2(2);
			ss2[0] = ospath::split(ssret[0], ss2[1]);
			return ss2;
		}
		else
			return ssret;
	}
	else
	{
		return Sdrings();
	}
}




#ifndef util_commdlg_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
