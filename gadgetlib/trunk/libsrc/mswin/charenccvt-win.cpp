#include <stdio.h>
#include <gadgetlib/dirfuncs.h>

bool 
ggt_FreeDirfuncBuffer(TCHAR *p)
{
	if(!p)
		return true;

	free(p);
}



TCHAR * 
ggt_RelPathToAbsPath(const TCHAR *rel, TCHAR *bufprev=0)
{

}
