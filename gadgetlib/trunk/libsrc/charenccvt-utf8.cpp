#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <gadgetlib/charenccvt.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__charenccvt_utf8__DLL_AUTO_EXPORT_STUB(void){}

typedef unsigned int u32;

const int WCHAR_SIZE = sizeof(wchar_t);


int 
ggt_utf8_decode1char(const char *utf8s, int lim, u32 *pwc)
{
	// MEMO: G:\AlienBuild\mad-0.14.2b\libid3tag\utf8.c: id3_utf8_decodechar()
	
	// pwc: pointer to output wide-char.
	// lim(limit): only check for lim bytes for valid utf8 sequence.
	// return utf8 sequence length, 0 if sequence invalid.
	
	const unsigned char *start = (const unsigned char*)utf8s;
	const unsigned char *utf8  = (const unsigned char*)utf8s;

	assert(lim>0);

	if (lim>=1 && (utf8[0] & 0x80) == 0x00) 
	{
		*pwc = utf8[0];
		return utf8 - start + 1;
	}
	else if (lim>=2 &&
		(utf8[0] & 0xe0) == 0xc0 &&
		(utf8[1] & 0xc0) == 0x80) 
	{
		*pwc =
			((utf8[0] & 0x1f) << 6) |
			((utf8[1] & 0x3f) << 0);
		
//		if (*pwc >= 0x00000080L) // chj: for the abnormal but not-harmful case of (utf8[0] & 0x1f)==0, just let it go.
			return utf8 - start + 2;
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
			return utf8 - start + 3;
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
			return utf8 - start + 4;
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
			return utf8 - start + 5;
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
			return utf8 - start + 6;
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
	if(utf8s==NULL || utf8_bytes<-1 || wbuf_chars<0)
		return -1;
	
	int max_bytes = utf8_bytes;
	if(utf8_bytes==-1)
		max_bytes = strlen(utf8s);
	

//	int runs_all = 0;
//	int adv = 0;
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

