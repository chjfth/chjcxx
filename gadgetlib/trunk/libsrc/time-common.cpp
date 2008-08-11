#include <gadgetlib/timefuncs.h>

__int64 
ggt_OsMillisecElapsed(__int64 old_msec, __int64 new_msec)
{
	int wrapbit = ggt_OsMillisecWrapLimit();

	__int64 delta = new_msec - old_msec;

	delta &= ((__int64)1<<wrapbit)-1;
	
	return delta;
}
