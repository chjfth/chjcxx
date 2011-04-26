#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <TScaWithLocalBuf.h>

#include <AnsiC_Tfuncs/AnsiC_Tfuncs.h>

#include <gadgetlib/charenccvt.h>

#include <gadgetlib/dirfuncs.h>

bool 
ggt_FreeDirfuncBuffer(TCHAR *p)
{
	if(!p)
		return true;

	//[2008-08-28] Sigh, ANSI C don't provide a function to verify the validity of p.

	free(p);
	return true;
}


const int c_fnbs_local = 256; // bs: buffer size
const char *s_badfill = "#";

TCHAR * 
ggt_RelPathToAbsPath(const TCHAR *rel, TCHAR *bufprev)
{
//	int rellen = T_strlen(rel);
//	int resultlen = 0;
//	TCHAR *ptmp = NULL, *pRet = NULL;
	const char *pRelToConvert = NULL;

#ifdef _UNICODE
	char fn_local[c_fnbs_local];
	TScalableArray<char> scaFn;

	int lenmbsRel_ = ggt_wcs2mbs_go(rel, -1); // check byte-length of the rel-path
	char *pmbsfn = Tscawlb_ReqOptBuf(fn_local, c_fnbs_local, scaFn, &lenmbsRel_);
	if(!pmbsfn)
		return NULL;

	int mbret = ggt_wcs2mbs_go(rel, -1, pmbsfn, lenmbsRel_, s_badfill);
	assert(mbret<=lenmbsRel_);
	pRelToConvert = scaFn;
#else
	pRelToConvert = rel;
#endif

	char *pszAbspath = realpath(pRelToConvert, NULL);
		// realpath() should malloc() a buffer for pszAbsPath .
	if(!pszAbspath)
		return NULL;

#ifdef _UNICODE
	int lenwcsAbs_ = ggt_mbs2wcs_go(pszAbspath, -1);
	wchar_t *pWcsAbs = (wchar_t*)malloc(sizeof(wchar_t)*lenwcsAbs_);
	if(!pWcsAbs)
	{
		free(pszAbspath); // release the half way allocated memory
		return NULL;
	}
	ggt_mbs2wcs_go(pszAbspath, -1, pWcsAbs, lenwcsAbs_);

	free(pszAbspath);
	if(bufprev)
		free(bufprev);

	return pWcsAbs;
#else
	if(bufprev)
		free(bufprev);

	return pszAbspath;
#endif

	// [2008-09-13]Note: _UNICODE case untested.
}
