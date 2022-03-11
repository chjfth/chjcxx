#include "stdafx.h"

#include <gadgetlib/textcrlf.h>

int 
ggt_normalize_crlf(const TCHAR *ibuf, TCHAR *obuf, int obufchars, const TCHAR *szcrlf)
{
	// Return required characters to fill the result, mm_snprintf style return value.
	
	// Purpose: From a string with mixed \r, \n or \r\n, separate the lines 
	// with user defined szcrlf substring.

	// Behavior: 
	// * If a \n is encountered, conclude it a newline.
	// * If a \r is encountered, 
	//      * and if it is followed by a \n, consider this \r\n pair as a newline.
	//      * otherwise, this \r is a newline itself.

	const TCHAR *pi = ibuf; // in pointer
	TCHAR *po = obuf; // out pointer
	TCHAR *po_end0 = obuf+obufchars-1; // note: po_end0 may be 'negative' when obuf==obufchars==0
	int omove = 0;

	for(; *pi; )
	{
		TCHAR c = *pi;
		if(c!='\r' && c!='\n')
		{
			if( (int)(po_end0-po) > 0 ) 
				*po = c;

			po++;
			pi++;
			continue;
		}
		
		omove = mm_snprintf(po, (mmbufsize_t)(po_end0-po+1), _T("%s"), szcrlf);
		po += omove;

		if(c=='\n' || pi[1]!='\n')
		{
			pi++;
		}
		else // meet a \r\n
		{	assert(c=='\r' && pi[1]=='\n');
			pi+=2;
		}
	}

	if(obufchars>0)
	{
		if( (int)(po_end0-po) > 0 )
			*po = '\0';
		else
			*po_end0 = '\0';
	}

	return (int)(po - obuf);
}

