#ifndef __mbcs_ops_h_20081231_
#define __mbcs_ops_h_20081231_

#ifdef __cplusplus
extern"C" {
#endif

//! Flags saying how to round a MBCS or UTF-8 string
enum ggt_RoundMbcs_et {
	ggt_RoundHeadUp = 1, ggt_RoundHeadDown = 0,
	ggt_RoundTailUp = 2, ggt_RoundTailDown = 0,
};


typedef int (*ggt_PROC_MbcsChk)(const char *pChk, void *pCallbackExtra);
	//!< The callback function type used with ggt_RoundMbcsString, to recognize the next MBCS char,
	/*!< This function checks the chars pointed to by pChk to find the next MBCS 
	 character in the string, or to skip the current invalid MBCS sequence.
		The first char by pChk may not be the lead-byte of an MBCS character; the function
	 implementor should be aware of this, and skip the incomplete/invalid MBCS byte sequence to
	 find the next valid MBCS character.

	@param[in] pChk
		Points to the string/text to check.

	@return
		* If bytes starting at pChk is recognized as a valid MBCS character, this function
		  returns a positive value. This positive value tells how many bytes constitute
		  the current MBCS character. 
		* If bytes starting at pChk is NOT recognized as a valid MBCS character, this function
		  returns a negative value. The absolute value of the return tells how many bytes
		  should be skipped in order to meet the next valid MBCS character.

	 Either case, by adding the ret-value to pChk, the caller knows where the next MBCS character is.
	*/

char *ggt_RoundMbcsString(const char *pIn, int InBytes, const char *pByteResetScan,
		ggt_RoundMbcs_et Flag, int *pResultBytes, 
		ggt_PROC_MbcsChk proc=0, void *pCallbackExtra=0);
	//!< Check a chunk of MBCS byte sequence, to recognize the correct MBCS characters in it.
	/*!< Usage scenario: User has a byte sequence by pIn and InBytes, but pIn may not point
	 to the lead-byte of an MBCS character and pIn+InBytes may not point to the trail-byte of
	 an MBCS char. So, the user calls this function to find the correct MBCS lead-byte and the
	 correct MBCS trail-byte.

	@param[in] pIn
		Points to user input byte sequence.

	@param[in] InBytes
		Tells how many bytes are given by pIn.
		Of course, there is no need to have any NUL-char after InBytes.

	@param[in] pByteResetScan
		A pointer that is known to correctly point to an MBCS lead-byte.
		This pointer must be ahead of(lower address) pIn. 
		Rationale: When user
			* is uncertain whether pIn points to a valid lead-byte, or
			* requests ggt_RoundHeadUp flag, 
		this function can go back scanning from pByteResetScan to find which byte is
		the correct lead-byte around pIn, or which is the MBCS character ahead of pIn.
		Without such scanning, we cannot know the accurate answer.
			\n
		If NULL, no such scanning will be carried out, so the request of ggt_RoundHeadUp
		will be ignored.
			\n
		If not NULL, the scanning from pByteResetScan will always be carried out so as
		to get the most accurate result.

	@param[in] Flag
		Tells how you would like to round the string head and string tail.
		Two groups of const can be used.
		* First group, ggt_RoundHeadUp and ggt_RoundHeadDown. You can choose either one.
		  - ggt_RoundHeadUp tells ggt_RoundMbcsString try to find an MBCS lead-byte 
		    ahead of pIn -- if *pIn itself is not considered as lead-byte by ggt_RoundMbcsString.
		  - ggt_RoundHeadDown tells ggt_RoundMbcsString try to find an MBCS lead-byte
		    after   pIn  -- if *pIn itself is not considered as lead-byte by ggt_RoundMbcsString.
		* Second group, ggt_RoundTailUp and ggt_RoundTailDown. You can choose either one.
		  - ggt_RoundTailUp tells ggt_RoundMbcsString try to find an MBCS trail-byte 
		    ahead of pIn+InBytes-1 -- if pIn[InBytes-1] itself is not considered as trail-byte.
		  - ggt_RoundTailDown tells ggt_RoundMbcsString try to find an MBCS trail-byte 
		    after   pIn+InBytes-1  -- if pIn[InBytes-1] itself is not considered as trail-byte.
			
		  of the user provided string -- that is the bytes around pIn+InBytes-1 .

	@param[out] pResultBytes
		Outputs the bytes of the adjusted MBCS string. This may be more or less than InBytes.

	@param[in] proc
		This callback provides the routine that can tell what bytes constitute a MBCS character.
			\n
		If NULL, a default routine is used.

	@param[in] pCallbackExtra
		Extra data to pass to ggt_PROC_MbcsChk routine.

	@return 
		Returns the pointer to the first byte of the resulting MBCS string.
		This pointer may be equal to pIn, or one or more bytes before/after pIn .
			\n
		Returning NULL on error, e.g invalid/inproper paramenter input.

	Example:
    <pre> (use spaces instead of TABs in pre section please)

        We have a string 
		    const char *pAllText = "IBMÀ¶4µçÄÔ"; // (the MBSC string is GBK, codepage 936)
        HEX:    49 42 4D C0 B6 34 B5 E7 C4 D4 
		        -- -- -- ----- -- ----- -----

	Case 1:
        "IBMÀ¶4µçÄÔ"
          ^pIn, InBytes=2
         Return [pIn, 2] // which means: return value is pIn, and *pResultBytes=2 .

	Case 2:
        "IBMÀ¶4µçÄÔ"
          ^pIn , InBytes=3 , Flag=ggt_RoundTailDown
         In this case, input tail splits an MBCS Char, so the tail must be adjusted on return.
         Return [pIn, 4]

	Case 3:
        "IBMÀ¶4µçÄÔ"
          ^pIn , InBytes=3 , Flag=ggt_RoundTailUp
         Similar to Case 2, only Flag different
         Return [pIn, 2]

	Case 4:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=2 , Flag=ggt_RoundHeadDown
         In this case, input head points to an MBCS trail-byte, and it does not make up anonther 
         MBCS Char with the byte following it(0x34). So,
         * If pByteResetScan==NULL, it can be deduced without question 0x34 is the next valid MBCS Char.
           Return [pIn+1, 1]
         * If pByteResetScan==pAllText, ggt_RoundMbcsString will do a scan from pAllText and
           deduce that 0xCOB6 is an MBCS Char, so next MBCS Char is 0x34.
           Return [pIn+1, 1]

	Case 5:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=2 , Flag=ggt_RoundHeadUp
         Similar to Case 4, only Flag different. 
         * If pByteResetScan==NULL, so ggt_RoundMbcsString don't know how to find the MBCS Char
           just ahead of pIn, so he leave the head position intact.
           Return [pIn, 2]
         * If pByteResetScan==pAllText, ggt_RoundMbcsString will do a scan from pAllText and
           deduce that 0xCOB6 is an MBCS Char, so the head is rounded UP.
           Return [pIn-1, 3]
         
        Caveat from Case 5: pByteResetScan==NULL leaves a "bad" output, so, when you are not certian
        whether pIn already points to a lead-byte, you should give pByteResetScan to make a safe scan.

	Case 6:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadUp
         Similar to Case 5, but InBytes=1. (no new tricky thing for this case)
         * If pByteResetScan==NULL,
           Return [pIn, 1]
         * If pByteResetScan==pAllText, 
           Return [pIn-1, 2]

	Case 7:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadDown
         Similar to Case 4, but InBytes=1.
         * If pByteResetScan==NULL, 
           Return [pIn+1, 0]
         * If pByteResetScan==pAllText, 
           Return [pIn+1, 0]   // Yes, same as pByteResetScan==NULL

	Case 8:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=3 , Flag=ggt_RoundHeadUp|ggt_RoundTailDown
         * If pByteResetScan==NULL,     Return [pIn, 4]
         * If pByteResetScan==pAllText, Return [pIn-1, 5]

	Case 10:
        "IBMÀ¶4µçÄÔ"
                ^pIn , InBytes=3 , Flag=ggt_RoundHeadUp|ggt_RoundTailDown
         * If pByteResetScan==NULL,     Return [pIn, 3]
           Reason: Without rescaning, ggt_RoundMbcsString will take 0xE7C4 as an MBCS Char.
         * If pByteResetScan==pAllText, Return [pIn-1, 4]
           Rescanning produce the correct result.
        
	Case 11: (weird input)
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadDown|ggt_RoundTail(Up/Down)
         * If pByteResetScan==NULL,     Return [pIn, 1]
           Reason: Without rescaning, ggt_RoundMbcsString will take 0xE7C4 as an MBCS Char.
         * If pByteResetScan==pAllText, Return [pIn+1, 0]
           Rescanning produce the correct result.
      Note for this case: If after round-head-down, the head pointer goes after the tail pointer,
      the returned *pResultBytes will always be 0.

	Case 12: (weird input)
        "IBMÀ¶4µçÄÔ"
            ^pIn , InBytes=1 , Flag=ggt_RoundHead(Up/Down)|ggt_RoundTailUp
         * If pByteResetScan==NULL,     Return [pIn, 0]
           Reason: Without rescaning, ggt_RoundMbcsString will take 0xE7C4 as an MBCS Char.
         * If pByteResetScan==pAllText, Return [pIn, 0]
           Rescanning produce the correct result.
      Note for this case: If after round-tail-up, the tail pointer goes before the head pointer,
      the returned *pResultBytes will always be 0.

	</pre>
	*/



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
