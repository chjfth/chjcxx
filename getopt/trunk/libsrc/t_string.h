#ifndef __string_op_h_
#define __string_op_h_

#include <ps_TCHAR.h>

int t_strlen(const TCHAR *str);

TCHAR * t_strchr(const TCHAR *str, TCHAR chr);

int t_strcmp(const TCHAR *str1, const TCHAR *str2);

int t_strncmp(const TCHAR *str1, const TCHAR *str2, size_t count);


#endif
