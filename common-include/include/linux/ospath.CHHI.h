// This is included by parent directory ospath.h .
// Do not compile this file alone.

// Last updated: 20260506

#ifndef _GNU_SOURCE
#define _GNU_SOURCE // [2026-05-06] Q: Even if I define this, get_current_dir_name() is still not available on Cygwin-gcc 13.4 Win7.
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h> // for PATH_MAX


////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////


Sdring fullpath_from_rela(const TCHAR* rela)
{
#if 0
	char* cwd = get_current_dir_name();
	if(!cwd)
		return Sdring();
	
	Sdrint sout = paths_join2(cwd, rela);
	
	free(cwd);
#endif

	Sdring sout;
	char cwd[4096] = "";
	
	const char *p_cwd = getcwd(cwd, sizeof(cwd));
	if(!p_cwd || cwd[0])
		sout = paths_join2(cwd, rela);

	return sout;
}

const TCHAR* get_exe_filename()
{
	static char s_exepath[PATH_MAX] = {};
	int nret = readlink("/proc/self/exe", s_exepath, PATH_MAX-1);
	return s_exepath;
}

const TCHAR* get_exe_dir()
{
	static char s_exepath[PATH_MAX] = {};
	static const char* s_pdir = nullptr;
	if (s_pdir)
		return s_pdir; // return the cached string

	int nret = readlink("/proc/self/exe", s_exepath, PATH_MAX-1);

	char *pLastSlash = strrchr(s_exepath, '/');
	if (pLastSlash)
	{
		*pLastSlash = '\0';
		s_pdir = s_exepath;
		return s_exepath;
	}
	else
		return "";
}

bool switch_to_exedir()
{
	const char *exedir = get_exe_dir();
	int err = chdir(exedir);
	return err ? false : true;
}




////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////
