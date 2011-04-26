#include <stdio.h>
#include <gadgetlib/charenccvt.h>
#include <gadgetlib/mbcs-ops.h>

static const char s[] = "IBM" "\xC0\xB6" "4" "\xB5\xE7\xC4\xD4";
	// "IBMÀ¶4µçÄÔ" in GBK

struct STest
{
	const char *pIn;
	int InBytes;
	const char *pStartScan;
	bool isGreedyBadMbcs;
	ggt_RoundMbcs_et Flags;
	
	// output to compare
	const char *pOut;
	int nResultBytes, nResultChars;
};

STest gar_test[] = 
{
	//Case 1:
	{ s+1, 2, s, true, ggt_RoundBothDown,  s+1, 2,2 },

	//Case 2:
	{ s+1, 3, s, true, ggt_RoundTailDown,  s+1, 4,3 },

	//Case 3:
	{ s+1, 3, s, true, ggt_RoundTailUp,  s+1, 2,2 },

	//Case 4:
    { s+4, 2, NULL, true, ggt_RoundHeadDown, s+4, 2,1 },
    { s+4, 2, NULL, false,ggt_RoundHeadDown, s+4, 2,2 },
    { s+4, 2, s,    true, ggt_RoundHeadDown, s+5, 1,1 },

	//Case 5:
	{ s+4, 2, NULL, true, ggt_RoundHeadUp, s+4, 2,1 },
	{ s+4, 2, NULL, false,ggt_RoundHeadUp, s+4, 2,2 },
	{ s+4, 2, s   , true, ggt_RoundHeadUp, s+3, 3,2 },

	//Case 6:
	{ s+4, 1, NULL, true, ggt_RoundHeadUp, s+4, 2,1 },
	{ s+4, 1, NULL, false,ggt_RoundHeadUp, s+4, 1,1 },
	{ s+4, 1, s,    true, ggt_RoundHeadUp, s+3, 2,1 },

	//Case 7:
	{ s+4, 1, NULL, true, ggt_RoundHeadDown, s+4, 2,1 },
	{ s+4, 1, NULL, false,ggt_RoundHeadDown, s+4, 1,1 },
	{ s+4, 1, s,    true, ggt_RoundHeadDown, s+5, 0,0 },

	//Case 8:
	{ s+4, 3, NULL, true, ggt_RoundHeadUp, s+4, 4,2 },
	{ s+4, 3, NULL, false,ggt_RoundHeadUp, s+4, 4,3 },
	{ s+4, 3, s,    true, ggt_RoundHeadUp, s+3, 5,3 },

	//Case 10:
	{ s+7, 3, NULL, true, ggt_RoundHeadUp, s+7, 3,2 },
	{ s+7, 3, s,    true, ggt_RoundHeadUp, s+6, 4,2 },
        
	//Case 11: (weird input)
	{ s+4, 1, NULL, true, ggt_RoundTailUp, s+4, 0,0 },
	{ s+4, 1, NULL, false,ggt_RoundTailUp, s+4, 1,1 },
	{ s+4, 1, s,    true, ggt_RoundTailUp, s+5, 0,0 },

	//Case 12: (weird input)
	{ s+3, 1, NULL, true, ggt_RoundBothUp, s+3, 0,0 },
	{ s+3, 1, s,    true, ggt_RoundBothUp, s+3, 0,0 },
};

int g_tests = sizeof(gar_test)/sizeof(gar_test[0]);


int ProcCharNextGreedy(const char *pChk, void *pCallbackExtra)
{
	const char *pret = ggt_charnext(pChk);
	return pret-pChk;
}

int ProcCharNextNonGreedy(const char *pChk, void *pCallbackExtra)
{
	const char *pret = ggt_charnext(pChk);
	int skip = pret-pChk;
	if(skip>1 && ggt_mb2wc(pChk,NULL)==(wchar_t)-1)
		skip = 1;
	
	return skip;
}

int main()
{
	int nResultBytes, nResultChars;
	int i;
	int isErr = 0;
	for(i=0; i<g_tests; i++)
	{
		ggt_PROC_CharNext procCharNext = gar_test[i].isGreedyBadMbcs
			? ProcCharNextGreedy : ProcCharNextNonGreedy;

		char *pRet = ggt_RoundMbcsString(
			gar_test[i].pIn, gar_test[i].InBytes, gar_test[i].pStartScan,
			gar_test[i].Flags, &nResultBytes, &nResultChars,  
			procCharNext, NULL);

		if(! (pRet==gar_test[i].pOut 
			&& nResultBytes==gar_test[i].nResultBytes
			&& nResultChars==gar_test[i].nResultChars) )
		{
			printf("ggt_RoundMbcsString error at test index %d.\n", i);
			isErr = 1;
		}
	}

	if(!isErr)
		printf("All %d test cases of ggt_RoundMbcsString succeed.\n", g_tests);

	return 0;
}
