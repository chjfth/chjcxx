#include <assert.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <commdefs.h>

#include <in_char_op.h>
#include <gadgetlib/charenccvt.h>


char *
ggt_charnext(const char *pchar)
{
	char *pret = CharNextExA(CP_ACP, pchar, 0);
	return pret;
}

char *
_ggt_charnext_on_illegal_lead_byte(const char *pchar)
{
	char *pret = CharNextExA(CP_ACP, pchar, 0);
	return pret;
}


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
		int wc = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, 
			mb, 2, wc_out, GetEleQuan_i(wc_out));
			// note: without MB_ERR_INVALID_CHARS flag, MultiByteToWideChar can't report illegal DBCS trail byte.
		assert(wc<=1);
		if(wc==1) // convert success, two bytes consumed
			ConBytes = 2;
		else if(wc==0)
		{
			ConBytes = 0;
			wc_out[0] = (wchar_t)-1;
		}
	}

	if(pConBytes)
		*pConBytes = ConBytes;

	return wc_out[0];
}

int 
ggt_wc2mb(wchar_t wc, char *pmb)
{
	BOOL isGotBad = 1;

	if(wc<0x80)
	{
		*pmb = (char)wc;
		return 1;
	}
	else
	{
		int mb = WideCharToMultiByte(CP_ACP, 0, 
			&wc, 1, pmb, ggt_max_mbcs_byteslen,
			NULL, &isGotBad);
		assert(mb>=1);
		
		if(isGotBad)
			return 0;
		else 
			return mb;
	}
}

