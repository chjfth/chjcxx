#include <stdio.h>
#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>
#include <TScalableArray.h>

#include "share.h"

int casual_test()
{
	TScalableArray<int> sa;
	sa.SetEleQuan(10, true); // no use, just trying my .h-PDB-sewing
	return sa[1];
}

void test_printf()
{
	T_printf_wall(PrnNoTs, _T("T_printf_wall: PrnNoTs"));
	T_printf_wall(PrnSec, _T("T_printf_wall: PrnSec"));
	T_printf_wall(PrnMsec, _T("T_printf_wall: PrnMsec"));

	T_printfe_wall(PrnNoTs, _T("T_printfe_wall: PrnNoTs"));
	T_printfe_wall(PrnSec, _T("T_printfe_wall: PrnSec"));
	T_printfe_wall(PrnMsec, _T("T_printfe_wall: PrnMsec"));
}


void PrintEnv()
{
#ifdef _MSC_VER
#ifdef _WIN32_WINNT
	T_printf(_T("[User defined _WIN32_WINNT=%04X]\n"), _WIN32_WINNT);
#else
	T_printf(_T("[User does not define _WIN32_WINNT on compiling this program.]\n"));
#endif
#endif
}

#ifdef _MSC_VER
int _tmain()
#else
int main()
#endif
{
	PrintEnv();
	
	bool succ = false;

	casual_test();

	test_printf();

	verify_textcrlf();

	verify_timefuncs();

	succ = verify_ReposNewbox();
	assert(succ);

	succ = verify_CalSpeed();
	assert(succ);

	T_printf(_T("Verify success.\n"));
	return 0;
}
