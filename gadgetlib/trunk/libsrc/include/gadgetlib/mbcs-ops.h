#ifndef __mbcs_ops_h_20081231_
#define __mbcs_ops_h_20081231_

#ifdef __cplusplus
extern"C" {
#endif

//! Flags saying how to round an MBCS or UTF-8 string
enum ggt_RoundMbcs_et {
	ggt_RoundHeadUp = 1, ggt_RoundHeadDown = 0,
	ggt_RoundTailUp = 2, ggt_RoundTailDown = 0,
};


typedef int (*ggt_PROC_CharNext)(const char *pChk, void *pCallbackExtra);
	//!< The callback function type used with ggt_RoundMbcsString, to recognize the next MBCS char,
	/*!< This function checks the chars pointed to by pChk to find the next MBCS 
	 character in the string, or to skip the current invalid MBCS sequence.
		The first char by pChk may not be the lead-byte of an MBCS character; the function
	 implementer should be aware of this, and skip the incomplete/invalid MBCS byte sequence to
	 find the next valid MBCS character.

	@param[in] pChk
		Points to the string/text to check.

	@return
		Returns how many bytes to skip in order to meet the next character.
		* If pChk points to a SBCS char, returns 1.
		* If pChk points to a DBCS char, returns 2.
		* If pChk points to a UTF-8 char, 1 to 6 may be returned.
			\n
		Note: If pChk points to NULL char, 1 is returned. 
		So, this function will not return 0 or negative.

		Either case, by adding the ret-value to pChk, the caller knows where the next MBCS character is.
	*/

char *ggt_RoundMbcsString(const char *pIn, int InBytes, const char *pStartScan,
		ggt_RoundMbcs_et Flag, int *pResultBytes, int *pResultChars,  
		ggt_PROC_CharNext procCharNext=0, void *pCallbackExtra=0);
	//!< Check a chunk of MBCS byte sequence, to recognize the correct MBCS characters in it.
	/*!< Usage scenario: User has a byte sequence by pIn and InBytes, but pIn may not point
	 to the lead-byte of an MBCS character and pIn+InBytes may not point to the trail-byte of
	 an MBCS char. So, the user calls this function to find the correct MBCS lead-byte and the
	 correct MBCS trail-byte.

	 Note: NUL char in the input byte stream is considered a normal SBCS char so not treated
	 specially.

	@param[in] pIn
		Points to user input byte sequence.

	@param[in] InBytes
		Tells how many bytes are given by pIn.
		Of course, there is no need to have any NUL-char after InBytes.

	@param[in] pStartScan
		A pointer that is known to correctly point to an MBCS lead-byte or a SBCS byte.
		This pointer must be ahead of(lower address) pIn. 
		Rationale: When user
			* is uncertain whether pIn points to a valid lead-byte, or
			* requests ggt_RoundHeadUp flag, 
		ggt_RoundMbcsString can go back scanning from pStartScan to find which byte is
		the correct lead-byte around pIn, or which is the MBCS character ahead of pIn.
		Without such scanning, we cannot know the accurate answer.
			\n
		If NULL, no such scanning will be carried out, so the request of ggt_RoundHeadUp
		will be ignored.
			\n
		If not NULL, the scanning from pStartScan will always be carried out so as
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

	@param[out] pResultChars
		Outputs characters(not bytes) of the adjusted MBCS string.

	@param[in] proc
		This callback provides the routine that can tell what bytes constitute an MBCS character.
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
		    const char *pAllText = "IBMÀ¶4µçÄÔ"; // (the MBCS string is GBK, codepage 936)
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
         In this case, input head points to an MBCS trail-byte, and it does not make up another 
         MBCS Char with the byte following it(0x34). So,
         * If pStartScan==NULL, two sub-cases. 
		   ggt_RoundMbcsString recognize 0xB6 is an MBCS lead-byte, so it calls system function
		   procCharNext to determine if 0x34 is a trail-byte. 
		   - Sub-case1: 0x34 is reported as a trail-byte, 
		     Return [pIn, 2] // Sigh, it results in an invalid MBCS sequence.
		   - Sub-case2: 0x34 is not reported as a trail-byte, 0x34 is considered the next valid MBCS Char.
             Return [pIn+1, 1]
         * If pStartScan==pAllText, ggt_RoundMbcsString will do a scan from pAllText and
           deduce that 0xCOB6 is an MBCS Char, so next MBCS Char is 0x34.
           Return [pIn+1, 1]

	Case 5:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=2 , Flag=ggt_RoundHeadUp
         Similar to Case 4, only Flag different. 
         * If pStartScan==NULL, so ggt_RoundMbcsString don't know how to find the MBCS Char
           just ahead of pIn, so he leave the head position intact.
           Return [pIn, 2]
         * If pStartScan==pAllText, ggt_RoundMbcsString will do a scan from pAllText and
           deduce that 0xCOB6 is an MBCS Char, so the head is rounded UP.
           Return [pIn-1, 3]
         
        Caveat from Case 4 and 5: pStartScan==NULL leaves a "bad" output, so, when you are not certain
        whether pIn already points to a lead-byte, you should give pStartScan to make a safe scan.

	Case 6:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadUp
         Similar to Case 5, but InBytes=1. (no new tricky thing for this case)
         * If pStartScan==NULL,
           - If 0xB634 is reported as one MBCS char,     return [pIn, 2] .
           - If 0xB6 alone is reported as one MBCS char, return [pIn, 1] .
         * If pStartScan==pAllText, 
           Return [pIn-1, 2]

	Case 7:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadDown
         Similar to Case 4, but InBytes=1.
         * If pStartScan==NULL, 
           - If 0xB634 is reported as one MBCS char,     return [pIn, 2] .
           - If 0xB6 alone is reported as one MBCS char, return [pIn, 1] .
         * If pStartScan==pAllText, 
           Return [pIn+1, 0]

	Case 8:
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=3 , Flag=ggt_RoundHeadUp|ggt_RoundTailDown
         * If pStartScan==NULL,     Return [pIn, 4]
         * If pStartScan==pAllText, Return [pIn-1, 5]

	Case 10:
        "IBMÀ¶4µçÄÔ"
                ^pIn , InBytes=3 , Flag=ggt_RoundHeadUp|ggt_RoundTailDown
         * If pStartScan==NULL,     Return [pIn, 3]
           Reason: Without rescaning, ggt_RoundMbcsString will take 0xE7C4 as an MBCS Char.
         * If pStartScan==pAllText, Return [pIn-1, 4]
           Rescanning produce the correct result.
        
	Case 11: (weird input)
        "IBMÀ¶4µçÄÔ"
             ^pIn , InBytes=1 , Flag=ggt_RoundHeadDown|ggt_RoundTailUp
		 Similar to Case 7, but ggt_RoundTailUp
         * If pStartScan==NULL,     //xxxxxx Return [pIn, 1]
           - If 0xB634 is reported as one MBCS char,     return [pIn, 0] .
           - If 0xB6 alone is reported as one MBCS char, return [pIn, 1] .
         * If pStartScan==pAllText, Return [pIn+1, 0]
      Note for this case: If after round-head-down, the head pointer goes behind 
	  the rounded(up/down) tail pointer, the returned *pResultBytes will always be 0.

	Case 12: (weird input)
        "IBMÀ¶4µçÄÔ"
            ^pIn , InBytes=1 , Flag=ggt_RoundHead(Up/Down)|ggt_RoundTailUp
         * If pStartScan==NULL,     Return [pIn, 0]
         * If pStartScan==pAllText, Return [pIn, 0]
      Note for this case: If after round-tail-up, the tail pointer goes ahead of the head pointer,
      the returned *pResultBytes will always be 0.

	</pre>
	*/
	


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
