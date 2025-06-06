#ifndef __CalSpeed_h_20250429_
#define __CalSpeed_h_20250429_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <commdefs.h>


class CCalSpeed
{
public:
	typedef Uint (*PROC_OsMillisec)(void);
	
	CCalSpeed(PROC_OsMillisec _procGetmsec)
	{
		m_procGetmsec = _procGetmsec;
		Reset();
	};
	
	void Reset();
	
	void Accum(Uint bytes);
	
	Uint64 GetTotal()
	{
		return m_nowTotal;
	}
	
	Uint GetSpeed(Uint64 *pDiffBytes)
	{
		Uint now_msec = m_procGetmsec();
		Uint diff_msec = now_msec - m_msecPrev;
		
		m_msecPrev = now_msec;

		*pDiffBytes = m_nowTotal - m_prevTotal;
		m_prevTotal = m_nowTotal;

		return diff_msec;
	}
	
	bool GetNextSpeed(Uint NeedIntervalMsec, Uint *pDiffMsec, Uint64 *pDiffBytes)
	{
		Uint now_msec = m_procGetmsec();
		if (now_msec - m_msecPrev <= NeedIntervalMsec)
			return false;
		
		*pDiffMsec = GetSpeed(pDiffBytes);
		return true;		
	}
	
	static TCHAR *sprint(Uint diffmsec, Uint64 diffbytes64, TCHAR *buf, int bufsize);
	
	static void SetThousepChar(TCHAR c);
	static TCHAR GetThousepChar();

private:
	Uint64 m_nowTotal;
	
	Uint64 m_prevTotal;
	Uint m_msecPrev;
		
	PROC_OsMillisec m_procGetmsec;
};

///////////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION CODE BELOW ////////////////////////
///////////////////////////////////////////////////////////////////////////

#ifdef CalSpeed_IMPL

#ifdef CalSpeed_DEBUG
#undef vaDBG
#else
#define vaDBG(...) // make vaDBG empty, no debugging message
#endif


static TCHAR s_thousepc = _T('.');

void CCalSpeed::SetThousepChar(TCHAR c)
{
	s_thousepc = c;
}

TCHAR CCalSpeed::GetThousepChar()
{
	return s_thousepc;
}

void CCalSpeed::Reset()
{
	m_nowTotal = m_prevTotal = 0;
	m_msecPrev = m_procGetmsec();

	vaDBG(_T("[CalSpeed] @%p Reset() millisec *%u"), this, m_msecPrev);
}

void CCalSpeed::Accum(Uint bytes)
{
	Uint64 oldtotal = m_nowTotal;

	m_nowTotal += bytes;

	vaDBG(_T("[CalSpeed] @%p *%u: Accum(low-32bit) %u -> %u"), 
		this, m_procGetmsec(), 
		(Uint)oldtotal, (Uint)m_nowTotal);
}


TCHAR *CCalSpeed::sprint(Uint diffmsec, Uint64 diffbytes64, TCHAR *buf, int bufsize)
{
	assert(buf || bufsize<=0);
	
	if (diffmsec == 0)
	{
		if (bufsize > 0)
			buf[0] = '\0';
		return buf;
	}

	// We keep 3 effective digits for the speed representation.
	
	const int _100 = 100, _10 = 10;
	int r = 0;
	
	Uint64 KBps = diffbytes64 / diffmsec;
	
	if (KBps == 0) // XXX Bps
	{
		Uint64 Bps = diffbytes64 * 1000 / diffmsec;
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u B/s"), (Uint)Bps);
		return buf;
	}
	else if (KBps < 1000) // XXX KBps
	{
		if (KBps < 10)
		{
			r = (diffbytes64*_100) / diffmsec % _100;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%02u KB/s"), (Uint)KBps, s_thousepc, r);
		}
		else if (KBps < 100)
		{
			r = (diffbytes64*_10) / diffmsec % _10;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%01u KB/s"), (Uint)KBps, s_thousepc, r);
		}
		else 
		{
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u KB/s"), (Uint)KBps);
		}
		return buf;
	}
	else if (KBps < 1000 * 1000) // XXX MBps
	{
		Uint64 MBps = KBps / 1000;
		Uint64 diffKB = diffbytes64 / 1000;
		if (MBps < 10)
		{
			r = (diffKB*_100) / diffmsec % _100;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%02u MB/s"), (Uint)MBps, s_thousepc, r);
		}
		else if (MBps < 100)
		{
			r = (diffKB*_10) / diffmsec % _10;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%01u MB/s"), (Uint)MBps, s_thousepc, r);
		}
		else 
		{
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u MB/s"), (Uint)MBps);
		}
		return buf;
	}
	else // XXX GBps 
	{
		Uint GBps = (Uint)(KBps / (1000*1000));
		Uint diffMB = (Uint)(diffbytes64 / (1000*1000));
		if(GBps<10)
		{
			r = (diffMB*_100) / diffmsec % _100;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%02u GB/s"), (Uint)GBps, s_thousepc, r);
		}
		else if (GBps < 100)
		{
			r = (diffMB*_10) / diffMB % _10;
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u%c%01u GB/s"), (Uint)GBps, s_thousepc, r);
		}
		else
		{
			_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%u GB/s"), (Uint)GBps);
		}

		return buf;
	}
	
}


#endif // CalSpeed_IMPL

#endif
