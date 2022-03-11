#include <stdio.h>
#include <gadgetlib/charenccvt.h>

// This tests program gives correct result when current codepage is 936 (GBK).

static int 
wc_strlen(const wchar_t *pwc)
{
	const wchar_t *pwc_o = pwc;
	for(; *pwc; pwc++);
	return pwc-pwc_o;
}

int test_mb2uc()
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

#define MYTEXT1 "我a们的"  // len = 4 Chars
#define MAKEWCHAR_(text) L ## text
#define MAKEWCHAR(text) MAKEWCHAR_(text)
	//[2008-09-01] Must add an intermediate level for L ## text .

int test_uc2mb()
{

	const wchar_t strwc1[] = MAKEWCHAR(MYTEXT1);
	const char strmb1_correct[] = MYTEXT1;
	int mblen_correct = strlen(strmb1_correct);
//	char strmb1[] = {0};

	
//	int nwcret = 0;
//	int mbbytes = 0;
	const int mbbufsz1 = 20;
	char mbbuf1[mbbufsz1] = {0};

	//// Check ggt_wc2mb
	
	int nmb1 =ggt_wc2mb(strwc1[0], mbbuf1);
	if( memcmp(mbbuf1, strmb1_correct, 2)!=0 )
	{
		printf("Error! ggt_wc2mb give error result %s.\n", mbbuf1);
	}

	//// Check ggt_wcs2mbs , -1 as wcchars param.

	int wstrlen = wc_strlen(strwc1);
	int nConChars = 0;
//	wchar_t buf1[mbbufsz1];
	int nmbret = ggt_wcs2mbs(strwc1, -1, mbbuf1, mbbufsz1, &nConChars);
	if(!( nmbret==mblen_correct+1 && nConChars==wstrlen+1 &&
		memcmp(strmb1_correct, mbbuf1, nmbret)==0 ))
	{
		printf("Error! ggt_wcs2mbs (case 1) give error result!\n");
	}

	//// Check ggt_wcs2mbs , explicit number as wcchars param.

	nmbret = ggt_wcs2mbs(strwc1, wstrlen, mbbuf1, mbbufsz1, &nConChars);
	if(!( nmbret==mblen_correct && nConChars==wstrlen &&
		memcmp(strmb1_correct, mbbuf1, nmbret)==0 ))
	{
		printf("Error! ggt_wcs2mbs (case 2) give error result!\n");
	}

	//// Check ggt_wcs2mbs , stop at an un-convertable Char.

	const wchar_t strwc2[] = L"我a\xD574们";
	const int wstrlen2 = sizeof(strwc2)/sizeof(strwc2[0]);
	const int mbbufsz2 = 20;
	char mbbuf2[mbbufsz2] = {0};
	nmbret = ggt_wcs2mbs(strwc1, -1, mbbuf2, mbbufsz2, &nConChars);
	if(!( nmbret==3 && nConChars==2 ))
	{
		printf("Error! ggt_wcs2mbs (case 3) give error result!\n");
	}

	//// Check ggt_wcs2mbs_go 

	char arMbBadFill[] = "？"; // GBK value 0xA3BF
	int nBadCvt = 0;
	ggt_cvt_map_st arCvtMap[wstrlen2];
	nmbret = ggt_wcs2mbs_go(strwc2, -1, mbbuf2, mbbufsz2, arMbBadFill,
		&nBadCvt, arCvtMap, wstrlen2);
	int mbbadpos0 = arCvtMap[0].pos_mb;
	if(!( nmbret==8
		&& nBadCvt==1 && arCvtMap[0].pos_wc==2 && mbbadpos0==3
		&& mbbuf2[mbbadpos0]==arMbBadFill[0] && mbbuf2[mbbadpos0+1]==arMbBadFill[1]
		))
	{
		printf("Error! ggt_wcs2mbs_go  give error result!\n");
	}

	return 0;
}

int main()
{
	test_mb2uc();

	test_uc2mb();

	return 0;
}
