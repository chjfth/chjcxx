#ifndef __CHwndTimer_h_20250707_
#define __CHwndTimer_h_20250707_

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
		m_isOnce = false;
	}

	virtual ~CHwndTimer()
	{
		StopTimer();
	}

public:

	void StartTimer(HWND hwnd, int interval_millisec, bool action_now=false)
	{
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

		UINT_PTR idEvent = (UINT_PTR)this;
		UINT_PTR idret = SetTimer(m_hwnd, idEvent, interval_millisec, s_TimerProc);
		assert(idret==idEvent);

		if(action_now)
			TimerCallback();
	}

	void StartTimerOnce(HWND hwnd, int interval_millisec)
	{
		m_isOnce = true;
		StartTimer(hwnd, interval_millisec, false);
	}

	void StopTimer()
	{
		UINT_PTR idEvent = (UINT_PTR)this;
		if (idEvent)
		{
			KillTimer(m_hwnd, (UINT_PTR)this);
		}
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
		if (m_isOnce)
		{
			StopTimer();
		}

		TimerCallback(); // User can call StartTimerOnce() inside.
	}

protected:
	// User must override this function to provide timer-callback action.
	virtual void TimerCallback() = 0;

protected:
	HWND m_hwnd;
//	UINT_PTR m_timerId; // =this
	bool m_isOnce;

};

#endif
