#include "stdafx.h"

#include <gadgetlib/charenccvt.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__charenccvt_utf8__DLL_AUTO_EXPORT_STUB(void){}

typedef unsigned int u32;

const int WCHAR_SIZE = sizeof(wchar_t);


// MEMO: G:\AlienBuild\mad-0.14.2b\libid3tag\utf8.c: id3_utf8_decodechar() 
// provides code for ggt_utf8_decode1char() and ggt_utf8_encode_1char()
	
int 
ggt_utf8_encode_1char(u32 ucs4, char utf8seq[6])
{
	unsigned char *utf8 = (unsigned char*)utf8seq;
	
	if (ucs4 <= 0x0000007fL) {
		utf8[0] = ucs4;

		return 1;
	}
	else if (ucs4 <= 0x000007ffL) {
		utf8[0] = 0xc0 | ((ucs4 >>  6) & 0x1f);
		utf8[1] = 0x80 | ((ucs4 >>  0) & 0x3f);

		return 2;
	}
	else if (ucs4 <= 0x0000ffffL) {
		utf8[0] = 0xe0 | ((ucs4 >> 12) & 0x0f);
		utf8[1] = 0x80 | ((ucs4 >>  6) & 0x3f);
		utf8[2] = 0x80 | ((ucs4 >>  0) & 0x3f);

		return 3;
	}
	else if (ucs4 <= 0x001fffffL) {
		utf8[0] = 0xf0 | ((ucs4 >> 18) & 0x07);
		utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3f);
		utf8[2] = 0x80 | ((ucs4 >>  6) & 0x3f);
		utf8[3] = 0x80 | ((ucs4 >>  0) & 0x3f);

		return 4;
	}
	else if (ucs4 <= 0x03ffffffL) {
		utf8[0] = 0xf8 | ((ucs4 >> 24) & 0x03);
		utf8[1] = 0x80 | ((ucs4 >> 18) & 0x3f);
		utf8[2] = 0x80 | ((ucs4 >> 12) & 0x3f);
		utf8[3] = 0x80 | ((ucs4 >>  6) & 0x3f);
		utf8[4] = 0x80 | ((ucs4 >>  0) & 0x3f);

		return 5;
	}
	else if (ucs4 <= 0x7fffffffL) {
		utf8[0] = 0xfc | ((ucs4 >> 30) & 0x01);
		utf8[1] = 0x80 | ((ucs4 >> 24) & 0x3f);
		utf8[2] = 0x80 | ((ucs4 >> 18) & 0x3f);
		utf8[3] = 0x80 | ((ucs4 >> 12) & 0x3f);
		utf8[4] = 0x80 | ((ucs4 >>  6) & 0x3f);
		utf8[5] = 0x80 | ((ucs4 >>  0) & 0x3f);

		return 6;
	}
	else
		return 0;
}

int 
ggt_encode_to_utf8(const wchar_t *wcs, int wccount, char *utf8out, int utf8bufsize,
	int *pwcs_used)
{
	// wchar_t => UTF8
	
	if(wcs==NULL || wccount<-1 || utf8bufsize<0)
		return -1;
	
	int max_wchars = wccount;
	if(wccount==-1)
		max_wchars = (int)wcslen(wcs)+1;

	const wchar_t *wcs_adv = wcs;
	int remain_wchars = max_wchars;
	int bytes_stored = 0;
	
	while(remain_wchars>0)
	{
		u32 wc = *wcs_adv;
		char utf8seq[6];
		int utf8bytes = ggt_utf8_encode_1char(wc, utf8seq);
		
		if( utf8bufsize>0 && (bytes_stored+utf8bytes > utf8bufsize) )
			break;
		
		if(utf8bufsize>0)
		{
			memcpy(utf8out+bytes_stored, utf8seq, utf8bytes);
		}

		wcs_adv++;
		remain_wchars--;
		bytes_stored += utf8bytes;
	}

	if(pwcs_used)
		*pwcs_used = (int)(wcs_adv - wcs);
	return bytes_stored;
}

