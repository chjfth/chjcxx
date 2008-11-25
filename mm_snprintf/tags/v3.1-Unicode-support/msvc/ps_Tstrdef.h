#ifndef __Tstrdef_h_
#define __Tstrdef_h_

#include <tchar.h>

#define TMM_strchr _tcschr
#define TMM_strlen _tcslen

#ifndef WINCE
# define TMM_isdigit _istdigit
#else
# define TMM_isdigit iswdigit // Just always use the wide-char variant on WinCE
#endif
	// Damn M$, hide _istdigit for WinCE app. See NLscan Wiki pageId=17432594 .

#define TMM_sprintf _stprintf

#endif
