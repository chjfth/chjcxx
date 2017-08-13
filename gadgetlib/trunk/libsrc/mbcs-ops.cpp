#include "stdafx.h"

#include <gadgetlib/charenccvt.h>
#include <gadgetlib/mbcs-ops.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__mbcs_ops__DLL_AUTO_EXPORT_STUB(void){}

static int 
s_DefaultCharNext(const char *pChk, void *pCallbackExtra)
{
	(void)pCallbackExtra;
	if(*pChk=='\0')
		return 1;

	const char *pnext = ggt_charnext(pChk);
	assert(pnext>pChk);
	return (int)(pnext-pChk);
}

char *ggt_RoundMbcsString(const char *pIn, int InBytes, const char *pStartScan,
	ggt_RoundMbcs_et Flag, int *pResultBytes, int *pResultChars,  
	ggt_PROC_CharNext procCharNext, void *pCallbackExtra)
{
	ggt_PROC_CharNext procCN = procCharNext ? procCharNext : s_DefaultCharNext;
	void *pcbeCN = pCallbackExtra ? pCallbackExtra : NULL;

	int nResultChars = 0;

	const char *pFinalHead = pIn; // pFinalHead will act as ggt_RoundMbcsString's ret-value.
	const char *pcur = pIn; // cur: current scanning position,
	const char *pInEnd_ = pIn+InBytes;
	const char *pnext = NULL; // as a temp var
	const char *pFinalEnd_ = NULL; // modify later

	if(pStartScan && pStartScan<pIn)
	{
		// Find a new pFinaHead
		pcur = pStartScan;
		for(;;)
		{
			pnext = pcur + procCN(pcur, pcbeCN);
			if(pnext>=pIn)
				break;
			pcur = pnext;
		}

		if(pnext==pIn)
			pFinalHead = pnext;
		else // pnext>pIn
		{
			if(Flag&ggt_RoundHeadUp)
				pFinalHead = pcur;
			else
				pFinalHead = pnext;
		}

		pcur = pFinalHead; // prepare to scan tail
	}
	// Now, pFinalHead has established.

	if(pcur<pInEnd_)
	{
		for(; pcur<pInEnd_; )
		{
			pnext = pcur + procCN(pcur, pcbeCN);
			if(pnext>pInEnd_)
				break;
			pcur = pnext;
			nResultChars++;
		}

		if(pnext==pInEnd_)
			pFinalEnd_ = pnext;
		else
		{
			if(Flag&ggt_RoundTailUp)
				pFinalEnd_ = pcur;
			else 
				pFinalEnd_ = pnext, nResultChars++;
		}
	}
	else // the resulting head position already surpass the tail(regardless ggt_RoundTail(Up/Down)
	{
		pFinalEnd_ = pFinalHead;
	}

	if(pResultBytes)
		*pResultBytes = (int)(pFinalEnd_ - pFinalHead);

	if(pResultChars)
		*pResultChars = nResultChars;

	return (char*)pFinalHead;
}

