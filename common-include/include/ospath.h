#ifndef __CHHI__ospath_h_
#define __CHHI__ospath_h_
#define __CHHI__ospath_h_created_ 20260418
#define __CHHI__ospath_h_updated_ 20260822

#include <ps_TCHAR.h>
#include <sdring.h>
#include <msvc_extras.h>
#include <commdefs.h>

////////////////////////////////////////////////////////////////////////////
namespace ospath { 
////////////////////////////////////////////////////////////////////////////

// Mimic Python os.path functions.

Sdring split(const TCHAR* inputpath, Sdring& outfilenam, TCHAR sepchar=0);
// -- dir part in return-value.
// sepchar: If 0, use os_sepchar, otherwise, should be '/' or '\' explicitly

inline Sdring split_dir(const TCHAR* inputpath, TCHAR sepchar=0)
{
	Sdring filenam;
	return split(inputpath, filenam);
}

inline Sdring split_filenam(const TCHAR* inputpath, TCHAR sepchar=0)
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

inline bool Is_split_got_root(const Sdring& indir)
{
	int len = indir.rawlen();
	if(len==1 && Is_pathsep(indir[0]))
		return true;
	else if(len==3 && Is_LetterColon(indir) && Is_pathsep(indir[2]))
		return true;
	else
		return false;
}


inline bool Is_winfullpath(const TCHAR *inpath)
{
	if(! (inpath && inpath[0]) )
		return false;
	
	if(_tcslen(inpath)>=3 
		&& Is_LetterColon(inpath) && Is_pathsep(inpath[2]))
		return true;
	else
		return false;
}

inline bool Is_fullpath(const TCHAR *inpath, bool *p_is_winstyle=nullptr)
{
	SETTLE_OUTPUT_PTR(bool, p_is_winstyle, false)

	if(! (inpath && inpath[0]) )
		return false;
	
	if(Is_pathsep(inpath[0]))
		return true;
	
	if(Is_winfullpath(inpath))
	{
		*p_is_winstyle = true;
		return true;
	}

	return false;
}


Sdring paths_join_sop(const TCHAR* const paths[], int npaths, TCHAR sepchar=0);
// -- sop: pure string operation; sepchar is typically '/' or '\'
// -- spechar is only for output Sdring; if 0, default to OS flavor.

inline Sdring paths_join2(const TCHAR* path1, const TCHAR* path2, TCHAR sepchar=0)
{
	TCHAR const* const paths[2] = {path1, path2};
	return paths_join_sop(paths, 2, sepchar);
}

inline Sdring paths_join3(const TCHAR* path1, const TCHAR* path2, const TCHAR* path3, TCHAR sepchar=0)
{
	TCHAR const* const paths[3] = { path1, path2, path3 };
	return paths_join_sop(paths, 3, sepchar);
}

inline Sdring path_normalize(const TCHAR* inpath, TCHAR sepchar=0)
{
	// This will remove redundant slashes, collapse midway ..\..\ .
	return paths_join2(_T(""), inpath, sepchar);
}


struct FTR_feedback_st
{
	int nparents; // how many ../../.. levels on output
	bool is_reach_root;
};

enum CaseSensitive_et 
{ 
	CaseSense_byOS = 0, // On Windows, not case-sensitive; on Linux, yes.
	CaseSense_yes = 1,
	CaseSense_no = 2,
};

Sdring fullpath_from_rela(const TCHAR* rela);
// -- Convert relative path to fullpath(absolute path).

Sdring fullpath_to_rela(const TCHAR *basedir, const TCHAR *tofullpath, 
	TCHAR sepchar=0, 
	FTR_feedback_st *pfeedback=nullptr, 
	CaseSensitive_et cas=CaseSense_byOS);
// -- Convert a fullpath to a possible relative-path(relapath).
// If user is in basedir, then returned relapath will refer to tofullpath.
// If no relative path representation is possible(different windows driver-letter etc), 
// it will return tofullpath verbatim.


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

#include <utility> // std::move()
#include <string.h>

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc

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


Sdring split(const TCHAR* inputpath, Sdring& outfilenam, TCHAR sepchar)
{
	if(!inputpath || !inputpath[0])
	{
		outfilenam = nullptr;
		return nullptr;
	}

	if(sepchar==0)
		sepchar = os_pathsep;

	const TCHAR *pFinalSlash = 
#ifdef CXX_TARGET_WINDOWS
	// Use WinAPI StrRChr so that it can deal with '\'(0x5c) in MBCS strings.
	// Example: GBK 0xd55c, 0xd65c, 0xd95c
	StrRChr(inputpath, NULL, sepchar);
#else
	strrchr(inputpath, sepchar);
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
			dirpart.setsn(inputpath, int(pFinalSlash-inputpath + 1));
		}
		else
			dirpart.setsn(inputpath, int(pFinalSlash-inputpath) );

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
		else if(Is_winfullpath(inpath))
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


inline bool is_A_prefix_of_B(const TCHAR *a, const TCHAR *b, 
	CaseSensitive_et cas=CaseSense_byOS)
{
	if(cas==CaseSense_byOS)
		cas = os_path_case_sensitive ? CaseSense_yes : CaseSense_no;

	int alen = (int)_tcslen(a);

	if(cas==CaseSense_yes)
	{
		int cmpret = _tcsncmp(a, b, alen);
		if(cmpret==0)
			return true;
		else
			return false;
	}
	else
	{
		assert(cas==CaseSense_no);

		int cmpret = shp_stricmp(a, b);
		if( cmpret==0 || cmpret==-(alen+1) )
			return true;
		else
			return false;
	}
}

Sdring fullpath_to_rela(const TCHAR *basedir, const TCHAR *tofullpath,  
	TCHAR sepchar, FTR_feedback_st *pfeedback, CaseSensitive_et cas)
{
	// This code is coarse now. To improve later.

	// [Example]
	//    basedir = D:\barn\x64\__Debug\_DigClock2
	// tofullpath = D:\barn\_data\winxp.wav
	//
	//     Return = ..\..\..\_data\winxp.wav
	//              pfeedback->nparents = 3
	//
	// If basedir and tofullpath is from different drive-letter, just return tofullpath.
	//
	// This function cannot NOT fail, worst case is to return tofullpath verbatim.

	if(sepchar==0)
		sepchar = os_pathsep;

	FTR_feedback_st odefault = {};
	SETTLE_OUTPUT_PTR(FTR_feedback_st, pfeedback, odefault);

	bool is_winstyle1 = false, is_winstyle2 = false;

	bool isfullpath1 = Is_fullpath(   basedir, &is_winstyle1);
	bool isfullpath2 = Is_fullpath(tofullpath, &is_winstyle2);
//	assert(isfullpath1);
//	assert(isfullpath2);
	if(!isfullpath1 || !isfullpath2)
		return nullptr;

	// Two fullpath styles must match to go.
	if(is_winstyle1 ^ is_winstyle2)
	{ 
		return Sdring(tofullpath);
	}

	Sdring basedir_norm = path_normalize(basedir, sepchar);
	Sdring tofullpath_norm = path_normalize(tofullpath, sepchar);
	basedir = basedir_norm;
	tofullpath = tofullpath_norm;

	// If tofullpath has basedir as prefix, do it easily.

	if(is_A_prefix_of_B(basedir, tofullpath, cas))
	{
		int baselen = (int)_tcslen(basedir);
		const TCHAR *pout = tofullpath + baselen;

		if(Is_pathsep(pout[0]))
		{
			pout++;
			return Sdring(pout);
		}
		else
		{	// A==B
			return Sdring(_T("."));
		}
	}
	else
	{
		// If winstyle drive-letter different, result is tofullpath.
		if (is_winstyle1 && is_winstyle2
			&& toupper(tofullpath[0]) != toupper(basedir[0]))
		{
			return Sdring(tofullpath);
		}
	}

	// Now we try: If some parent dir of basedir can lead to tofullpath.

	Sdring nowbase = basedir;
	int nParents = 0;

	for(;;)
	{
		Sdring nowtail;
		nowbase = split(nowbase, nowtail, sepchar);

		nParents++;

		if(Is_split_got_root(nowbase))
		{
			pfeedback->is_reach_root = true;
			break;
		}

		if (is_A_prefix_of_B(nowbase, tofullpath))
			break;
	}

	assert(nParents>0);
	
	Sdring srela;
	int i;
	for(i=0; i<nParents; i++)
	{
		srela.append_self(_T(".."));
		srela.append_self(&sepchar, 1);
	}

	int lenbase = (int)_tcslen(nowbase);

	assert(lenbase>0);

	if(tofullpath[lenbase] != '\0')
	{
		int extra_slash = 1;
		if( Is_pathsep(nowbase[lenbase-1]) )
		{
			// nowbase is like "/" or "d:/" .
			extra_slash = 0;
		}

		srela.append_self(tofullpath + lenbase + extra_slash);
	}
	else
	{	// Remove redundant trailing slash
		assert( srela[srela.rawlen()-1]==sepchar );
		srela.quick_shrink(1);
	}

	pfeedback->nparents = nParents;
	return srela;
}


////////////////////////////////////////////////////////////////////////////
} // namespace ospath
////////////////////////////////////////////////////////////////////////////



#ifndef ospath_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
