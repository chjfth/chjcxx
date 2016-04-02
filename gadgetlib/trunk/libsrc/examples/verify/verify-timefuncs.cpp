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
	
	bool b;
	__int64 today = 1459575155;
	
	printf("ggt_gmtime ...\n");
	
	const int MyHour = 5;
	b = ggt_gmtime(today, ptm);
	assert(b);
	assert( ptm->tm_year==2016 && ptm->tm_mon==3 && ptm->mday==2 );
	assert( ptm->tm_hour==MyHour && ptm->tm_min==32 && ptm->sec==35 );
	assert( ptm->wday==6 && tm_yday==91);
	
	memset(ptm, 0, sizeof(*ptm));
	
	printf("ggt_localtime ...\n");
	int tz_minutes = ggt_LocalTimeZone();
	assert(tz_minutes!=-1);
	assert(tz_minutes%60==0); // sorry, this program only copes with whole-hour timezone offset
	
	ggt_localtime(today, ptm);
	assert(b);
	assert( ptm->tm_year==2016 && ptm->tm_mon==3 );

	if(tz_minutes >= -MyHour*60) {
		assert( ptm->mday==2 );
		assert( ptm->tm_hour==MyHour+tz_minutes/60 && ptm->tm_min==32 && ptm->sec==35 );
		assert( ptm->wday==6 && tm_yday==91);
	} 
	else {
		assert( ptm->mday==1 );
		assert( ptm->tm_hour==24+(MyHour+tz_minutes/60) && ptm->tm_min==32 && ptm->sec==35 );
		assert( ptm->wday==5 && tm_yday==90);
	}
	
}
