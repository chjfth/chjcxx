#ifndef __dirfuncs_h_20080825_h
#define __dirfuncs_h_20080825_h


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
#define bool int; // sly
#endif


bool ggt_FreeDirfuncBuffer(TCHAR *p);
	//!< Free dir or path buffer allocated by functions below.


TCHAR * ggt_RelPathToAbsPath(const TCHAR *rel);
	//!< Convert relative path to absolute path.
	/*!< This is only literal operation, not checking disk files.

	@param[in] rel
		The relative path. This can be an absolute path as well.

	@return
		Return a string pointer to the resulting absolute path. (can be null?)
		The returned buffer should be freed by ggt_FreeDirfuncBuffer();
	*/


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
