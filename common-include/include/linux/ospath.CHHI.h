// This is included by parent directory ospath.h .
// Do not compile this file alone.

// Last updated: 20260504

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>



////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////

bool file_exists(const TCHAR* inputpath)
{
	// Note: We always follow soft-links, so using stat() instead of lstat().
	struct stat buffer;

	int err = stat(inputpath, &buffer);   
	
	if(err)
	{
		// [2026-05-04] Seems I cannot distinguish whether it is due to
		// some directory node denied or the file node denied, so consider it fail,
		// even if we get EACCES.
		return false;
	}
	
	if(S_ISREG(buffer.st_mode)) // regular file
		return true;
	else // directory or pipe/sockets etc
		return false;
}


bool dir_exists(const TCHAR* inputpath)
{
	// Note: We always follow soft-links, so using stat() instead of lstat().
	struct stat buffer;

	int err = stat(inputpath, &buffer);   
	
	if(err)
	{
		// [2026-05-04] Seems I cannot distinguish whether it is due to
		// some directory node denied or the file node denied, so consider it fail,
		// even if we get EACCES.
		return false;
	}
	
	if(S_ISDIR(buffer.st_mode)) // a directory
		return true;
	else // file or pipe/sockets etc
		return false;
}


Sdring fullpath_from_rela(const TCHAR* rela)
{
	char* abspath = realpath(rela, nullptr);
	
	if (abspath) {
		Sdring result(abspath);
		free(abspath);  // Must use free(), not `delete`
		return result;
	}

	return Sdring();
}


bool file_mark_readonly(const TCHAR* inputpath, bool is_readonly)
{
	// Get current permissions
	struct stat st;
	if (stat(inputpath, &st) != 0) {
		return false;
	}
	
	mode_t newMode = 0;
	if(is_readonly)
	{
		// Remove write bits for all (owner, group, others)
		newMode = st.st_mode & ~(S_IWUSR | S_IWGRP | S_IWOTH);
	}
	else
	{
		// Turn on write bit only for current user.
		newMode = st.st_mode | S_IWUSR;
	}
	
	return chmod(inputpath, newMode) == 0;
}


bool file_delete(const TCHAR* inputpath)
{
	int err = unlink(inputpath);
	if(err)
		return false;
	else
		return true;
}


////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////
