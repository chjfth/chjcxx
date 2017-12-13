#include <string.h>
#include "t_string.h"

int 
t_strlen(const TCHAR *str)
{
#ifdef UNICODE
	int ret = (int)wcslen(str);
#else
	int ret = (int)strlen(str);
#endif
	return ret;
}

TCHAR * 
t_strchr(const TCHAR *str, TCHAR chr)
{
#ifdef UNICODE
	return (TCHAR*)wcschr(str, chr);
#else
	return (TCHAR*)strchr(str, chr);
#endif
}


int 
t_strcmp(const TCHAR *str1, const TCHAR *str2)
{
#ifdef UNICODE
	int ret = wcscmp(str1, str2);
#else
	int ret = strcmp(str1, str2);
#endif
	return ret;
}

int 
t_strncmp(const TCHAR *str1, const TCHAR *str2, size_t count)
{
#ifdef UNICODE
	int ret = wcsncmp(str1, str2, count);
#else
	int ret = strncmp(str1, str2, count);
#endif
	return ret;
}
