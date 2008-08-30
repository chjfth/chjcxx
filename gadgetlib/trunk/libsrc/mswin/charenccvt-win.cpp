#include <assert.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdefs.h>
#include <gadgetlib/charenccvt.h>


wchar_t 
ggt_mb2wc(const char *mb, int *pConBytes)
{
	wchar_t wc_out[2];
	int ConBytes = 1;

//	if( (mb[0]&0x80)==0 ) // danger for Shift-JIS? 
	if( (mb[0]&0x80)==0 || !IsDBCSLeadByteEx(CP_ACP, mb[0]))
		// Check (mb[0]&0x80) first to avoid system function call
	{
		ConBytes = 1;
		wc_out[0] = mb[0];
	}
	else
	{
		// On Windows, an MBCS char is at most 2 bytes (i.e. DBCS). 
		// UTF-8 is not actually considered MBCS.
		int wc = MultiByteToWideChar(CP_ACP, 0, mb, 2, wc_out, GetEleQuan_i(wc_out));
		assert(wc<=1);
		if(wc==1)
			ConBytes = 2;
		else if(wc==0)
			wc_out[0] = (wchar_t)-1;
	}

	if(pConBytes)
		*pConBytes = ConBytes;

	return wc_out[0];
}


