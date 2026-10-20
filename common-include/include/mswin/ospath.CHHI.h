// This is included by parent directory ospath.h .
// Do not compile this file alone.

// Last updated: 20260504

#include <windows.h>

#include <mswin/utils_env.h>

////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////


Sdring fullpath_from_rela(const TCHAR* rela)
{
	DWORD fulllen_ = GetFullPathName(rela, 0, NULL, NULL);
	Sdring fullpath(fulllen_);
	
	TCHAR *pfilenam = NULL;
	int ret = GetFullPathName(rela, fulllen_, fullpath.getptr(), &pfilenam);
	assert(ret==fulllen_-1);

	return fullpath;
}


const TCHAR* get_exe_filename()
{
	return GetExeFilename();
}

const TCHAR* get_exe_dir()
{
	return GetExeDir();
}

bool switch_to_exedir()
{
	return SwitchToExeDir();
}



////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////
