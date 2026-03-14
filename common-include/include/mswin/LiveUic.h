#ifndef __CHHI__LiveUic_h_20260313_
#define __CHHI__LiveUic_h_20260313_


#include <windows.h>
#include <windowsx.h>
#include <assert.h>

#include <_MINMAX_.h>
#include <mswin/utils_wingui.h>


////////////////////////////////////////////////////////////////////////////
namespace liveuic { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

class LiveUic      // abstract base-class
{
	// LiveUic may be associated with an editbox, a checkbox etc.

public:
	~LiveUic() {}

	virtual void Reset() = 0;
	virtual void DataToUic() = 0;
	virtual void DataFromUic() = 0;
};


template<typename T>
struct Convert;

template<>
struct Convert<int>
{
	static int FromString(const TCHAR* s) { return _ttoi(s); }
	static void ToString(int val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%d"), val); 
	}
};

template<>
struct Convert<double>
{
	static double FromString(const TCHAR* s) { return _ttof(s); }
	static void ToString(double val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%g"), val); 
	}
};

template<> // Chj note: We cannot [omit this `float` specialization and rely only on `double`]
struct Convert<float>
{
	static float FromString(const TCHAR* s) { return (float)_ttof(s); }
	static void ToString(float val, TCHAR buf[], int bufsize) { 
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%g"), val); 
	}
};



template<typename T> // T as user data type in the editbox
class CEditValue : public LiveUic
{
	T m_val;

	T m_default_val, m_min_val, m_max_val;

	HWND m_hedit;

public:
	CEditValue()
	{
		m_val = 0;
		m_default_val = m_min_val = m_max_val = 0;
		m_hedit = NULL;
	}

	void Init(HWND hedit, T default_val, T min_val, T max_val)
	{
		assert(IsWindow(hedit));
		assert(max_val>=min_val);
		assert(default_val>=min_val && default_val<=max_val);

		m_hedit = hedit;

		m_val = default_val;
		m_default_val = default_val;
		m_min_val = min_val;
		m_max_val = max_val;

		DataToUic();
	}

	void SetData(T new_val)
	{
		m_val = _MID_(m_min_val, new_val, m_max_val);
		DataToUic();
	}

	T GetData()
	{
		return m_val;
	}

	virtual void Reset()
	{
		m_val = m_default_val;
		DataToUic();
	}

	virtual void DataToUic()
	{
		TCHAR buf[32] = {};
		Convert<T>::ToString(m_val, buf, ARRAYSIZE(buf));
		SetWindowText(m_hedit, buf);
	}

	virtual void DataFromUic()
	{
		TCHAR buf[32] = {};
		GetWindowText(m_hedit, buf, ARRAYSIZE(buf));
		m_val = Convert<T>::FromString(buf);
	}
};


class CCheckbox : public LiveUic
{
	int m_chkstate; // BST_UNCHECKED=0, BST_CHECKED=1, BST_INDETERMINATE=2

	int m_default_state;

	HWND m_hbutton;

public:
	CCheckbox()
	{
		m_chkstate = m_default_state = BST_UNCHECKED;
	}

	void Init(HWND hbutton, int default_state)
	{
		assert(IsWindow(hbutton));
		m_hbutton = hbutton;
		
		m_default_state = _MID_(BST_UNCHECKED, default_state, BST_INDETERMINATE);
		m_chkstate = m_default_state;

		DataToUic();
	}

	void SetState(int new_state)
	{
		m_chkstate = _MID_(BST_UNCHECKED, new_state, BST_INDETERMINATE);
		DataToUic();
	}

	int GetState()
	{
		return m_chkstate;
	}

	virtual void Reset()
	{
		m_chkstate = m_default_state;
		DataToUic();
	}

	virtual void DataToUic()
	{
		Button_SetCheck(m_hbutton, m_chkstate);
	}

	virtual void DataFromUic()
	{
		m_chkstate = Button_GetCheck(m_hbutton);
	}
};


class CRadioGroup : public LiveUic
{
	int m_uicActive; // 0, 1, 2 ...

	int m_uicDefault;

	int m_uicStart, m_uicEnd;
//	TScalableArray<HWND> m_saHbuttons;
	HWND m_hParent;

public:
	CRadioGroup()
	{
		m_uicActive = m_uicDefault = 0;
		m_uicStart = m_uicEnd = 0;
		m_hParent = NULL;
	}

	void Init(HWND hwndParent, int uicStart, int uicEnd, int uicDefault=0)
	{
		assert(m_hParent==NULL); // blame on twice Init()
		//assert(m_saHbuttons.CurrentEles()==0); // blame on twice Init()
		int nUic = uicEnd - uicStart + 1;
		if(uicDefault<=0)
			uicDefault = uicStart;

		assert(nUic>1);
		assert(uicStart<=uicDefault && uicDefault<=uicEnd);

		for(int i=0; i<nUic; i++)
		{
			HWND hbtn = GetDlgItem(hwndParent, uicStart+i);
			assert(IsWindow(hbtn));

			TCHAR szClassName[10];
			GetClassName(hbtn, szClassName, ARRAYSIZE(szClassName));
			assert(_tcscmp(szClassName, _T("Button"))==0);

			DWORD btnstyle = GetWindowStyle(hbtn) & BS_TYPEMASK;
			assert(btnstyle==BS_RADIOBUTTON || btnstyle==BS_AUTORADIOBUTTON);

//			m_saHbuttons.AppendTail(hbtn);
		}

		m_uicStart = uicStart; m_uicEnd = uicEnd;
		m_uicActive = m_uicDefault = uicDefault;
		m_hParent = hwndParent;

		DataToUic();
	}

	void SetActive(int uicActive)
	{
		assert(m_uicStart<=uicActive && uicActive<=m_uicEnd);
		m_uicActive = _MID_(m_uicStart, uicActive, m_uicEnd);

		DataToUic();
	}

	int GetActive()
	{
		return m_uicActive;
	}

	virtual void Reset()
	{
		m_uicActive = m_uicDefault;
		DataToUic();
	}

	virtual void DataToUic()
	{
		CheckRadioButton(m_hParent, m_uicStart, m_uicEnd, m_uicActive);
	}

	virtual void DataFromUic()
	{
		m_uicActive = getCheckedRadioButton(m_hParent, m_uicStart, m_uicEnd);
		assert(m_uicActive>0);
	}
};


////////////////////////////////////////////////////////////////////////////
} // namespace liveuic
////////////////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++


#if defined(LiveUic_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_LiveUic) // [IMPL]


// >>> Include headers required by this lib's implementation
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
// <<< Include headers required by this lib's implementation




#ifndef LiveUic_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace liveuic {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.





////////////////////////////////////////////////////////////////////////////
} // namespace liveuic
////////////////////////////////////////////////////////////////////////////



#ifndef LiveUic_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__LiveUic_h_20260313_
