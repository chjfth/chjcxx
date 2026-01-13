#ifndef CHHI__utf8wchar_h_20260113_
#define CHHI__utf8wchar_h_20260113_

// Code from:
// D:\gitw\bookcode-mswin\__chjcxx\gadgetlib\libsrc\charenccvt-utf8.cpp
// with function name adjusts.

////////////////////////////////////////////////////////////////////////////
namespace utf8 { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


typedef unsigned int u32;

const int WCHAR_SIZE = sizeof(wchar_t);

int from_ucs4(u32 ucs4, char utf8seq[6]);

int from_wchars(const wchar_t *wcs, int wccount, char *utf8out, int utf8bufsize,
	int *pwcs_used=0);
	//!< Convert(encode) a wchar_t string into utf8 byte sequence. 
	/*!< 

	@param[in] wcs
		Input wchar_t string. This can be NUL-terminated or not, depending on wccount param.
		
		If unsupported wchar_t encountered(surrogate pair), the conversion stops, and *pwcs_used will tell 
		valid wchar_t's already processed.

	@param[in] wccount
		Tells the length of input wchar_t string.
		If -1, consider wcs as NUL-terminated, and the convertion result will be nul-terminated as well.
		If >=0, converting exactly that many wchar_t's.
		
	
	@param[out] utf8out 
		utf8 output buffer.
	
	@param[in] utf8bufsize
		utf8out's buffer size in bytes.
		If 0, utf8out[] will not be written to, and return value just tells how many bytes are required to
		hold the result, include possible one terminating nul char.
	
	@param[out] pwcs_used
		Report how many wchar_t's are consumed from wcs.
	
	return
		If utf8bufsize==0, return how any bytes are required to hold the encoding result.
		If utf8bufsize>0, return how many bytes are actually stored into utf8out.
		
		When invalid input parameter detected, return -1.
		
		Note: I will not store "partial" utf8 sequence into output buffer. For example, a wchar_t encodes to 
		threes utf8 bytes, and I will not store any of the three bytes if remaining output buffer is one or two.
		
		Note: utf8out is not guaranteed to be nul-terminated.

	Note on Windows: Currently this function can not cope with Unicode surrogate pair. 
	(TO IMPROVE LATER)
	*/

char* ptr_from_wchars(const wchar_t *wcs, int wccount, int *pwcs_used, int *pbytes_stored);


int to_ucs4(const char *utf8s, int lim, u32 *pwc);

int to_wchars(const char* utf8s, int utf8_bytes, wchar_t *wbuf, int wbuf_chars=0, 
	int *pbytes_used=0);
	//!< Convert(decode) a utf8 string into wchar_t string. 
	/*!< 

	@param[in] utf8s
		Input utf8 string. This can be nul-terminated or not, depending on utf8_bytes param.
		
		If invalid utf8 sequence found, the conversion stops, and *pbytes_used will tell 
		valid bytes already processed.

	@param[in] utf8_bytes
		Tells the length of input utf8 bytes.
		* If -1, consider utf8s as nul-terminated, and the conversion result will be NUL-terminated as well.
		* If >=0, converting exactly that many bytes. 
		
		Note: If invalid utf8 sequence is encountered before the desired end, conversion stops after
		converting the final valid sequence, and *pbytes_used will tell you where it stops.
		So, when utf8_bytes is -1, success is identified by (*pbytes_used>0 && utf8s[*pbytes_used-1]=='\0') .
		When utf8_bytes>=0, success is identified by (*pbytes_used==utf8_bytes) .
	
	@param[out] wbuf
		wchar_t output buffer.
		
	@param[in] wbuf_chars
		If 0, wbuf[] will not be written to, and return value just tells how many wchar_t's are required to 
		hold the result, including possible one terminating NUL.
		(If invalid utf8 sequence encountered, return value will only indicate for those valid utf8 sequences.)
		
		If >0, tells your buffer size, in wchar_t count.

	@param[out] pbytes_used (optional)
		Report how many bytes of valid utf8 sequence are consumed.

	return
		If wbuf_chars==0, return how many wchar_t are required to hold the decoding result.
		If wbuf_chars>0, return how many wchar_t are actually stored into wbuf.
		
		When invalid input parameter detected, return -1.
		
		Note: wbuf is not guaranteed to be NUL-terminated.
	
	Note on Windows: if the decoded wchar_t exceeds 0xFFFF, I'll consider the input utf8 sequence as invalid.
		
	*/

wchar_t* to_wchars_ptr(const char* utf8s, int utf8_bytes, int *pbytes_used, int *pwchars_stored);


void ptr_freebuf(void *ptr);


////////////////////////////////////////////////////////////////////////////
} // namespace utf8
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


#if defined(utf8wchar_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_utf8wchar) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef utf8wchar_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace utf8 { 
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


int 
from_ucs4(u32 ucs4, char utf8seq[6])
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
from_wchars(const wchar_t *wcs, int wccount, char *utf8out, int utf8bufsize,
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
		int utf8bytes = from_ucs4(wc, utf8seq);
		
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

char*
ptr_from_wchars(const wchar_t *wcs, int wccount, int *pwcs_used, int *pbytes_stored)
{
	// Calls from_wchars() internally. 
	// The returned pointer should be freed by calling utf8::ptr_freebuf() .

	int bytes_req1 = from_wchars(wcs, wccount, NULL, 0, pwcs_used);
	char *utf8buf = new char[bytes_req1];
	int bytes_req2 = from_wchars(wcs, wccount, utf8buf, bytes_req1, pwcs_used);
	
	assert(bytes_req1==bytes_req2);

	if(pbytes_stored)
		*pbytes_stored = bytes_req2;

	return utf8buf;
}


int 
to_ucs4(const char *utf8s, int lim, u32 *pwc)
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
to_wchars(const char* utf8s, int utf8_bytes, wchar_t *wbuf, int wbuf_chars, 
	int *pbytes_used)
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
		int runs = to_ucs4(utf8s_adv, remain_bytes, &wc);
		
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
		else // only count wchars decoded
		{
			++wcount;
		}
	}
	
	if(pbytes_used)
		*pbytes_used = (int)(utf8s_adv - utf8s);
	
	return wcount;
}

wchar_t* 
to_wchars_ptr(const char* utf8s, int utf8_bytes, int *pbytes_used, int *pwchars_stored)
{
	// Calls to_wchars() internally. 
	// The returned pointer should be freed by calling utf8::ptr_freebuf() .

	int wchars_req1 = to_wchars(utf8s, utf8_bytes, NULL, 0, pbytes_used);
	wchar_t *wcharsbuf = new wchar_t[wchars_req1];
	int wchars_req2 = to_wchars(utf8s, utf8_bytes, wcharsbuf, wchars_req1, pbytes_used);
	
	assert(wchars_req1==wchars_req2);

	if(pwchars_stored)
		*pwchars_stored = wchars_req2;

	return wcharsbuf;
}


void 
ptr_freebuf(void *ptr)
{
	delete ptr;
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
		int wcount = to_wchars(argv[i], -1, NULL, 0);
		wchar_t *pw = new wchar_t[wcount];
		int wcount2 = to_wchars(argv[i], -1, pw, wcount);
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



////////////////////////////////////////////////////////////////////////////
} // namespace utf8
////////////////////////////////////////////////////////////////////////////



#ifndef utf8wchar_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__utf8wchar_h_20260113_
