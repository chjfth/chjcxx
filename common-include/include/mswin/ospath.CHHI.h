// This is included by parent directory ospath.h .
// Do not compile this file alone.

// Last updated: 20260504

#include <windows.h>

////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////


bool file_exists(const TCHAR* inputpath)
{
	DWORD attrs = GetFileAttributes(inputpath);
		
	if (attrs == INVALID_FILE_ATTRIBUTES) 
	{
		DWORD error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
			return false;
		} else if (error == ERROR_ACCESS_DENIED) {
			// [2026-05-04] Seems I cannot distinguish whether it is due to
			// some directory node denied or the file node denied, so consider it fail.
			return false;
		} else {
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


Sdring fullpath_from_rela(const TCHAR* rela)
{
	DWORD fulllen_ = GetFullPathName(rela, 0, NULL, NULL);
	Sdring fullpath(fulllen_);
	
	TCHAR *pfilenam = NULL;
	int ret = GetFullPathName(rela, fulllen_, fullpath.getptr(), &pfilenam);
	assert(ret==fulllen_-1);

	return fullpath;
}


bool file_mark_readonly(const TCHAR* inputpath, bool is_readonly)
{
	DWORD attr = GetFileAttributes(inputpath);
	if(attr==INVALID_FILE_ATTRIBUTES)
		return false;
	
	BOOL succ = FALSE;
	if(is_readonly)
		succ = SetFileAttributesW(inputpath, attr | FILE_ATTRIBUTE_READONLY);
	else
		succ = SetFileAttributesW(inputpath, attr & ~FILE_ATTRIBUTE_READONLY);
	
	return succ ? true : false;
}


bool file_delete(const TCHAR* inputpath)
{
	BOOL succ = DeleteFile(inputpath);
	if(succ)
		return true;
	else
		return false;
}



////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////
