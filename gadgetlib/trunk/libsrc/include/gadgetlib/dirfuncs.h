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


TCHAR * ggt_RelPathToAbsPath(const TCHAR *rel, TCHAR *bufprev);
	//!< Convert relative path to absolute path.
	/*!< This is only literal operation, not checking disk files.

	@param[in] rel
		The relative path. This can be an absolute path as well.

	@param[in] bufprev
		Buffer pointer returned by previous ggt_RelPathToAbsPath() or other functions
		declared here. 
		- If NULL, a new buffer will be allocated for the caller.
		- If non-NULL, this function will check whether bufprev can be re-used to accommodate
		  the result, this improves some performance. The internal works like ANSI C realloc().

	@return
		Return a string pointer to the resulting absolute path.
		The returned buffer should be freed by ggt_FreeDirfuncBuffer();

		- If return value is NULL, it means path buffer allocation fail and bufprev remains
		  intact. You should later free bufprev .
		- If return value is non-NULL, the input bufprev variable can be discarded; you 
		  just get the result from the return value(may or may not be the same as bufprev)
		  and free it later.
		  

	*/


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
