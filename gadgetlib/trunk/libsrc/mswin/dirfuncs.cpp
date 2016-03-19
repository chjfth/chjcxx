#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <gadgetlib/dirfuncs.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__dirfuncs__DLL_AUTO_EXPORT_STUB(void){}


bool 
ggt_FreeDirfuncBuffer(TCHAR *p)
{
	if(!p)
		return true;

	//[2008-08-28] Sigh, ANSI C don't provide a function to verify the validity of p.

	free(p);
	return true;
}



TCHAR * 
ggt_RelPathToAbsPath(const TCHAR *rel, TCHAR *bufprev)
{
	int rellen = T_strlen(rel);
	int resultlen = 0;
	TCHAR *ptmp = NULL, *pRet = NULL;

#ifdef WINCE
	// Damn WinCE(until 6.0) don't support the concept of "process current directory".
	// So I just copy rel[] as result. 
	
	resultlen = rellen;

	ptmp = (TCHAR*)realloc(bufprev, (resultlen+1)*sizeof(TCHAR));
	if(ptmp)
		pRet = ptmp;
	else
		return NULL;

	T_strcpy(pRet, rel);

#else
	TCHAR *pFilePart; 

	resultlen = GetFullPathName(rel, 0, NULL, &pFilePart);
	
	ptmp = (TCHAR*)realloc(bufprev, (resultlen+1)*sizeof(TCHAR));
	if(ptmp)
		pRet = ptmp;
	else
		return NULL;

	GetFullPathName(rel, resultlen+1, pRet, &pFilePart);

#endif

	return pRet;
}
