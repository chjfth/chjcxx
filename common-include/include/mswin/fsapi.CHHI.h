// This is included by parent directory fsapi.h .
// Do not compile this file alone.


#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
//#include <intsafe.h> 

////////////////////////////////////////////////////////////////////////////
namespace fsapi { 
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.

static fserror_et winapi_xerr(DWORD winerr)
{
	switch(winerr)
	{
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
		return E_not_found;

	case ERROR_ACCESS_DENIED:
		return E_access_denied;

	}

	return E_unknown;
}

filehandle_t 
file_open(const TCHAR *filepath, open_for_et ofor, open_share_et oshare)
{
	DWORD dwDesiredAccess = 0;
	dwDesiredAccess |= (ofor & open_for_read) ? GENERIC_READ : 0;
	dwDesiredAccess |= (ofor & (open_for_write|open_for_append)) ? GENERIC_WRITE : 0;
	dwDesiredAccess |= (ofor & open_for_append) ? FILE_APPEND_DATA : 0;

	DWORD dwShareMode = 0;
	dwShareMode |= (oshare & open_share_read) ? FILE_SHARE_READ : 0;
	dwShareMode |= (oshare & open_share_write) ? FILE_SHARE_WRITE : 0;

	DWORD dwCreateHow = 0;
	if(ofor & (open_for_write|open_for_append))
		dwCreateHow = OPEN_ALWAYS;
	else
		dwCreateHow = OPEN_EXISTING;

	HANDLE hFile = CreateFile(filepath,
		dwDesiredAccess,
		dwShareMode, 
		NULL, // no security attribute
		dwCreateHow, // dwCreationDisposition
		0, // not FILE_FLAG_OVERLAPPED,
		NULL);
	
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		return (filehandle_t)hFile; // success
	}
	else
	{
		DWORD winerr = GetLastError();
//		if(winerr==ERROR_ALREADY_EXISTS && (dwCreateHow & OPEN_ALWAYS)) // will not get here
//			return (filehandle_t)hFile;

		return (filehandle_t)winapi_xerr(winerr);
	}
}


fserror_et 
file_close(filehandle_t fh)
{
	BOOL succ = CloseHandle((HANDLE)fh);
	if(succ)
		return E_success;
	else
		return winapi_xerr(GetLastError());
}

int // mux fserror_et
file_read(filehandle_t fh, void *rbuf, int bytes_to_read)
{
	if(bytes_to_read==0)
		return 0;
	else if(bytes_to_read<0)
		return E_bad_param;

	HANDLE hFile = (HANDLE)fh;
	DWORD bytesRed = 0;
	BOOL succ = ReadFile(hFile, rbuf, bytes_to_read, &bytesRed, NULL);
	if(succ)
		return bytesRed;
	else
		return winapi_xerr(GetLastError());
}


int // mux fserror_et
file_write(filehandle_t fh, const void *wbuf, int bytes_to_write)
{
	if(bytes_to_write==0)
		return 0;
	else if(bytes_to_write<0)
		return E_bad_param;

	HANDLE hFile = (HANDLE)fh;
	DWORD bytesWritten = 0;
	BOOL succ = WriteFile(hFile, wbuf, bytes_to_write, &bytesWritten, NULL);
	if(succ)
		return bytesWritten;
	else
		return winapi_xerr(GetLastError());
}


__int64 file_seek(filehandle_t fh, __int64 offset, whence_et whence)
{
	LARGE_INTEGER li = {}, lo = {};
	li.QuadPart = offset;

	HANDLE hFile = (HANDLE)fh;
	BOOL succ = SetFilePointerEx(hFile, li, &lo, whence);
	if(succ)
		return lo.QuadPart;
	else
		return winapi_xerr(GetLastError());
}

fserror_et 
file_setsize(filehandle_t fh, __int64 newsize)
{
	__int64 seekpos = file_seek(fh, newsize, seek_set);
	if(seekpos<0)
		return (fserror_et)seekpos;

	HANDLE hFile = (HANDLE)fh;
	BOOL succ = SetEndOfFile(hFile);
	if(succ)
		return E_success;
	else
		return winapi_xerr(GetLastError());
}



__int64 file_getsize(filehandle_t fh)
{
	LARGE_INTEGER li = {};
	
	HANDLE hFile = (HANDLE)fh;
	BOOL succ = GetFileSizeEx(hFile, &li);
	if(succ)
		return li.QuadPart;
	else
		return winapi_xerr(GetLastError());

	return 0;
}


bool file_exists(const TCHAR* inputpath)
{
	DWORD attrs = GetFileAttributes(inputpath);

	if (attrs == INVALID_FILE_ATTRIBUTES)
	{
		DWORD error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
			return false;
		}
		else if (error == ERROR_ACCESS_DENIED) {
			// [2026-05-04] Seems I cannot distinguish whether it is due to
			// some directory node denied or the file node denied, so consider it fail.
			return false;
		}
		else {
			return false;
		}
	}

	if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
		return false;
	}

	return true;
}


bool dir_exists(const TCHAR* inputpath)
{
	DWORD attrs = GetFileAttributes(inputpath);

	if (attrs == INVALID_FILE_ATTRIBUTES)
		return false;

	if (attrs & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	else
		return false;
}


bool file_mark_readonly(const TCHAR* inputpath, bool is_readonly)
{
	DWORD attr = GetFileAttributes(inputpath);
	if (attr == INVALID_FILE_ATTRIBUTES)
		return false;

	BOOL succ = FALSE;
	if (is_readonly)
		succ = SetFileAttributes(inputpath, attr | FILE_ATTRIBUTE_READONLY);
	else
		succ = SetFileAttributes(inputpath, attr & ~FILE_ATTRIBUTE_READONLY);

	return succ ? true : false;
}


bool file_delete(const TCHAR* inputpath)
{
	BOOL succ = DeleteFile(inputpath);
	if (succ)
		return true;
	else
		return false;
}


bool file_copy(const TCHAR* srcpath, const TCHAR *dstpath, bool is_overwrite)
{
	BOOL succ = CopyFile(srcpath, dstpath, !is_overwrite);
	return succ ? true : false;
}



////////////////////////////////////////////////////////////////////////////
} // namespace fsapi
////////////////////////////////////////////////////////////////////////////
