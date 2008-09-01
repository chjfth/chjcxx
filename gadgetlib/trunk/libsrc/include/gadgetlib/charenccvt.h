/*****************************************************************************
Code provided by Newland Auto-ID Tech Co.,Ltd. (www.nlscan.com)
Created: 2008-08-30 by Chen Jun
Function: Provide basic MBCS, Unicode convertion interface.
Updated log: 
*****************************************************************************/


#ifndef __charenccvt_h_20080829_
#define __charenccvt_h_20080829_

// Character encoding convert.

/* Interface provided by this file is similar to Windows API MultiByteToWideChar and 
 WideCharToMultiByte but with some enhancement.
	
	User note: MBCS chars referred in the functions here are all encoded in system 
 default locale.

	[2008-08-29]UTF-16 surrogate pair is not supported yet.
*/

#ifdef __cplusplus
extern"C" {
#else
# ifndef bool
# define bool int; // sly
# endif
#endif

#include <string.h> // For PC Windows, otherwise, wchar_t is not defined on VC6.


enum { ggt_max_mbcs_byteslen = 6 };
	//!< Max bytes that a single MBCS character('Char' for short) can occupy.
	/*!< I use 6 because an character represented in UTF-8 can be as long as 6 bytes.
	*/


wchar_t ggt_mb2wc(const char *mb, int *pConBytes);
	//!< Convert an MBCS char pointed to by mb to Unicode value.
	/*!< ggt_mb2wc checks the bytes sequence pointed to by mb for an MBCS char, 
	 and if recognized, return the Unicode value of that char.

	@param[in] mb
		Pointer to MBCS char sequence.

	@param[out] pConBytes
		*pConBytes tells how many bytes from mb is recognized as an MBCS char.
		"Con" means "consume".
			\n\n
		Can be NULL.

	@return
		Return the Unicode char.
		If the bytes from mb does not compose a valid MBCS char, (wchar_t)-1 is returned.
		Note: If the first byte(HZ: jiu) can not be recognized as an MBCS char, -1 will
		be returned with no further bytes being examined.
			\n\n
		If the conversion facility fails to work(e.g. MBCS<->Unicode mapping table absent),
		(wchar_t)-1 will also be returned.
	*/

int ggt_mbs2wcs(const char *pmb, int mbbytes, wchar_t *pwc, int wcbufchars, int *pConMbBytes);
	//!< Convert an MBCS string to Unicode string.
	/*!<

	@param[in] pmb
		Points to the MBCS string, whose length is determined by mbbytes.

	@param[in] mbbytes
		- If 0, ggt_mbs2wcs will return 0.
		- If positive, tells exactly how many bytes in pmb to convert. In this case, 
		  ggt_mbs2wcs will not append an extra NUL Char to result buffer.
		- If negative(e.g. -1), indicates pmb is an NUL terminated string, and ggt_mbs2wcs
		  converts the whole string, including the terminating NUL Char.

	@param[out] pwc
		Pointer to output Unicode string buffer.
		Can be NULL if wcbufchars is 0 or negative.

	@param[in] wcbufchars
		Tells the buffer size of pwc, in Unicode chars. 
		The output will never exceed this buffer limit but the MBCS string conversion 
		will still carry on in order to tell you how many Unicode chars are converted
		(as return value) on return.
			\n\n
		Let wcbufchars<=0 is a way to know the resulting chars without providing a
		real buffer.

	@param[out] pConMbBytes
		This tells you how many bytes from pmb are consumed(converted) on return.
			\n\n
		NOTE: This may be less than mbbytes or less than the NUL-terminating string length in bytes, 
		-- this can happen if an invalid byte sequence for the MBCS is encountered.
			\n
			Example: For a GBK(codepage 936) sequence "30 31 A4 20 ...", 0xA4 is the lead-byte
			for a GBK character, but 0x20 is an invalid trail-byte, so the byte starting
			from 0xA4 can not get converted, and (pmb + *pConMbBytes) will point to 0xA4.
			\n
		When such a converting error occurs, the conversion stops. User should check this output
		to know whether this happens. On getting such an error, user can try starting 
		ggt_mbs2wcs() with a new pmb skipping the problematic byte(i.e. let pmb points to 0x20 in the example).
			\n\n
		Can be NULL if not interested(not recommended).

	@return 
		Return how many Unicode characters are converted out -- assuming the output buffer
		is sufficient. 
			\n
		The return value cannot be negative.

	 Note: pmb and pwc must not overlap.

	 Special case: If mbbytes is given explicitly and the final byte of it does not
	 concludes an MBCS char(e.g. it is an MBCS lead byte), will ggt_mbs2wcs() peek the trail
	 byte(s) beyond mbbytes to output an converted Unicode char? The answer is no, it will
	 report *pConMbBytes less than mbbytes. To distinguish this special case with the real
	 trail byte missing case, user can check it with ggt_mb2wc() after ggt_mbs2wcs() returns.
	 ggt_mb2wc() will not do that check for you because ggt_mb2wc() don't have any idea 
	 whether the bytes beyond mbbytes is valid for the caller.
	*/

struct ggt_cvt_map_st
{
	int pos_mb;	//!< the position counted in bytes
	int pos_wc; //!< the position counted in Unicode characters
};

int ggt_mbs2wcs_go(const char *pmb, int mbbytes, wchar_t *pwc, int wcbufchars, 
	wchar_t badfill, int *pBadCvts, struct ggt_cvt_map_st arBadCvt[], int CvtBufsize);
	//!< Convert an MBCS string to Unicode string, auto processing bad converts.
	/*!< This function is similar to ggt_mbs2wcs, except that when invalid MBCS byte sequence
	 is encountered, it records the position of the bad converts and skip them, go on processing
	 from next input byte.


	@param[in] badfill
		When a bad MBCS byte sequence is seen, a Unicode Char will still be written to
		output buffer as a mark, this Char value is told by badfill.
			\n\n
		If 0, a system default Char is used.

	@param[out] pBadCvts
		Tells how many bad converts are encountered.
		Can be NULL if not interested.

	@param[out] arBadCvt
		User provided buffer receiving bad converts info. 
		For each bad convert, 
		- position of the bad byte from pmb is recorded in .pos_mb, the position is byte
		  offset from start of pmb.
		- position of the badfill char from pwc is recorded in .pos_wc, the position is 
		  the Unicode character offset from start of pwc.

	@param[in] CvtBufsize
		Tells elements of arBadCvt[] array so that the function will not write beyond the 
		given buffer. 

	@return
		Unicode chars written to output buffer, -- assuming output buffer is sufficient.

	*/


int ggt_wc2mb(wchar_t wc, char *pmb);
	//!< Convert a Unicode value to MBCS byte sequence.
	/*!< 

	@param[in] wc
		The Unicode char to convert.

	@param[out] pmb
		Points to a buffer receiving MBCS bytes sequence.
		User should provided a sufficient buffer length; use ggt_max_mbcs_byteslen(6)
		if you are unsure about the resulting length.
			\n\n
		Note: '\0' will not be append as output.
			\n\n
		This cannot be NULL.

	@return
		- If conversion success, return the MBCS bytes for the converted character.
		- Returning 0 means the character is not representable in MBCS,
		  or conversion facility fails to work. 
	
	 Suggestion: To check whether conversion facility works, call 
	 ggt_check_conversion_facility() .
	*/

int ggt_wcs2mbs(const wchar_t *pwc, int wcchars, char *pmb, int mbbufbytes, int *pConWcChars);
	//!< Convert an Unicode string to MBCS string .
	/*!<
	
	param[in] pwc
		Points to the Unicode string, whose length is determined by wcchars.

	param[in] wcchars
		- If 0, ggt_wcs2mbs will return 0.
		- If positive, tells exactly how many Chars in pwc to convert. In this case, 
		  ggt_wcs2mbs will not append an extra NUL Char to result buffer.
		- If negative(e.g. -1), indicates pwc is an NUL terminated string, and ggt_wcs2mbs
		  converts the whole string, including the terminating NUL Char.

	param[out] pmb
		Pointer to output MBCS string buffer.
		Can be NULL if mbbufbytes is 0 or negative.

	param[in] mbbufbytes
		Tells the buffer size of pmb in bytes.
		The output will never exceed this buffer limit but the string conversion 
		will still carry on in order to tell you it result in how many MBCS bytes 
		(as return value) on return.
			\n\n
		Let mbbufbytes<=0 is a way to know the resulting bytes without providing a
		real buffer.
	
	param[out] pConWcChars
		This tells you how many Unicode Chars from pwc are consumed(converted) on return.
			\n\n
		NOTE: This may be less than wcchars or less than the NUL-terminating string length in Chars, 
		-- this can happen if some Unicode char cannot be converted to MBCS.
			\n
			Example: Korean character(e.g. U+D574) does not have a code point in GBK(codepage 936).
			When GBK is the system default MBCS encoding, and a Unicode Char with value 0xD574 is
			encountered in pwc buffer, the conversion will stop prematurely, and 
			(pwc + *pConWcChars) will point to 0xD574.
			\n
		When such a converting error occurs, the conversion stops. User should check this output
		to know whether this happens. On getting such an error, user can try starting 
		ggt_wcs2mbs() with a new pwc skipping the problematic wchar_t value(i.e. let pwc points to 
		the wchar_t element after 0xD574 in the example).
			\n\n
		Can be NULL if not interested(not recommended).
		
	@return 
		Return how many MBCS bytes are converted out -- assuming the output buffer
		is sufficient. 
			\n
		The return value cannot be negative.

	 Note: pmb and pwc must not overlap.

	*/

int ggt_wcs2mbs_go(const wchar_t *pwc, int wcchars, char *pmb, int mbbufbytes, 
	const char *pszbadfill, int *pBadCvts, struct ggt_cvt_map_st arBadCvt[], int CvtBufsize);
	//!< Convert a Unicode string to MBCS string , auto processing bad converts.
	/*!< This function is similar to ggt_wcs2mbs, except that when invalid conversion happens,
	 it records the position of the bad converts and skip them, go on processing
	 from next input Char.


	@param[in] pszbadfill
		When a invalid Unicode to MBCS conversion is encountered, some MBCS bytes can be
		written to output buffer as a mark. These chars are determined by pszbadfill. 
		pszbadfill points to a NUL-terminated string, the string will be copied to output buffer.
			\n\n
		If NULL or pszbadfill[0]=='\0' , a system default string is used.

	@param[out] pBadCvts
		Tells how many bad converts are encountered.
		Can be NULL if not interested.

	@param[out] arBadCvt
		User provided buffer receiving bad converts info. 
		For each bad convert, 
		- position of the failing Char from pwc is recorded in .pos_wc, the position is wchar_t
		  offset from start of pwc.
		- position of the pszbadfill chars from pmb is recorded in .pos_mb, the position is 
		  the byte offset from start of pmb.

	@param[in] CvtBufsize
		Tells elements of arBadCvt[] array so that the function will not write beyond the 
		given buffer. 

	@return
		MBCS bytes written to output buffer, -- assuming output buffer is sufficient.

	*/


#ifdef __cplusplus
} // extern"C" {
#endif



#endif
