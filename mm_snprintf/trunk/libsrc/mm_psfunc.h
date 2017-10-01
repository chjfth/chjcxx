#ifndef __mm_psfunc_20061002_h_
#define __mm_psfunc_20061002_h_

#include <ps_TCHAR.h>

#ifdef __cplusplus
extern"C"{
#endif


int mmps_i64_type_prefix(char *szPrefix); // no use since v5.0
/*!<
	The implementer of this function returns in \e szfmt the type prefix 
 for formatting a 64-bit integer. For example:
	- For Microsoft Visual C++: "I64"
	- For GNU glibc: "ll"

  @return Returns the length of the type prefix string stored in \e szfmt,
	in characters.
*/

wchar_t mmps_wsfmt_char(); // ws: wide-string
	// Return "s" on Windows, implying that wprintf("%s") outputs a wide-string.
	// Return "S" on Linux, implying that wprintf("%S") outputs a wide-string.
	// So wprintf outputs correct wide string.


#ifdef __cplusplus
}//extern"C"{
#endif

#endif
