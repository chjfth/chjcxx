#ifndef __CHHI__fsapi_h_
#define __CHHI__fsapi_h_
#define __CHHI__fsapi_h_created_ 20260112
#define __CHHI__fsapi_h_updated_ 20260406


// Include OS headers to provide OS-specific data-types used in API prototype.
// But do not include Implementation-code depending headers.
// Example:
// <windows.h> provides DWORD
// <unistd.h> provides pid_t, off64_t

#ifdef _WIN32
# include <windows.h>
#else // consider it Linux
# include <unistd.h>
#endif

#include <ps_TCHAR.h>

//
// Check current compiler 
//

#ifdef _MSC_VER
// ...
#else // consider it GNU GCC
// ...
#endif


////////////////////////////////////////////////////////////////////////////
namespace fsapi { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

enum fserror_et
{
	// Error code here MUST be negative.
	
	E_success = 0,
	E_unknown = -1,
	E_bad_param = -2,

	E_not_found = -5,
	E_create = -6,
	E_access_denied = -7,

	E_disk_full = -10, // pending
};


enum open_for_et // can be OR-ed
{
	open_for_check = 0,
	open_for_read = 1,
	open_for_write = 2,
	open_for_append = 4,
};

enum open_share_et // can be OR-ed
{
	open_share_none = 0,
	open_share_read = 0x10,
	open_share_write = 0x20,
	open_share_both = open_share_read|open_share_write,
};

typedef __int64 filehandle_t;

enum { bad_filehandle = -1 }; // use it as invalid filehandle_t

filehandle_t file_open(const TCHAR *filepath, open_for_et ofor, open_share_et oshare);
// -- 
// On success, return a 64bit handle(always >=0).
// On fail, an fserror_et error code.
//
// The flag `open_for_write` implies creating the file if not exist yet.
// This function treats file content as binary stream, no idea of text-mode.

inline bool is_bad_filehandle(filehandle_t fh) { return fh<0; }

fserror_et file_close(filehandle_t fh);

int file_read(filehandle_t fh, void *rbuf, int bytes_to_read);
// -- On fail, return fserror_et code.

int file_write(filehandle_t fh, const void *wbuf, int bytes_to_write);
// -- On fail, return fserror_et code.

enum whence_et
{
	seek_set = 0,
	seek_cur = 1,
	seek_end = 2,
};

__int64 file_seek(filehandle_t fh, __int64 offset, whence_et whence);

fserror_et file_setsize(filehandle_t fh, __int64 newsize);
// -- On success, seek pointer is placed at the newsize.

__int64 file_getsize(filehandle_t fh);
// -- Return fserror_et on error.


bool file_exists(const TCHAR* inputpath);

bool dir_exists(const TCHAR* inputpath);

bool file_mark_readonly(const TCHAR* inputpath, bool is_readonly);

bool file_delete(const TCHAR* inputpath);

bool file_copy(const TCHAR* srcpath, const TCHAR *dstpath, bool is_overwrite);


////////////////////////////////////////////////////////////////////////////
} // namespace fsapi
////////////////////////////////////////////////////////////////////////////

#ifdef __CHHI__InterpretConst_h_
namespace itc
{
extern const CInterpretConst& fsapi_E_xxx();
}
#endif



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


#if defined(fsapi_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_fsapi) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation


#ifndef fsapi_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


#ifdef _MSC_VER 
// Using Microsoft Visual C++ as compiler
#include "mswin\fsapi.CHHI.h"
#else 
// Consider it Linux/GCC
#include "linux/fsapi.CHHI.h"
#endif



#ifdef __CHHI__InterpretConst_h_
namespace itc
{

const Enum2Val_st _e2v_fsapi_E_xxx[]=
{
	ITC_NAMEPAIR(fsapi::E_success),
	ITC_NAMEPAIR(fsapi::E_unknown),
	ITC_NAMEPAIR(fsapi::E_bad_param),
	
	ITC_NAMEPAIR(fsapi::E_not_found),
	ITC_NAMEPAIR(fsapi::E_create),
	ITC_NAMEPAIR(fsapi::E_access_denied),
	
	ITC_NAMEPAIR(fsapi::E_disk_full), // pending
};
ITC_MAKE_OBJECT(fsapi_E_xxx, _e2v_fsapi_E_xxx, ITCF_SINT);

}
#endif // __CHHI__InterpretConst_h_




#ifndef fsapi_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__fsapi_h_20260112_
