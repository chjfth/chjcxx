/*****************************************************************************
Code provided by Newland Auto-ID Tech Co.,Ltd. (www.nlscan.com)
Created: 2008-08-30 by Chen Jun
Function: Provide basic MBCS, Unicode conversion interface.
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

#include <string.h> // Need it on PC Windows, otherwise, wchar_t is not defined on VC6.

#include <gadgetlib/ggt-const.h>

enum { ggt_max_mbcs_byteslen = 6 };
	//!< Max bytes that a single MBCS character('Char' for short) can occupy.
	/*!< I use 6 because an character represented in UTF-8 can be as long as 6 bytes.
	*/

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


DLLEXPORT_gadgetlib
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
		Suggestion: Use macro ggt_INVALID_WCHAR for (wchar_t)-1 .
		Warning: if wchar_t equals "unsigned short", omitting (wchar_t) before -1 will wrong compare result.
			\n
		Note: If the first byte(HZ: jiu) can not be recognized as an MBCS char, -1 will
		be returned with no further bytes being examined.([2009-01-07]STOP! How can this happen?)
			\n\n
		If the conversion facility fails to work(e.g. MBCS<->Unicode mapping table absent),
		(wchar_t)-1 will also be returned.
	*/

DLLEXPORT_gadgetlib
int ggt_mbs2wcs(const char *pmb, int mbbytes, wchar_t *pwc=0, int wcbufchars=0, int *pConMbBytes=0);
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
	 ggt_mbs2wcs() will not do that check for you because ggt_mbs2wcs() don't have any idea 
	 whether the bytes beyond mbbytes is valid for the caller.
	*/

struct ggt_cvt_map_st
{
	int pos_mb;	//!< the position counted in bytes
	int pos_wc; //!< the position counted in Unicode characters
};

DLLEXPORT_gadgetlib
int ggt_mbs2wcs_go(const char *pmb, int mbbytes, wchar_t *pwc=0, int wcbufchars=0, 
	wchar_t badfill=0, int *pBadCvts=0, struct ggt_cvt_map_st arBadCvt[]=0, int CvtBufsize=0);
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


DLLEXPORT_gadgetlib
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

DLLEXPORT_gadgetlib
int ggt_wcs2mbs(const wchar_t *pwc, int wcchars, char *pmb=0, int mbbufbytes=0, int *pConWcChars=0);
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

DLLEXPORT_gadgetlib
int ggt_wcs2mbs_go(const wchar_t *pwc, int wcchars, char *pmb=0, int mbbufbytes=0, 
	const char *pszbadfill=0, int *pBadCvts=0, struct ggt_cvt_map_st arBadCvt[]=0, int CvtBufsize=0);
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



DLLEXPORT_gadgetlib
char *ggt_charnext(const char *pchar);
	//!< Determine what is the next character in a MBCS byte sequence. 
	/*!< ... Almost the same to Windows API CharNext.
	 This function shows its special significance when pchar points to an illegal
	 MBCS byte sequence, e.g. in GBK encoding(codepage=936), (hex) B0 34 is 
	 an illegal sequence since 0x34 is not a valid trail-byte of a GBK character.
	 In this case, there are two equally reasonable decisions:
	   1. Take 0x34 as next char.
	   2. Take the byte sequence after 0x34 as next char. 
	 Different system(OS platform) may take different choice, so it is suggested to
	 call ggt_charnext to determine the "next char" in current system's perspective.

	 PS: Windows takes choice 2 .

	@param[in] pchar
		Points to input MBCS byte sequence.

	@return
		If *pchar!='\0', return the pointer to the next MBCS character.
		If *pchar=='\0', just return pchar.
	*/

DLLEXPORT_gadgetlib
char *ggt_charprev(const char *pStartScan, const char *pCurChar);
	//!< Find the character(may be a MBCS character) preceding pCurChar. 
	/*!< Almost the same to Windows API CharPrev.

	@param[in] pStartScan
		Pointer to the start scanning position, i.e. start of your MBCS string.

	@param[in] pCurChar
		Pointer to the MBCS character to which you'd like to know the previous character.

	return
		Return pointer to the previous character to pCurChar.
		Special: If pCurChar==pSstartScan, pCurChar is returned.
	*/


DLLEXPORT_gadgetlib
int ggt_utf8_encode_1char(unsigned int ucs4, char utf8seq[6]);

