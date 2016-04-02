#ifndef __gadgetlib_timefuncs_h_20080811_
#define __gadgetlib_timefuncs_h_20080811_

#include <time.h> // for `struct tm'

#include <ps_TCHAR.h>
#include <ps_TypeDecl.h> // DECL_INT64()


#ifdef __cplusplus
extern"C" {
#else
# ifndef bool
# define bool int; // sly
# endif
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif
	
#define ggt_DELTA_EPOCH_IN_MILLISEC DECL_INT64(11644473600000)
	// Number of milliseconds between the beginning of the Windows epoch (Jan. 1, 1601) 
	// and the Unix epoch (Jan. 1, 1970) 

/* Leap year is any year divisible by four, but not by 100 unless also
 * divisible by 400
 */
#define ggt_IsLeapYear(y) ((!(y % 4)) ? (((!(y % 400)) && (y % 100)) ? 1 : 0) : 0)


typedef int ggt_Minute_t;

DLLEXPORT_gadgetlib
ggt_Minute_t ggt_LocalTimeZone(void);
	//!< Query local time zone.
	/*!< Return time difference(in minutes) of local time to UTC.
	 Example, for time zone of China(UTC+8), this functino returns 480 .
	 On a system not supporting timezone, it should return 0.
	 On a system supporting timezone but it fails, it should return -1.
	*/

DLLEXPORT_gadgetlib
__int64 ggt_time64(void);
	//!< Get current UTC time expressed in seconds elapsed since Unix epoch(Jan 1, 1970 UTC).

DLLEXPORT_gadgetlib
__int64 ggt_time64_local(void);
	//!< Similar to ggt_time64(), but express in elapsed seconds since Jan 1 1970 local time.
	//!< Briefly, it is ``ggt_time64()-ggt_LocalTimeZone()*60''.
	//!< (not oftenly used)

DLLEXPORT_gadgetlib
__int64 ggt_time64_millisec(void);
	//!< Similar to ggt_time64(), but express in millisecond instead of in second.

DLLEXPORT_gadgetlib
__int64 ggt_time64_local_millisec(void);
	//!< Similar to ggt_time64_local(), but express millisecond instead of second.

DLLEXPORT_gadgetlib
bool ggt_gmtime(__int64 epsec, struct tm *ptm);

DLLEXPORT_gadgetlib
bool ggt_localtime(__int64 epsec, struct tm *ptm);

DLLEXPORT_gadgetlib
bool ggt_gmtime_now(struct tm *ptm);
	//!< Get current UTC time expressed in ANSI C struct tm, similar to ANSI C gmtime(). 
	/*!< If gmtime() from your system lib does not support year 2038, use this instead.
	*/

DLLEXPORT_gadgetlib
bool ggt_gmtime_now_millisec(struct tm *ptm, int *pMillisec);
	//!< Similar to ggt_gmtime_now, but return millisecond as well. 
	/*!< The returned *pMillisec has the range from 0 to 999 . 
	 If pMillisec is NULL, the millisecond value will not be returned.

	 Implementor's Note: If the system does not support reporting millisecond precision,
	 *pMillisec should be filled with 0 and return true instead of returning false to user.
	*/

DLLEXPORT_gadgetlib
bool ggt_localtime_now(struct tm *ptm);
	//!< Similar to ggt_gmtime, but expressed in local time.

DLLEXPORT_gadgetlib
bool ggt_localtime_now_millisec(struct tm *ptm, int *pMillisec);
	//!< Similar to ggt_localtime_now, but return millisecond as well. 


DLLEXPORT_gadgetlib
bool ggt_SleepMillisec(int millisec);
	//!< Sleep for a period of time.
	/*!< The behavior of millisec==0 can be either return immediately or giving current CPU slice,
	 so don't busy calling ggt_SleepMillisec(0) .
	*/

DLLEXPORT_gadgetlib
__int64 ggt_GetOsMillisec(void);
	//!< Get number of milliseconds since the device booted, excluding any time that the system was suspended.
	/*!< According to actual OS behavior, this counter may wrap to 0 when it reaches some limit.
	 If it wraps to 0, the function should return 1.
	 Return value 0 is used when this function fails.
	 ggt_OsMillisecWrapLimit() tells this wrap limit.
	 To compare two [ time points returned by this func ], call ggt_OsMillisecElapsed().
	*/

DLLEXPORT_gadgetlib
int ggt_OsMillisecWrapBits(void);
	//!< Query the wrap bits of ggt_GetOsMillisec()'s return value.
	/*!< The value is represented in bits. e.g. for Win32 GetTickCout() which wraps at 2^32,
	 this function will return 32.
	*/

DLLEXPORT_gadgetlib
__int64 ggt_OsMillisecElapsed(__int64 old_msec, __int64 new_msec);
	//!< Return milliseconds elapsed from old_msec to new_msec.


enum ggt_Time_et {
	ggt_Year=1, ggt_Month=2, ggt_MDay=4, 
	ggt_Hour=8, ggt_Minute=16, ggt_Second=32,
	ggt_Millisec=64
};

DLLEXPORT_gadgetlib
TCHAR *ggt_FormatTimeStr(__int64 epsec, ggt_Time_et fmt, TCHAR *buf, int bufsize);
	//!< Format time to string
	/*!< epsec is the seconds since Unix epoch. 
		`fmt' can be 
		- ggt_Year: outputs YYYY-MM-DD hh:mm:ss
		- ggt_Month: outputs MM-DD hh:mm:ss
		- ggt_Hour: outputs hh:mm:ss

	@return Return buf.
		If error occurs, buf[0] is NULL character.
	*/


DLLEXPORT_gadgetlib
TCHAR *ggt_FormatTimeStr_Now(int isLocalTime, ggt_Time_et fmt, TCHAR *buf, int bufsize);
	//!< Similar to ggt_FormatTimeStr, but use current time.
	/*!< isLocalTime tells whether express time in local timezone or UTC . 

	@return Return buf.
		If get system time fail, buf[0] is NULL character.
	*/

#ifdef __cplusplus
} // extern"C" {
#endif


#endif
