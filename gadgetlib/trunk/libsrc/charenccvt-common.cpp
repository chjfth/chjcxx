#include <assert.h>
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
	const char *pmb_end_ = pmb + (mbbytes<0 ? strlen(pmb)+1 : mbbytes);

	wchar_t *pwc_adv = pwc; 
	const wchar_t *pwc_buf_end_ = pwc + wcbufchars;

	if(badfill==0)
		badfill = L'?';
	
	int nBadCvt = 0;

	for(;;)
	{
		int mb_done = pmb_adv - pmb;
		int wc_done = pwc_adv - pwc;

		int mb_this_do = pmb_end_-pmb_adv; //mbbytes-mb_done;

		wc_consume = ggt_mbs2wcs(
			pmb_adv, mb_this_do, pwc_adv, wcbufchars-wc_done,
			&mb_consume);

		pmb_adv += mb_consume;
		pwc_adv += wc_consume;

		assert(mb_consume<=mb_this_do);
		if(mb_consume<mb_this_do) // convert error occurs
		{
			if(nBadCvt<CvtBufsize)
			{
				arBadCvt[nBadCvt].pos_mb = pmb_adv-pmb;
				arBadCvt[nBadCvt].pos_wc = pwc_adv-pwc;
			}
			nBadCvt++;

			pmb_adv++; // skip the bad byte.
			if(pwc_adv<pwc_buf_end_)
				*pwc_adv = badfill;
			pwc_adv++;
		}

		assert(pmb_adv<=pmb_end_);
		if(pmb_adv>=pmb_end_)
			break;
	}

	if(pBadCvts)
		*pBadCvts = nBadCvt;

	return pwc_adv-pwc;
}


static int 
wc_strlen(const wchar_t *pwc)
{
	const wchar_t *pwc_o = pwc;
	for(; *pwc; pwc++);
	return pwc-pwc_o;
}

int 
ggt_wcs2mbs(const wchar_t *pwc, int wcchars, char *pmb, int mbbufbytes, int *pConWcChars)
{
	int consume;
	const wchar_t *pwc_adv = pwc; 
	const wchar_t *pwc_end_ = pwc + (wcchars<0 ? wc_strlen(pwc)+1 : wcchars);

	char *pmb_adv = pmb; 
	const char *pmb_buf_end_ = pmb + mbbufbytes;

	if(wcchars==0)
	{
		if(pConWcChars)
			*pConWcChars = 0;
		return 0;
	}

	for(; pwc_adv<pwc_end_; pwc_adv++)
	{
		int mbbuf_remain = pmb_buf_end_-pmb_adv;
		if(mbbuf_remain>=ggt_max_mbcs_byteslen)
		{	// convert into user buffer directly
			consume = ggt_wc2mb(*pwc_adv, pmb_adv);
			if(consume<=0)
				break;
		}
		else
		{
			char mbbbuf_tmp[ggt_max_mbcs_byteslen];
			consume = ggt_wc2mb(*pwc_adv, mbbbuf_tmp);
			if(consume<=0)
				break;

			if(consume<=mbbuf_remain)
				memcpy(pmb_adv, mbbbuf_tmp, consume);
		}
		pmb_adv += consume;
	}

	if(pConWcChars)
		*pConWcChars = pwc_adv - pwc;

	return pmb_adv - pmb;
}

int ggt_wcs2mbs_go(const wchar_t *pwc, int wcchars, char *pmb, int mbbufbytes, 
	const char *pszbadfill, int *pBadCvts, struct ggt_cvt_map_st arBadCvt[], int CvtBufsize)
{
	int mb_consume, wc_consume;
	const wchar_t *pwc_adv = pwc;
	const wchar_t *pwc_end_ = pwc + (wcchars<0 ? wc_strlen(pwc)+1 : wcchars);

	char *pmb_adv = pmb;
	const char *pmb_buf_end_ = pmb + mbbufbytes;

	static const char s_dftBadfill[2] = "?";
	if(!pszbadfill || !pszbadfill[0])
		pszbadfill = s_dftBadfill;

	int i;
	int nBadCvt = 0;

	for(;;)
	{
		int wc_done = pwc_adv - pwc;
		int mb_done = pmb_adv - pmb;

		int wc_this_do = pwc_end_-pwc_adv;

		mb_consume = ggt_wcs2mbs(
			pwc_adv, wc_this_do, pmb_adv, mbbufbytes-mb_done,
			&wc_consume);

		pwc_adv += wc_consume;
		pmb_adv += mb_consume;

		assert(wc_consume<=wc_this_do);
		if(wc_consume<wc_this_do) // convert error occurs
		{
			if(nBadCvt<CvtBufsize)
			{
				arBadCvt[nBadCvt].pos_mb = pmb_adv-pmb;
				arBadCvt[nBadCvt].pos_wc = pwc_adv-pwc;
			}
			nBadCvt++;

			pwc_adv++; // skip the bad Char
			for(i=0; pszbadfill[i]; i++)
			{
				if(pmb_adv<pmb_buf_end_)
					*pmb_adv = pszbadfill[i];
				pmb_adv++;
			}
		}

		assert(pwc_adv<=pwc_end_);
		if(pwc_adv>=pwc_end_)
			break;
	}

	if(pBadCvts)
		*pBadCvts = nBadCvt;

	return pmb_adv - pmb;
}
