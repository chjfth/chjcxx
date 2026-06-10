#ifndef __CHwndTimer_h_
#define __CHwndTimer_h_
#define __CHwndTimer_h_created_ 20250707
#define __CHwndTimer_h_updated_ 20260610

#include <CxxVerCheck.h>
// No .cpp, try to keep all functions inline.

// Child class virtual function as timer callback.

#include <assert.h>
#include <windows.h>

class CHwndTimer
{
public:
	CHwndTimer()
	{
		m_hwnd = NULL;
		m_isTicking = false;

		m_duratype = DuraCount;
		m_need_count = m_done_count = 0;
		m_end_millisec = 0;
	}

	virtual ~CHwndTimer()
	{
		StopTimer();
	}

public:

	void StartPeriodicWorkN(HWND hwnd, int interval_millisec, bool action_now, int do_count)
	{
		return StartPeriodicWork(hwnd, interval_millisec, action_now, DuraCount, do_count, 0);
	}

	void StartPeriodicWorkT(HWND hwnd, int interval_millisec, bool action_now, DWORD dura_millisec)
	{
		return StartPeriodicWork(hwnd, interval_millisec, action_now, DuraTimespan, 0, dura_millisec);
	}

	// Old name: StartTimerOnce
	void StartDelayedWork(HWND hwnd, int interval_millisec)
	{
		StartPeriodicWorkN(hwnd, interval_millisec, false, 1);
	}

	void StopTimer()
	{
		if (!m_isTicking)
		{
			// Don't do repetitive stop-work
			return;
		}
		
		UINT_PTR idEvent = (UINT_PTR)this;
		KillTimer(m_hwnd, idEvent);

		m_isTicking = false;
		m_need_count = m_done_count = 0;
		m_end_millisec = 0;

		TimerOffCallback();
	}

	bool IsTicking()
	{
		return m_isTicking;
	}

protected:
	enum Duration_et { DuraCount = 0, DuraTimespan = 1 };

	// Old name: StartTimer()
	void StartPeriodicWork(HWND hwnd, int interval_millisec, bool action_now,
		 Duration_et duratype, int need_count, DWORD dura_millisec)
	{
		// (duratype==DuraCount && need_count==-1) means infinite count

		if(duratype==DuraCount && need_count==0)
		{
			// User normally won't do this.
			StopTimer();
			return;
		}

		if(!m_hwnd)
		{
			// On first time call, user must provide a valid hwnd.
			// According to WinAPI SetTimer(), only a solid HWND allows arbitrary UINT_PTR as idEvent.

			assert(hwnd);
			assert(IsWindow(hwnd));

			m_hwnd = hwnd;
		}
		else
		{
			// On second call, hwnd can be NULL, so existing m_hwnd is used.

			if (hwnd && m_hwnd != hwnd)
			{
				// Kill timer for old-HWND first, then start for new-HWND.
				StopTimer();

				m_hwnd = hwnd;
			}
		}

		m_duratype = duratype;
		m_need_count = need_count;
		m_done_count = 0;
		m_end_millisec = GetTickCount() + dura_millisec;

		m_isTicking = true;

		if (action_now)
		{
			TimerProc();
			
			if(!m_isTicking)
				return;
		}

		UINT_PTR idEvent = (UINT_PTR)this;
		UINT_PTR idret = SetTimer(m_hwnd, idEvent, interval_millisec, s_TimerProc);
		assert(idret==idEvent);
	}

private:
	static void CALLBACK s_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		CHwndTimer *ptimer = (CHwndTimer*)idEvent;
		assert(ptimer->m_hwnd == hwnd);
		ptimer->TimerProc();
	}

	void TimerProc()
	{
		TimerCallback();
		m_done_count++;

		if (IsTimerDue())
		{
			StopTimer();
		}
	}

	bool IsTimerDue()
	{
		if(m_duratype==DuraCount)
		{
			if (m_need_count == -1 || m_done_count < m_need_count)
				return false;
			else
				return true;
		}
		else
		{
			DWORD now_millisec = GetTickCount();
			if( int(now_millisec - m_end_millisec) > 0 )
				return true;
			else
				return false;
		}
	}

protected:
	// User must override this function to provide timer-callback action.
	virtual void TimerCallback() = 0;
	virtual void TimerOffCallback() {} // user can override this to add "done" work

protected:
	HWND m_hwnd;

	bool m_isTicking;

	Duration_et m_duratype;

	// For DuraCount:
	int m_need_count;
	int m_done_count;

	// For DuraTimeSpan:
	DWORD m_end_millisec;
};

#endif
