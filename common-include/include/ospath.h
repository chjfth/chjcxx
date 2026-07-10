#ifndef __CHHI__ospath_h_
#define __CHHI__ospath_h_
#define __CHHI__ospath_h_created_ 20260418
#define __CHHI__ospath_h_updated_ 20260710

#include <ps_TCHAR.h>
#include <sdring.h>

////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////

// Mimic Python os.path functions.

Sdring split(const TCHAR* inputpath, Sdring& outfilenam);
// -- dir part in return-value.

inline Sdring split_dir(const TCHAR* inputpath)
{
	Sdring filenam;
	return split(inputpath, filenam);
}

inline Sdring split_filenam(const TCHAR* inputpath)
{
	Sdring filenam;
	split(inputpath, filenam);
	return filenam;
}

inline bool Is_pathsep(int charval)
{
	return (charval=='/' || charval=='\\') ? true : false;
}

inline bool Is_LetterColon(const TCHAR *inpath)
{
	// Checks whether inpath[] starts with "C:", "D:" etc, the Windows drive letter prefix.

	if(inpath && inpath[0])
	{
		if(inpath[1] == ':'
			&& (inpath[0]>='A' && inpath[0]<='Z' || inpath[0]>='a' && inpath[0]<='z')	
			)
			return true;
	}
	return false;
}

Sdring fullpath_from_rela(const TCHAR* rela);

Sdring paths_join_sop(const TCHAR* const paths[], int npaths, TCHAR sepchar=0);
// -- sop: pure string operation; sepchar is typically '/' or '\'
// -- spechar is only for output Sdring; if 0, default to OS flavor.

inline Sdring paths_join2(const TCHAR* path1, const TCHAR* path2, TCHAR sepchar=0)
{
	TCHAR const* const paths[2] = {path1, path2};
	return paths_join_sop(paths, 2, sepchar);
}

inline Sdring paths_join3(const TCHAR* path1, const TCHAR* path2, const TCHAR* path3, TCHAR sepchar = 0)
{
	TCHAR const* const paths[3] = { path1, path2, path3 };
	return paths_join_sop(paths, 3, sepchar);
}


////////////////////////////////////////////////////////////////////////////
} // namespace ospath
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


#if defined(ospath_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_ospath) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <CxxTargetCheck.h>
#ifdef CXX_TARGET_WINDOWS
#include "mswin\ospath.CHHI.h"
#else 
// Consider it Linux/GCC
#include "linux/ospath.CHHI.h"
#endif


#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <string.h>

#include <osdiff.h> // os_pathsep
#include <msvc_extras.h>
//#include <snTprintf.h>
#include <cxx_stack.h>
#include <StringHelper.h>
#include <osdiff.h>

#ifdef _MSC_VER
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib") 
#endif

// <<< Include headers required by this lib's implementation





#ifndef ospath_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace ospath {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


Sdring split(const TCHAR* inputpath, Sdring& outfilenam)
{
	if(!inputpath || !inputpath[0])
	{
		outfilenam = nullptr;
		return nullptr;
	}

	const TCHAR *pFinalSlash = 
#ifdef CXX_TARGET_WINDOWS
	// Use WinAPI StrRChr so that it can deal with '\'(0x5c) in MBCS strings.
	// Example: GBK 0xd55c, 0xd65c, 0xd95c
	StrRChr(inputpath, NULL, os_pathsep);
#else
	strrchr(inputpath, os_pathsep);
#endif
	if (pFinalSlash)
	{
		Sdring dirpart; // as return value

		// found a '/' or '\' (the slash)
		// Shall this slash appear at end of dirpart? Two cases.
		// [1] If the the dirpart is '/' or 'C:\' etc, the slash must be copied to dirpart.
		// [2] Other cases(non-root slash), the slash should be discarded.
		//
		// Python 3.7 example:
		//  In [42]: os.path.split(r'c:\abc.txt')
		//	Out[42]: ('c:\\', 'abc.txt')

		if(pFinalSlash==inputpath // the final slash is root-slash
			|| ( pFinalSlash-inputpath==2 && Is_LetterColon(inputpath) ) // the final slash is mswin-style root-slash
			) 
		{
			dirpart.setsn(inputpath, pFinalSlash-inputpath + 1);
		}
		else
			dirpart.setsn(inputpath, pFinalSlash-inputpath );

		outfilenam = Sdring(pFinalSlash+1);

		return dirpart;
	}
	else
	{	// not found any '\'
		outfilenam = inputpath;
		return nullptr;
	}
}