DLLEXPORT_gadgetlib
int ggt_encode_to_utf8(const wchar_t *wcs, int wccount, char *utf8out, int utf8bufsize,
	int *pwcs_used=0);
	//!< Convert(encode) a wchar_t string into utf8 byte sequence. 
	/*!< 

	@param[in] wcs
		Input wchar_t string. This can be NUL-terminated or not, depending on wccount param.
		
		If unsupported wchar_t encountered(surrogate pair), the conversion stops, and *pwcs_used will tell 
		valid wchar_t's already processed.

	@param[in] wccount
		Tells the length of input wchar_t string.
		If -1, consider wcs as NUL-terminated, and the convertion result will be nul-terminated as well.
		If >=0, converting exactly that many wchar_t's.
		
	
	@param[out] utf8out 
		utf8 output buffer.
	
	@param[in] utf8bufsize
		utf8out's buffer size in bytes.
		If 0, utf8out[] will not be written to, and return value just tells how many bytes are required to
		hold the result, include possible one terminating nul char.
	
	@param[out] pwcs_used
		Report how many wchar_t's are consumed from wcs.
	
	return
		If utf8bufsize==0, return how any bytes are required to hold the encoding result.
		If utf8bufsize>0, return how many bytes are actually stored into utf8out.
		
		When invalid input parameter detected, return -1.
		
		Note: I will not store "partial" utf8 sequence into output buffer. For example, a wchar_t encodes to 
		threes utf8 bytes, and I will not store any of the three bytes if remaining output buffer is one or two.
		
		Note: utf8out is not guaranteed to be nul-terminated.

	Note on Windows: Currently this function can not cope with Unicode surrogate pair. 
	(TO IMPROVE LATER)
	*/

DLLEXPORT_gadgetlib
int ggt_utf8_decode1char(const char *utf8s, int lim, unsigned int *pwc);


DLLEXPORT_gadgetlib
int ggt_decode_from_utf8(const char* utf8s, int utf8_bytes, wchar_t *wbuf, int wbuf_chars=0, 
	int *pbytes_used=0);
	//!< Convert(decode) a utf8 string into wchar_t string. 
	/*!< 

	@param[in] utf8s
		Input utf8 string. This can be nul-terminated or not, depending on utf8_bytes param.
		
		If invalid utf8 sequence found, the conversion stops, and *pbytes_used will tell 
		valid bytes already processed.

	@param[in] utf8_bytes
		Tells the length of input utf8 bytes.
		* If -1, consider utf8s as nul-terminated, and the conversion result will be NUL-terminated as well.
		* If >=0, converting exactly that many bytes. 
		
		Note: If invalid utf8 sequence is encountered before the desired end, conversion stops after
		converting the final valid sequence, and *pbytes_used will tell you where it stops.
		So, when utf8_bytes is -1, success is identified by (*pbytes_used>0 && utf8s[*pbytes_used-1]=='\0') .
		When utf8_bytes>=0, success is identified by (*pbytes_used==utf8_bytes) .
	
	@param[out] wbuf
		wchar_t output buffer.
		
	@param[in] wbuf_chars
		If 0, wbuf[] will not be written to, and return value just tells how many wchar_t's are required to 
		hold the result, including possible one terminating NUL.
		(If invlalid utf8 sequence encountered, return value will only indicate for those valid utf8 sequences.)
		
		If >0, tells your buffer size, in wchar_t count.

	@param[out] pbytes_used (optional)
		Report how many bytes of valid utf8 sequence are consumed.

	return
		If wbuf_chars==0, return how many wchar_t are required to hold the decoding result.
		If wbuf_chars>0, return how many wchar_t are actually stored into wbuf.
		
		When invalid input parameter detected, return -1.
		
		Note: wbuf is not guaranteed to be NUL-terminated.
	
	Note on Windows: if the decoded wchar_t exceeds 0xFFFF, I'll consider the input utf8 sequence as invalid.
		
	*/



DLLEXPORT_gadgetlib
wchar_t **ggt_argvs_utf8_to_wchars(int argc, char *argv[]);

DLLEXPORT_gadgetlib
void ggt_argvs_free_wchars(wchar_t **wargv);


#ifdef __cplusplus
} // extern"C" {
#endif



#endif
