#include <gadgetlib/timefuncs.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__time_common__DLL_AUTO_EXPORT_STUB(void){}

__int64 
ggt_OsMillisecElapsed(__int64 old_msec, __int64 new_msec)
{
	int wrapbit = ggt_OsMillisecWrapBits();

	__int64 delta = new_msec - old_msec;

	delta &= ((__int64)1<<wrapbit)-1;
	
	return delta;
}
