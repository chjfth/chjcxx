#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	//[2009-04-20] Currently, uBase does not have directory structure,
	//so we just return bufprev's content.

	int newlen = strlen(rel);

	TCHAR *pnewbuf = (TCHAR*)realloc(bufprev, newlen+1);
	if(pnewbuf)
	{
		strcpy(pnewbuf, rel);
		return pnewbuf;
	}
	else
		return NULL;
}
