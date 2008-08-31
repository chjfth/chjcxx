#include <stdio.h>
#include <gadgetlib/charenccvt.h>

// This tests program gives correct result when current codepage is 936 (GBK).

int main()
{
	const char strmb1[] = "我a们的";
	const wchar_t strwc1[] = {0x6211, 0x61, 0x4eec, 0x7684, 0}; // the correct convert result of strmb1[]
	wchar_t wc1 = 0;
	int nwcret = 0;
	int mbbytes = 0;

	//// Check ggt_mb2wc

	wc1 = ggt_mb2wc(strmb1, &mbbytes);
	// check result
	if(wc1!=0x6211)
	{
		printf("Error! ggt_mb2wc give error result 0x%X.\n", wc1);
	}

	//// Check ggt_mbs2wcs , -1 as mbbytes param.

	int astrlen = strlen(strmb1);
	int nConBytes = 0;
	const int wcbufsz1 = 6;
	wchar_t wcbuf1[wcbufsz1];
	nwcret = ggt_mbs2wcs(strmb1, -1, wcbuf1, wcbufsz1, &nConBytes);
	if(!( nwcret==5 && nConBytes==astrlen+1 &&
		memcmp(strwc1, wcbuf1, 5*sizeof(wchar_t))==0 ))
	{
		printf("Error! ggt_mbs2wcs (case 1) give error result!\n");
	}

	//// Check ggt_mbs2wcs , explicit number as mbbytes param.

	nwcret = ggt_mbs2wcs(strmb1, astrlen, wcbuf1, wcbufsz1, &nConBytes);
	if(!( nwcret==4 && nConBytes==astrlen &&
		memcmp(strwc1, wcbuf1, 4*sizeof(wchar_t))==0 ))
	{
		printf("Error! ggt_mbs2wcs (case 2) give error result!\n");
	}

	//// Check ggt_mbs2wcs , cut at half MBCS char
#define CUT_MBCS_AT 4 // cut at 们
	nwcret = ggt_mbs2wcs(strmb1, CUT_MBCS_AT, wcbuf1, wcbufsz1, &nConBytes);
	if(!( nwcret==2 && nConBytes==CUT_MBCS_AT-1 ))
	{
		printf("Error! ggt_mbs2wcs (case 3) give error result!\n");
	}

	//// Check ggt_mbs2wcs , stop at illegal byte sequence.

	const char strmb2[] = "我a\xEE\x31们"; // 0x31 is illegal trail-byte here!
	const wchar_t strwc2[] = {0x6211, 0x61, 0x4eec, 0}; // the correct convert result of strmb1[]
//	astrlen = strlen(strmb2);
	const int wcbufsz2 = 6;
	wchar_t wcbuf2[wcbufsz2];

	nwcret = ggt_mbs2wcs(strmb2, -1, wcbuf2, wcbufsz2, &nConBytes);
	if(!( nwcret==2 && nConBytes==3 ))
	{
		printf("Error! ggt_mbs2wcs (case 4) give error result!\n");
	}

	//// Check ggt_mbs2wcs_go 

	wchar_t badfill = '#';
	int nBadCvt = 0;
	ggt_cvt_map_st arCvtMap[wcbufsz2];
	nwcret = ggt_mbs2wcs_go(strmb2, -1, wcbuf2, wcbufsz2, badfill,
		&nBadCvt, arCvtMap, wcbufsz2);
	if(!( nwcret==6
		&& nBadCvt==1 && arCvtMap[0].pos_wc==2 && arCvtMap[0].pos_mb==3
		&& wcbuf2[arCvtMap[0].pos_wc]==badfill 
		))
	{
		printf("Error! ggt_mbs2wcs_go give error result!\n");
	}
	
	printf("Test done. If no error prompt above, the test is successful.\n");

	return 0;
}
