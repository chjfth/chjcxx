#include "stdafx.h"

// #ifdef _MSC_VER
// # include <tchar.h>
// #endif

#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__T_string__DLL_AUTO_EXPORT_STUB(void){}



int 
T_strlen(const TCHAR *str)
{
#ifdef UNICODE
	int ret = (int)wcslen(str);
#else
	int ret = (int)strlen(str);
#endif
	return ret;
}


TCHAR * 
T_strcpy(TCHAR *dst, const TCHAR *src)
{
#ifdef UNICODE
	TCHAR * ret = wcscpy(dst, src);
#else
	TCHAR * ret = strcpy(dst, src);
#endif
	return ret;
}	


TCHAR * 
T_strncpy(TCHAR *dst, const TCHAR *src, int count)
{
#ifdef UNICODE
	TCHAR * ret = wcsncpy(dst, src, count);
#else
	TCHAR * ret = strncpy(dst, src, count);
#endif
	return ret;
}


int 
T_strcmp(const TCHAR *str1, const TCHAR *str2)
{
#ifdef UNICODE
	int ret = wcscmp(str1, str2);
#else
	int ret = strcmp(str1, str2);
#endif
	return ret;
}

int 
T_stricmp(const TCHAR *str1, const TCHAR *str2, size_t count)
{
#if (_MSC_VER >= 1400) && (!defined _WIN32_WCE) // For VS2005+
	return _tcsicmp(str1, str2);
#else
	int i;
	for(i=0; ; i++)
	{
		TCHAR c1 = toupper(str1[i]);
		TCHAR c2 = toupper(str2[i]);
		
		if(c1 < c2)
			return -1;
		else if(c1 > c2)
			return 1;

		if(!c1 && !c2)
			return 0;
	}
	return 0; // first count characters same
#endif
}

int 
T_strncmp(const TCHAR *str1, const TCHAR *str2, size_t count)
{
#ifdef UNICODE
	int ret = wcsncmp(str1, str2, count);
#else
	int ret = strncmp(str1, str2, count);
#endif
	return ret;
}

int 
T_strnicmp(const TCHAR *str1, const TCHAR *str2, size_t count)
{
#if _MSC_VER >= 1400 && (!defined _WIN32_WCE) // For VS2005+
	return _tcsnicmp(str1, str2, count);
#else
	int i;
	for(i=0; i<count; i++)
	{
		TCHAR c1 = toupper(str1[i]);
		TCHAR c2 = toupper(str2[i]);
		
		if(c1 < c2)
			return -1;
		else if(c1 > c2)
			return 1;

		if(!c1 && !c2)
			return 0;
	}
	return 0; // first count characters same
#endif
}


TCHAR * 
T_strstr(const TCHAR *str1, const TCHAR *str2)
{
#ifdef UNICODE
	return (TCHAR*)wcsstr(str1, str2);
#else
	return (TCHAR*)strstr(str1, str2);
#endif
}

TCHAR * 
T_strchr(const TCHAR *str, TCHAR chr)
{
#ifdef UNICODE
	return (TCHAR*)wcschr(str, chr);
#else
	return (TCHAR*)strchr(str, chr);
#endif
}


//DLLEXPORT_gadgetlib
//int T_printf wprintf // set-aside due to %s/%S difference between Windows and GLibc


//// Functions from stdlib.h

int 
T_atoi(const TCHAR *str)
{
	return T_strtoi(str, NULL, 10);
}

__int64 
T_atoi64(const TCHAR *str)
{
	return T_strtoi64(str, NULL, 10);
}


int 
T_strtoi(const TCHAR *str, TCHAR **endptr, int base)
{
#ifdef UNICODE
	int ret = wcstol(str, endptr, base);
#else
	int ret = strtol(str, endptr, base);
#endif
	return ret;
}



