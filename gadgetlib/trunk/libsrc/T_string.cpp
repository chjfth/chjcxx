#include <string.h>
#include <stdlib.h>


#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB

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
