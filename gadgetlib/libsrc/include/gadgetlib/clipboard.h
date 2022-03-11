#ifndef __easyclipboard_h_20150914_
#define __easyclipboard_h_20150914_

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


DLLEXPORT_gadgetlib
bool ggt_SetClipboardText(const TCHAR Text[], int textchars=-1, HWND hwnd=0);
/*
	Put Text to clipboard. 
	textchars tell how many characters to put, -1 means count by lstrlen.
*/

DLLEXPORT_gadgetlib
TCHAR *ggt_GetClipboardText(int *ptextchars=0, HWND hwnd=0);
	// Not tested yet in Keyview2.
/*
	Get text from clipboard.
	Return value points to the text(a copy), NUL-terminated.
	You should free this text buffer later by calling ggt_FreeClipboardText().
	Returning NULL means no text in clipboard currently.

	On return, *ptextchars tells the returned string length, in characters,
	not counting terminating NUL.
*/

DLLEXPORT_gadgetlib
bool ggt_FreeClipboardText(TCHAR *p);


#ifdef __cplusplus
} // extern"C"{
#endif

#endif
