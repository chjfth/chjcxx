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


wchar_t ggt_mb2wc(const char *mb, int *pConBytes);
	//!< Convert an MBCS char pointed to by mb to Unicode value.
	/*!< ggt_mb2wc checks the bytes sequence pointed to by mb for an MBCS char, 
	 and if recognized, return the Unicode value of that char.

	@param[in] mb
		Pointer to MBCS char sequence.

	@param[out] pConBytes
		Output many bytes from mb is recognized as as MBCS char.
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

int ggt_wc2mb(wchar_t wc, char *pmb);
	//!< Convert a Unicode value to MBCS byte sequence.
	/*!< 

	@param[in] wc
		The Unicode char.

	@param[out] pmb
		Points to a buffer receiving MBCS bytes sequence.
		User should guarantee the buffer size is enough. 6 bytes is enough for any char.
		Note: '\0' will not be append as output.

	@return
		Return bytes of the converted MBCS byte sequence(1 or greater).
			\n\n
		If the conversion facility fails to work(e.g. MBCS<->Unicode mapping table absent),
		-1 is returned.
	*/

int ggt_mbs2wcs(const char *pmb, int mbbytes, wchar_t *pwc, int wcbufchars, int *pConMbBytes);
	//!< Convert an MBCS string to Unicode string.
	/*!<

	@param[in] pmb
		Points to the MBCS string, whose length is determined by mbbytes.

	@param[in] mbbytes
		- If 0, ggt_mbs2wcs will return 0.
		- If positive, tells exactly how many bytes in pmb to convert. In this case, 
		  ggt_mbs2wcs will not append an extra NUL char to result buffer.
		- If negative(e.g. -1), indicates pmb is an NUL terminated string, and ggt_mbs2wcs
		  converts the whole string, including the terminating NUL char.

	@param[out] pwc
		Pointer to output Unicode char buffer.

	@param[in] wcbufchars
		Tells the buffer size of pwc, in Unicode chars. 
		The output will never exceed this buffer limit but the MBCS string conversion 
		will still carry on in order to tell you how many Unicode chars are converted
		(as return value) on return.
			\n\n
		Let wcbufchars<=0 is a way to know the resulting chars without providing a
		real buffer(let pwc==NULL).

	@param[out] pConMbBytes
		This tells you how many bytes from pmb are consumed(converted) on return.
			\n\n
		NOTE: This may be less than mbbytes or less than the NUL-terminating string length in bytes, 
		-- this can happen if an invalid byte sequence for the MBCS is encountered.
			\n
			Example: For a GBK(codepage 936) sequence "30 31 A4 20 ...", 0xA4 is the lead-byte
			for a GBK character, but 0x20 is an invalid trail-byte, so the byte starting
			from 0xA4 can not get converted, and pmb+*pConMbBytes will point to 0xA4.
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
	 is encountered, it records the position of the bad converts and skip it, go on processing
	 from next input byte.


	@param[in] badfill
		When a bad MBCS byte sequence is seen, a Unicode char will still be written to
		output buffer, this char value is told by badfill.

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


#ifdef __cplusplus
} // extern"C" {
#endif



#endif