int 
ggt_utf8_decode1char(const char *utf8s, int lim, u32 *pwc)
{
	// pwc: pointer to output wide-char.
	// lim(limit): only check for lim bytes for valid utf8 sequence.
	// return utf8 sequence length, 0 if sequence invalid.
	
	const unsigned char *start = (const unsigned char*)utf8s;
	const unsigned char *utf8  = (const unsigned char*)utf8s;

	assert(lim>0);

	if (lim>=1 && (utf8[0] & 0x80) == 0x00) 
	{
		*pwc = utf8[0];
		return (int)(utf8 - start) + 1;
	}
	else if (lim>=2 &&
		(utf8[0] & 0xe0) == 0xc0 &&
		(utf8[1] & 0xc0) == 0x80) 
	{
		*pwc =
			((utf8[0] & 0x1f) << 6) |
			((utf8[1] & 0x3f) << 0);
		
//		if (*pwc >= 0x00000080L) // chj: for the abnormal but not-harmful case of (utf8[0] & 0x1f)==0, just let it go.
			return (int)(utf8 - start) + 2;
	}
	else if (lim>=3 &&
		(utf8[0] & 0xf0) == 0xe0 &&
		(utf8[1] & 0xc0) == 0x80 &&
		(utf8[2] & 0xc0) == 0x80) 
	{
			*pwc =
				((utf8[0] & 0x0fL) << 12) |
				((utf8[1] & 0x3fL) <<  6) |
				((utf8[2] & 0x3fL) <<  0);
//		if (*pwc >= 0x00000800L)
			return (int)(utf8 - start) + 3;
	}
	else if (lim>=4 &&
		(utf8[0] & 0xf8) == 0xf0 &&
		(utf8[1] & 0xc0) == 0x80 &&
		(utf8[2] & 0xc0) == 0x80 &&
		(utf8[3] & 0xc0) == 0x80) 
	{
		*pwc =
			((utf8[0] & 0x07L) << 18) |
			((utf8[1] & 0x3fL) << 12) |
			((utf8[2] & 0x3fL) <<  6) |
			((utf8[3] & 0x3fL) <<  0);
//		if (*pwc >= 0x00010000L)
			return (int)(utf8 - start) + 4;
	}
	else if (lim>=5 &&
		(utf8[0] & 0xfc) == 0xf8 &&
		(utf8[1] & 0xc0) == 0x80 &&
		(utf8[2] & 0xc0) == 0x80 &&
		(utf8[3] & 0xc0) == 0x80 &&
		(utf8[4] & 0xc0) == 0x80) 
	{
		*pwc =
			((utf8[0] & 0x03L) << 24) |
			((utf8[1] & 0x3fL) << 18) |
			((utf8[2] & 0x3fL) << 12) |
			((utf8[3] & 0x3fL) <<  6) |
			((utf8[4] & 0x3fL) <<  0);
//		if (*pwc >= 0x00200000L)
			return (int)(utf8 - start) + 5;
	}
	else if (lim>=6 &&
		(utf8[0] & 0xfe) == 0xfc &&
		(utf8[1] & 0xc0) == 0x80 &&
		(utf8[2] & 0xc0) == 0x80 &&
		(utf8[3] & 0xc0) == 0x80 &&
		(utf8[4] & 0xc0) == 0x80 &&
		(utf8[5] & 0xc0) == 0x80) 
	{
		*pwc =
			((utf8[0] & 0x01L) << 30) |
			((utf8[1] & 0x3fL) << 24) |
			((utf8[2] & 0x3fL) << 18) |
			((utf8[3] & 0x3fL) << 12) |
			((utf8[4] & 0x3fL) <<  6) |
			((utf8[5] & 0x3fL) <<  0);
//		if (*pwc >= 0x04000000L)
			return (int)(utf8 - start) + 6;
	}
	else 
	{
		*pwc = -1;
		return 0;
	}
}

int
ggt_decode_from_utf8(const char* utf8s, int utf8_bytes, wchar_t *wbuf, int wbuf_chars, 
	int *pbytes_run)
{
	// UTF8 => wchar_t
	
	if(utf8s==NULL || utf8_bytes<-1 || wbuf_chars<0)
		return -1;
	
	int max_bytes = utf8_bytes;
	if(utf8_bytes==-1)
		max_bytes = (int)strlen(utf8s)+1;
	
	u32 wc = 0;
	const char *utf8s_adv = utf8s;
	int remain_bytes = max_bytes;
	
	int wcount = 0;
	while(remain_bytes>0)
	{
		int runs = ggt_utf8_decode1char(utf8s_adv, remain_bytes, &wc);
		
		if(runs==0) // meet invalid sequence
			break;

		if(wc>=0x10000 && WCHAR_SIZE==2)
			break;

		utf8s_adv += runs;
		remain_bytes -= runs;
		
		if(wbuf_chars>0)
		{
			wbuf[wcount] = wc;
			
			if(++wcount==wbuf_chars)
				break;
		}
		else // only count wchars decodwd
		{
			++wcount;
		}
	}
	
	if(pbytes_run)
		*pbytes_run = (int)(utf8s_adv - utf8s);
	
	return wcount;
}


// ======================================================================

wchar_t **
ggt_argvs_utf8_to_wchars(int argc, char *argv[])
{
	// The return value should be free-ed by calling ggt_argvs_free_wchars()
	
	wchar_t **wargv = new wchar_t*[argc+1]; // one extra for end identification
	int i;
	for(i=0; i<argc; i++)
	{
		int wcount = ggt_decode_from_utf8(argv[i], -1, NULL, 0);
		wchar_t *pw = new wchar_t[wcount];
		int wcount2 = ggt_decode_from_utf8(argv[i], -1, pw, wcount);
		assert(wcount2==wcount);
		
		wargv[i] = pw;
	}
	wargv[argc] = NULL;
	return wargv;
}

void 
ggt_argvs_free_wchars(wchar_t **wargv)
{
	int i;
	for(i=0; ; i++)
	{
		if(wargv[i]==NULL)
			break; // meet end
		delete[] wargv[i];
	}
	delete[] wargv;
}




