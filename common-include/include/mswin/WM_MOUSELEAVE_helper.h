#pragma once

#include <assert.h>
#include <windows.h>

class CWmMouseleaveHelper
{
public:
	CWmMouseleaveHelper(HWND hwnd_to_tack=nullptr) {
		m_is_tracking = false;
		m_hwnd_to_track = hwnd_to_tack;
	}

	void SetHwnd(HWND hwnd_to_track) {
		m_hwnd_to_track = hwnd_to_track;
	}


	enum Move_ret { JustMoving=0, JustEntered=1, MovingError=4 };

	// User call this function in his WM_MOUSEMOVE message.
	//
	Move_ret do_WM_MOUSEMOVE()
	{
		assert(m_hwnd_to_track);

		if (!m_is_tracking)
		{
			// Establish WM_MOUSELEAVE tracking.
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, m_hwnd_to_track };
			BOOL succ = TrackMouseEvent(&tme);
			assert(succ);

			if (succ)
			{
				m_is_tracking = true;
				return JustEntered;
			}
			else
				return MovingError;
		}
		else
			return JustMoving;
	}

	// User call this function in his WM_MOUSELEAVE message.
	//
	void do_WM_MOUSELEAVE()
	{
		m_is_tracking = false;
	}

private:
	bool m_is_tracking;
	HWND m_hwnd_to_track;
};

