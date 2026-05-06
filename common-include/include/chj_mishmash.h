#ifndef __CHHI__chj_mishmash_h_
#define __CHHI__chj_mishmash_h_
#define __CHHI__chj_mishmash_h_created_ 20260503
#define __CHHI__chj_mishmash_h_updated_ 20260503

#include <ps_TCHAR.h>
#include <sdring.h>

bool binfile_is_match(const TCHAR *filename1, const TCHAR *filename2, int *pDiffAt=nullptr);


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


#if defined(chj_mishmash_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_chj_mishmash) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <fsapi.h>
#include <EnsureClnup_misc.h>
using namespace fsapi;

// <<< Include headers required by this lib's implementation




#ifndef chj_mishmash_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


bool binfile_is_match(const TCHAR *filename1, const TCHAR *filename2, int *pDiffAt)
{
	SETTLE_OUTPUT_PTR(int, pDiffAt, -1)

	filehandle_t fh1 = file_open(filename1, open_for_read, open_share_read|open_share_write);
	if (fh1 < 0)
		return false;

	filehandle_t fh2 = file_open(filename2, open_for_read, open_share_read|open_share_write);
	if (fh2 < 0)
		return false;

	CEC_filehandle_t cec_fh1 = fh1;
	CEC_filehandle_t cec_fh2 = fh2;

	int fsize1 = (int)file_getsize(fh1);
	if(fsize1<0)
		return false;

	int fsize2 = (int)file_getsize(fh2);
	if(fsize2<0)
		return false;

	sdring<char> s1(fsize1);
	sdring<char> s2(fsize2);

	file_read(fh1, s1.getptr(), fsize1);
	file_read(fh2, s2.getptr(), fsize2);

	return sdring<char>::str_match(s1.c_str(), s2.c_str(), pDiffAt);
}




#ifndef chj_mishmash_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