Sdring paths_join_sop(const TCHAR* const paths[], int npaths, TCHAR sepchar)
{
	if(npaths<=0)
		return Sdring();

	if(sepchar==0)
		sepchar = os_pathsep;

	// sepchar is typically '/' or '\'
	//
	// Root-lead-path can appear in any paths[X]
	// * If no root-lead-path appears in paths[], final-path will be considered a relative path,
	//   and leading ".."s will be preserved.
	// * If any root-lead-path appears in paths[], then the final-path will be / leading,
	//   adding extra ".." to "/" still stay at "/"

	// Note: this function does not cope with Windows drive letter abspath.
	// If you need to join 'C:\dir1' and 'dir2', please strip-off 'C:' first, and
	// pass in '\dir1' and 'dir2' instead.

	bool is_root_lead = false;
	TCHAR windrive_root[3] = _T(""); // could be "C:"

	chjds::stack<Sdring> stk; // path-node stack`

	int i;
	for(i=0; i<npaths; i++)
	{
		const TCHAR *inpath = paths[i];

		if (Is_pathsep(inpath[0]))
		{
			// inpath starts with '/', so start over from root-dir
			is_root_lead = true;
			stk.clear();
		}
		else if(_tcslen(inpath)>=3 
			&& Is_LetterColon(inpath) && Is_pathsep(inpath[2]))
		{
			// Special for Windows root-drive path like C:\dir
			is_root_lead = true;
			stk.clear();
			windrive_root[0]=inpath[0], windrive_root[1]=inpath[1];
			inpath += 2;
		}

		// Split one inpath into path-nodes and append them into stk.

		StringSplitter<const TCHAR*,
			Is_pathsep, StringSplitter_IsSpaceTab,
			false> // false: remove consecutive multiple \ and /
			spnode(inpath);

		for (;;)
		{
			int nodelen = 0;
			int pos = spnode.next(&nodelen);
			if(pos==-1)
				break;

			assert(nodelen>0);

			// If ".", which means no-dir-change, skip it.
			if(nodelen==1 && inpath[pos]=='.')
				continue; 

			bool isdotdot = (nodelen==2 && inpath[pos]=='.' && inpath[pos+1]=='.');

			if (!isdotdot)
			{
				stk.push(Sdring(inpath+pos, nodelen));
				continue;
			}

			// Now cope with ".." input:
			// [1] If some nodes already in stk:
			// [1A] * if previous node is not "..", we strip off that previous node.
			// [1B] * if previous node is "..", we push ".." .
			// [2] If no nodes in stk, 
			// [2A] * if not is_root_lead, we push "..".
			// [2B] * if is_root_lead, we discard this input "..".

			if(stk.count()>0)
			{ 
				const Sdring &prev = stk[-1];

				bool prevdotdot = (prev.rawlen()==2 && prev[0]=='.' && prev[1]=='.');

				if (!prevdotdot)
				{
					Sdring nouse;
					stk.pop(nouse);
				}
				else
				{
					stk.push(_T(".."));
				}
			}
			else // stk.count()==0
			{
				if (!is_root_lead)
				{
					stk.push(_T(".."));
				}
				else
				{
					// do nothing
				}
			}
		} // for(;;)
	}

	// Scan through stk, to collapse each ".."
	Sdring sout;
	if(is_root_lead)
	{ 
		if(windrive_root[0])
			sout.append_self(windrive_root);

		sout.append_self(&sepchar, 1);
	}

	int nodes = stk.count();
	for (i=0; i<nodes; i++)
	{
		sout.append_self( stk[i].c_str() );
		
		if(i<nodes-1)
			sout.append_self( &sepchar, 1 );
	}

	return sout;
}



////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////



#ifndef ospath_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
