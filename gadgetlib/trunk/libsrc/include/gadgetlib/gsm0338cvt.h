#ifndef __gsm0338cvt_h_20100112_
#define __gsm0338cvt_h_20100112_

// Do character code conversion between GSM Default alphabet(GSM 03.38 spec) and Unicode.
// Character table reference: http://en.wikipedia.org/wiki/GSM_03.38

#ifdef __cplusplus
extern"C" {
#else
# ifndef bool
# define bool int; // sly
# endif
#endif

#ifdef DONT_WANT_PS_TCHAR
# if defined WIN32 || defined WINCE
#  include <tchar.h> // The Microsoft way
# else
   typedef char TCHAR;
# endif
#else
# include <ps_TCHAR.h>
#endif

#include <gadgetlib/ggt-const.h>


wchar_t ggt_gsm2uc(int gsmc);
	//!< Convert a single GSM 03.38 charcode to Unicode.
	/*!< 
	@param[in] gsmc
		GSM 03.38 character code. Valid value like:
		0	    @
		0x21	!
		0x40	the upside down !
		0x1B0A	form feed
		0x1b14	^
		0x1b2f	\

	@return
		Return the converted Unicode charcode.
		If gsmc is not a valid GSM 03.38 charcode, ggt_INVALID_WCHAR(-1) is returned.
	*/

int ggt_uc2gsm(wchar_t uc);
	//!< Convert a single Unicode charcode to GSM 03.38 charcode.
	/*!< 
	@param[in] uc
		Unicode character code.

	@return
		Return the converted GSM 03.38 charcode.
		If uc cannot be mapped to a valid GSM 03.38 character, -1 will be returned.
	*/


TCHAR * XXggt_RelPathToAbsPath(const TCHAR *rel, TCHAR *bufprev=0);
	//!< Convert relative path to absolute path.
	/*!< This is only literal operation, not checking disk files.

	@param[in] rel
		The relative path. This can be an absolute path as well.

	@param[in] bufprev
		Buffer pointer returned by previous ggt_RelPathToAbsPath() or other functions
		declared in this header(e.g TO-BE-ADDED). 
		- If NULL, a new buffer will be allocated for the caller.
		- If non-NULL, this function will check whether bufprev can be re-used to accommodate
		  the result, this improves some performance. The internal works like ANSI C realloc().
		  
		Note again: You cannot pass a casual TCHAR pointer to bufprev, e.g. you cannot pass
		a static C string to bufprev.

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
