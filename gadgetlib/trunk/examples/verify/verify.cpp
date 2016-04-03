#include <stdio.h>
#include <ps_TCHAR.h>
#include <TScalableArray.h>

#include "share.h"

int casual_test()
{
	TScalableArray<int> sa;
	sa.SetEleQuan(10, true); // no use, just trying my .h-PDB-sewing
	return sa[1];
}


#ifdef _MSC_VER
int _tmain()
#else
int main()
#endif
{
	casual_test();
	
	verify_timefuncs();
	
	T_printf(_T("Verify success.\n"));
	return 0;
}
