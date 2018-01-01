#define CALSPEED_IMPL
#include <CalSpeed.h> // from common-include
#include <gadgetlib/T_string.h>

#include "share.h"

struct VCalSpeed_st
{
	Uint diffmsec;
	Uint64 diffbytes;
	const TCHAR *result;
};

static VCalSpeed_st sar_cases[] =
{
	{    0, 3, _T("") },
	{  500, 0, _T("0 B/s") },
	{ 2000, 1, _T("0 B/s") },
	{ 1000, 1, _T("1 B/s") },
	{ 2000, 2, _T("1 B/s") },
	{  500, 2, _T("4 B/s") },
	{  500, 5, _T("10 B/s") },
	{  400, 60, _T("150 B/s") },
	{ 2000, 1000, _T("500 B/s") },
	{ 1000, 1000, _T("1.00 KB/s") },
	{ 2000, 4000, _T("2.00 KB/s") },
	{ 2000, 4040, _T("2.02 KB/s") },
	{ 2000, 4047, _T("2.02 KB/s") },
	{ 2000, 4407, _T("2.20 KB/s") },
	{ 2000, 4000*1000, _T("2.00 MB/s") },
	{ 2000, 4040*1000, _T("2.02 MB/s") },
	{ 2000, 4047*1000, _T("2.02 MB/s") },
	{ 2000, 4407*1000, _T("2.20 MB/s") },
};

bool verify_CalSpeed()
{
	TCHAR buf[100];

	int ncases = GetEleQuan(sar_cases);
	int i;
	for(i=0; i<ncases; i++)
	{
		const VCalSpeed_st &cc = sar_cases[i];
		CCalSpeed::sprint(cc.diffmsec, cc.diffbytes, buf, GetEleQuan(buf));

		mm_printf(_T("Calspeed::sprint() verifying: %ums, %lluB, %s\n"), 
			cc.diffmsec, cc.diffbytes, cc.result);

		if(T_strcmp(cc.result, buf)!=0)
		{
			mm_printf(_T("-- This case verify error.\n"));
			return false;
		}
	}

	return true;
}
