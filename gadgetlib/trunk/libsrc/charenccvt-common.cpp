#include <string.h>
#include <gadgetlib/charenccvt.h>


int 
ggt_mbs2wcs(const char *pmb, int mbbytes, wchar_t *pwc, int wcbufchars, int *pConMbBytes)
{
	int consume;
	const char *pmb_adv = pmb; 
	const char *pmb_adv_prev = pmb; 
	const char *pmb_end_ = pmb + (mbbytes<0 ? strlen(pmb)+1 : mbbytes);
	wchar_t *pwc_adv = pwc; 
	const wchar_t *pwc_buf_end_ = pwc + wcbufchars;

	if(mbbytes==0) 
	{
		if(pConMbBytes)
			*pConMbBytes = 0;
		return 0;
	}
	
	for(; pmb_adv<pmb_end_; pmb_adv+=consume)
	{
		wchar_t wc = ggt_mb2wc(pmb_adv, &consume);
		if(wc!=(wchar_t)-1) // conversion success
		{
			if(pwc_adv<pwc_buf_end_)
				*pwc_adv++ = wc;
		}
		else
		{
			break;
		}

		pmb_adv_prev = pmb_adv;
	}
	
	// check for incomplete MBCS char at end situation
	
	if(pmb_adv>pmb_end_)
	{
		pmb_adv = pmb_adv_prev;
		pwc_adv--;
	}

	if(pConMbBytes)
		*pConMbBytes = pmb_adv - pmb;

	return pwc_adv - pwc;

}

int 
ggt_mbs2wcs_go(const char *pmb, int mbbytes, wchar_t *pwc, int wcbufchars, 
	wchar_t badfill, int *pBadCvts, struct ggt_cvt_map_st arBadCvt[], int CvtBufsize)
{
	int mb_consume, wc_consume;
	const char *pmb_adv = pmb; 
//	const char *pmb_adv_prev = pmb; 
	const char *pmb_end_ = pmb + (mbbytes<0 ? strlen(pmb)+1 : mbbytes);
	wchar_t *pwc_adv = pwc; 
	const wchar_t *pwc_buf_end_ = pwc + wcbufchars;
	
	for(;;)
	{
		int mb_this_succ;
		int mb_done = pmb_adv - pmb;
		int wc_done = pwc_adv - pwc;
		mb_consume = ggt_mbs2wcs(
			pmb_adv, mbbytes-mb_done, pwc_adv, wcbufchars-wc_done,
			&mb_this_succ);

//		if(mb_consume!=mb_this_succ)
	}

	return 0;
}

