#include "stdafx.h"

#include <gadgetlib/clipboard.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__clipboard__DLL_AUTO_EXPORT_STUB(void){}



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
		textchars = _tcslen(text);
	
	int textchars_ = textchars+1;

	HGLOBAL hmem = GlobalAlloc(GPTR, textchars_*sizeof(TCHAR));
	if(!hmem)
		return FALSE;
	
	TCHAR *pmem = (TCHAR*)GlobalLock(hmem);
	_tcsncpy(pmem, text, textchars_); 
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

	text_bytes = GlobalSize(hmem);

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
//	GlobalFree(hmem);
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
