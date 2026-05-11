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



////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////
