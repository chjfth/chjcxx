#ifndef __CHHI__util_Clipboard_h_
#define __CHHI__util_Clipboard_h_
#define __CHHI__util_Clipboard_h_created_ 20260711
#define __CHHI__util_Clipboard_h_updated_ 20260711


// [2026-07-11] Most code from 2017 D:\gitw\chjcxx\gadgetlib\libsrc\mswin\clipboard.cpp

#include <windows.h>

bool ggt_SetClipboardText(const TCHAR Text[], int textchars=-1, HWND hwnd=0);
/*
	Put Text to clipboard. 
	textchars tell how many characters to put, -1 means count by lstrlen.
	If clipboard open error, false will be returned.
*/

TCHAR *ggt_GetClipboardText(int *ptextchars=0, HWND hwnd=0);
/*
	Get text from clipboard.
	Return value points to the text(a copy), NUL-terminated.
	You should free this text buffer later by calling ggt_FreeClipboardText().
	Returning NULL means no text in clipboard currently.

	On return, *ptextchars tells the returned string length, in characters,
	not counting terminating NUL.
*/

bool ggt_FreeClipboardText(TCHAR *p);



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


#if defined(util_Clipboard_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_util_Clipboard) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef util_Clipboard_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


static BOOL 
openclipboard_with_timeout(DWORD millisec, HWND hwnd)
{
	DWORD msec_start = GetTickCount();
	do 
	{
		if(OpenClipboard(hwnd))
			return TRUE;
	}
	while( GetTickCount()-msec_start < millisec );
	return FALSE;
}

bool  
ggt_SetClipboardText(const TCHAR text[], int textchars, HWND hwnd)
{
	BOOL b = FALSE;
	HANDLE hret = NULL;

	if(textchars<0)
		textchars = (int)_tcslen(text);
	
	int textchars_ = textchars+1;

	HGLOBAL hmem = GlobalAlloc(GPTR, textchars_*sizeof(TCHAR));
	if(!hmem)
		return FALSE;
	
	TCHAR *pmem = (TCHAR*)GlobalLock(hmem);
	_tcsncpy_s(pmem, textchars_, text, textchars_); 
		//lstrcpyn(pmem, text, textchars_); // WinCE no this API
	pmem[textchars] = _T('\0');
	GlobalUnlock(hmem);

	if(!openclipboard_with_timeout(2000, hwnd)) {
		goto FAIL_FREE_HMEM;
	}

	b = EmptyClipboard();
	assert(b);

	hret = SetClipboardData(sizeof(TCHAR)==1?CF_TEXT:CF_UNICODETEXT, hmem);
	if(!hret) {
		goto FAIL_FREE_HMEM;
	}

	CloseClipboard();
	return TRUE;

FAIL_FREE_HMEM:
	CloseClipboard();
	GlobalFree(hmem);
	return FALSE;
}

TCHAR *  
ggt_GetClipboardText(int *ptextchars, HWND hwnd)
{
	// Note: User should call easyFreeClipboardText() to free the returning pointer.

	int text_bytes = 0;
	TCHAR *pmem = NULL; // ptr to Clipboard storage
	TCHAR *pret = NULL; // the pointer returned to user

	if(ptextchars)
		*ptextchars = 0;

	if(!openclipboard_with_timeout(2000, hwnd)) // + resource
		return NULL;

	HGLOBAL hmem = GetClipboardData(sizeof(TCHAR)==1?CF_TEXT:CF_UNICODETEXT);
	if(!hmem) {
		goto END;
	}

	pmem = (TCHAR*)GlobalLock(hmem);  // +resource
	if(!pmem) {
		goto END;
	}

	text_bytes = (int)GlobalSize(hmem);

	// Allocate returning user memory
	pret = (TCHAR*)GlobalAlloc(GPTR, text_bytes);
	if(!pret) {
		goto END;
	}

	memcpy(pret, pmem, text_bytes);	
	
	if(ptextchars)
		*ptextchars = text_bytes/sizeof(TCHAR)-1;

END:
	if(pmem)
		GlobalUnlock(pmem);

	CloseClipboard();
	return pret;
}

bool  
ggt_FreeClipboardText(TCHAR *p)
{
	if(!p)
		return FALSE;
	
	if(GlobalFree(p)==NULL) // yes, NULL means success
		return TRUE;
	else
		return FALSE;
}


#ifndef util_Clipboard_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
