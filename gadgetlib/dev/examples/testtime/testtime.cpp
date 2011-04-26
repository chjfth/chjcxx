#include <stdio.h>

#define DONT_WANT_PS_TCHAR
#include <gadgetlib/timefuncs.h> 

int main()
{
	
	char timestr1[40], timestr2[40];
	
	ggt_FormatTimeStr_Now(true, ggt_Year, timestr1, sizeof(timestr1));
	ggt_SleepMillisec(1000);
	ggt_FormatTimeStr_Now(true, ggt_Year, timestr2, sizeof(timestr2));

	printf("Now time  : %s\n", timestr1);
	printf("After 1sec: %s\n", timestr2);

	return 0;
}
