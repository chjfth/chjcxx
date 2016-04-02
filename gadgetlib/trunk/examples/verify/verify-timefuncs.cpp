#undef NDEBUG // for assert() to work.

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <commdefs.h>
#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <mm_snprintf.h>
#include <gadgetlib/T_string.h>
#include <gadgetlib/timefuncs.h>
//#include <getopt/sgetopt.h>


#include "share.h"

void verify_timefuncs()
{
	struct tm mytm, *ptm = &mytm;
	
	int i;
	bool b;
	__int64 today = 1459575155;
	
	printf("ggt_gmtime ...\n");
	
	const int MyHour = 5;
	b = ggt_gmtime(today, ptm);
	assert(b);
	assert( ptm->tm_year==2016-1900 && ptm->tm_mon==3 && ptm->tm_mday==2 );
	assert( ptm->tm_hour==MyHour && ptm->tm_min==32 && ptm->tm_sec==35 );
	assert( ptm->tm_wday==6 && ptm->tm_yday==91);
	
	memset(ptm, 0, sizeof(*ptm));
	
	printf("ggt_localtime ...\n");
	int tz_minutes = ggt_LocalTimeZone();
	assert(tz_minutes!=-1);
	assert(tz_minutes%60==0); // sorry, this program only copes with whole-hour timezone offset
	
	ggt_localtime(today, ptm);
	assert(b);
	assert( ptm->tm_year==2016-1900 && ptm->tm_mon==3 );

	if(tz_minutes >= -MyHour*60) {
		assert( ptm->tm_mday==2 );
		assert( ptm->tm_hour==MyHour+tz_minutes/60 && ptm->tm_min==32 && ptm->tm_sec==35 );
		assert( ptm->tm_wday==6 && ptm->tm_yday==91);
	} 
	else {
		assert( ptm->tm_mday==1 );
		assert( ptm->tm_hour==24+(MyHour+tz_minutes/60) && ptm->tm_min==32 && ptm->tm_sec==35 );
		assert( ptm->tm_wday==5 && ptm->tm_yday==90);
	}
	
	printf("ggt_gmtime (year 2038) ...\n");
	struct St64test {
		__int64 epsec;
		int year, month, mday, hour, minute, second;
	}ar_t64samples[] = 
	{
		{ 0x80000000, 2038,1,19, 3,14,8 }, // year 2038, Jan-19th
		{ 0xFFFFFFFF, 2106,2,7, 6,28,15 },
		{0x200000000, 2242,3,16, 12,56,32 },
	};
	int t64samples = GetEleQuan_i(ar_t64samples);
	for(i=0; i<t64samples; i++)
	{
		St64test &sam = ar_t64samples[i];
		T_printf(_T("  Verifying epsec=0x%-9llX, expected: %d-%02d-%02d %02d:%02d:%02d\n"),
			 sam.epsec, 
			sam.year, sam.month, sam.mday, sam.hour, sam.minute, sam.second);
		b = ggt_gmtime(sam.epsec, ptm);
		assert( ptm->tm_year+1900==sam.year && ptm->tm_mon+1==sam.month && ptm->tm_mday==sam.mday );
		assert( ptm->tm_hour==sam.hour && ptm->tm_min==sam.minute && ptm->tm_sec==sam.second );
	}
}
