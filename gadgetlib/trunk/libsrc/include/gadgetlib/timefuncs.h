#ifndef __gadgetlib_timefuncs_h_20080811_
#define __gadgetlib_timefuncs_h_20080811_

#include <time.h> // for `struct tm'

#ifdef DONT_WANT_PS_TCHAR
# if defined WIN32 || defined WINCE
#  include <tchar.h> // The Microsoft way
# else
   typedef char TCHAR;
# endif
#else
# include <ps_TCHAR.h>
#endif


#ifdef __cplusplus
extern"C" {
#else
# ifndef bool
# define bool int; // sly
# endif
#endif


/* Leap year is any year divisible by four, but not by 100 unless also
 * divisible by 400
 */
#define ggt_IsLeapYear(y) ((!(y % 4)) ? (((!(y % 400)) && (y % 100)) ? 1 : 0) : 0)


typedef int ggt_Minute_t;

ggt_Minute_t ggt_LocalTimeZone(void);
	//!< Query local time zone.
	/*!< Return time difference(in minutes) of local time to UTC.
	 Example, for time zone of China(UTC+8), this functino returns 480 .
	*/

__int64 ggt_time64(void);
	//!< Get current UTC time expressed in seconds elapsed since Unix epoch(Jan 1, 1970).

__int64 ggt_time64_local(void);
	//!< Similar to ggt_time64(), but express in localtime.

__int64 ggt_time64_millisec(void);
	//!< Similar to ggt_time64(), but express millisecond instead of second.

__int64 ggt_time64_local_millisec(void);
	//!< Similar to ggt_time64_local(), but express millisecond instead of second.


bool ggt_gmtime(struct tm *ptm);
	//!< Get current UTC time expressed in ANSI C struct tm, similar to ANSI C gmtime(). 
	/*!< If gmtime() from your system lib does not support year 2038, use this instead.
	*/

bool ggt_gmtime_millisec(struct tm *ptm, int *pMillisec);
	//!< Similar to ggt_gmtime, but return millisecond as well. 
	/*!< The returned *pMillisec has the range from 0 to 999 . 
	 If pMillisec is NULL, the millisecond value will not be returned.

	 Implementor's Note: If the system does not support reporting millisecond precision,
	 *pMillisec should be filled with 0 and return true instead of returning false to user.
	*/

bool ggt_localtime(struct tm *ptm);
	//!< Similar to ggt_gmtime, but expressed in local time.

bool ggt_localtime_millisec(struct tm *ptm, int *pMillisec);
	//!< Similar to ggt_localtime, but return millisecond as well. 


bool ggt_SleepMillisec(int millisec);
	//!< Sleep for a period of time.
	/*!< The behavior of millisec==0 can be either return immediately or giving current CPU slice,
	 so don't busy calling ggt_SleepMillisec(0) .
	*/

__int64 ggt_GetOsMillisec(void);
	//!< Get number of milliseconds since the device booted, excluding any time that the system was suspended.
	/*!< According to actual OS behavior, this counter may wrap to 0 when it reaches some limit.
	 ggt_OsMillisecWrapLimit() tells this wrap limit.
	 To compare two [ time points returned by this func ], call ggt_OsMillisecElapsed().
	*/

int ggt_OsMillisecWrapBits(void);
	//!< Query the wrap bits of ggt_GetOsMillisec()'s return value.
	/*!< The value is represented in bits. e.g. for Win32 GetTickCout() which wraps at 2^32,
	 this function will return 32.
	*/

__int64 ggt_OsMillisecElapsed(__int64 old_msec, __int64 new_msec);
	//!< Return milliseconds elapsed from old_msec to new_msec.


enum ggt_Time_et {
	ggt_Year=1, ggt_Month=2, ggt_MDay=4, 
	ggt_Hour=8, ggt_Minute=16, ggt_Second=32, 
};

TCHAR *ggt_FormatTimeStr(__int64 t64sec, ggt_Time_et fmt, TCHAR *buf, int bufsize);
	//!< Format time to string
	/*!< t64sec is the seconds since Unix epoch. 
		`fmt' can be 
		- ggt_Year: outputs YYYY-MM-DD hh:mm:ss
		- ggt_Month: outputs MM-DD hh:mm:ss
		- ggt_Hour: outputs hh:mm:ss

	@return Return buf.
		If error occurs, buf[0] is NULL character.
	*/


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
