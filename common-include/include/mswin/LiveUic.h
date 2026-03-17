#ifndef __CHHI__LiveUic_h_20260313_
#define __CHHI__LiveUic_h_20260313_


#include <windows.h>
#include <windowsx.h>
#include <assert.h>

#include <_MINMAX_.h>
#include <EnsureClnup.h>
#include <vaDbgTs.h>
#include <mswin/WinUser.itc.h>
#include <mswin/utils_wingui.h>
#include <mswin/CxxWindowSubclass.h>
#include <TScalableArray.h>


////////////////////////////////////////////////////////////////////////////
namespace liveuic { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

static const TCHAR *sigwmDataChanged = _T("LiveUic-DataChanged"); // pass to RegisterWindowMessage ()
extern const UINT wmDataChanged;

class LiveUic      // abstract base-class
{
	// LiveUic may be associated with an editbox, a checkbox etc.

public:
	~LiveUic() {}

	virtual void Reset() = 0;
	virtual void DataToUic() = 0;
	virtual void DataFromUic() = 0;
};


class DlgboxPeeker : public CxxWindowSubclass
{
//	HWND m_hdlg;
	TScalableArray<LiveUic*> m_saLiveUics;

public:
//	DlgboxPeeker() { m_hdlg=NULL; }

	static bool AddUic(HWND hUic, LiveUic* plu);

	static bool AddUic(HWND hdlg, int uic, LiveUic* plu) {
		return AddUic(GetDlgItem(hdlg, uic), plu);
	}
	
	static bool DelUic(HWND hUic, LiveUic* plu);

	static bool DelUic(HWND hdlg, int uic, LiveUic* plu) {
		return DelUic(GetDlgItem(hdlg, uic), plu);
	}

	void ResetAllUicContent();
};

static const TCHAR *sigDlgboxPeeker = _T("liveuic::DlgboxPeeker");

inline DlgboxPeeker* GetDlgboxPeeker(HWND hdlg, BOOL is_create=FALSE, 
	CxxWindowSubclass::ReCode_et *pErr=nullptr)
{
	DlgboxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<DlgboxPeeker>(
		hdlg, sigDlgboxPeeker, is_create, pErr);
	return peeker;
}


////////////////////////////////////////////////////////////////////////////
//              LiveUic classes for various Window controls               //
////////////////////////////////////////////////////////////////////////////


template<typename T> struct Convert;   // type conversion helper

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
class CEditValue : public LiveUic, public CxxWindowSubclass
{
	T m_val;

	T m_default_val, m_min_val, m_max_val;

	HWND m_hedit;
	int m_uic;

public:
	CEditValue()
	{
		m_val = 0;
		m_default_val = m_min_val = m_max_val = 0;
		
		m_hedit = NULL;
		m_uic = 0;
	}

	void Init(HWND hedit, T default_val, T min_val, T max_val)
	{
		assert(!m_hedit); // blame on twice Init()

		assert(IsWindow(hedit));
		assert(max_val>=min_val);
		assert(default_val>=min_val && default_val<=max_val);

		m_hedit = hedit;
		m_uic = GetDlgCtrlID(hedit);

		m_val = default_val;
		m_default_val = default_val;
		m_min_val = min_val;
		m_max_val = max_val;

		DataToUic();

		auto err = CxxWindowSubclass::FetchCxxobjFromHwnd_as_partial(m_hedit,
			_T("liveuic::CEditValue"), this);
		assert(!err);
		// vaDbgTs(_T("hedit = 0x%08X , this=%p"), m_hedit, this);

		DlgboxPeeker::AddUic(hedit, this);
	}

	void SetValue(T new_val)
	{
		m_val = _MID_(m_min_val, new_val, m_max_val);
		DataToUic();
	}

	T GetValue()
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

		assert(wmDataChanged);
		HWND hdlg = GetParent(m_hedit);
		::SendMessage(hdlg, wmDataChanged, m_uic, 0);
	}

protected: // from CxxWindowSubclass
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lret = DefSubclassProc(hwnd, uMsg, wParam, lParam);
		//vaDbgTs(_T("CEditValue peek: uMsg=%s"), ITCSvn(uMsg, itc::EM_xxx));
		if(uMsg==WM_CHAR || uMsg==WM_SETTEXT )
		{
			// User type-in or delete new text || EXE code set text via API.
			DataFromUic();
		}
		return lret;
	}
};


class CCheckbox : public LiveUic, public CxxWindowSubclass
{
	int m_chkstate; // BST_UNCHECKED=0, BST_CHECKED=1, BST_INDETERMINATE=2

	int m_default_state;

	HWND m_hbutton;
	int m_uic;

public:
	CCheckbox()
	{
		m_chkstate = m_default_state = BST_UNCHECKED;
		m_hbutton = NULL;
		m_uic = 0;
	}

	void Init(HWND hbutton, int default_state)
	{
		assert(!m_hbutton); // blame on twice Init()

		assert(IsWindow(hbutton));
		m_hbutton = hbutton;
		m_uic = GetDlgCtrlID(hbutton);
		
		m_default_state = _MID_(BST_UNCHECKED, default_state, BST_INDETERMINATE);
		m_chkstate = m_default_state;

		DataToUic();

		auto err = CxxWindowSubclass::FetchCxxobjFromHwnd_as_partial(m_hbutton,
			_T("liveuic::CCheckbox"), this);
		assert(!err);

		DlgboxPeeker::AddUic(hbutton, this);
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

	bool IsChecked()
	{
		return m_chkstate ? true: false;
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

		assert(wmDataChanged);
		HWND hdlg = GetParent(m_hbutton);
		::SendMessage(hdlg, wmDataChanged, m_uic, 0);
	}

protected: // from CxxWindowSubclass
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lret = DefSubclassProc(hwnd, uMsg, wParam, lParam);
		//vaDbgTs(_T("CCheckbox peek: uMsg=%s"), ITCSvn(uMsg, itc::BM_xxx));
		if(uMsg==BM_SETCHECK)
		{
			DataFromUic();
		}
		return lret;
	}
};


class CRadioGroup : public LiveUic
{
	int m_uicActive; // 0, 1, 2 ...

	int m_uicDefault;

	int m_uicStart, m_uicEnd;
	HWND m_hParent;

////
	
	// Define an inner class
	class RadioBtnPeeker : public CxxWindowSubclass
	{
	public:
		RadioBtnPeeker() {}
		void Init(CRadioGroup *pOuter, int uic, int idxInGroup)
		{
			m_pOuter = pOuter;
			m_uic = uic; m_idxInGroup = idxInGroup;
		}

	protected:
		virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			LRESULT lret = DefSubclassProc(hwnd, uMsg, wParam, lParam);

			int uic = GetDlgCtrlID(hwnd);
			//vaDbgTs(_T("RadioBtnPeeker: uMsg=%s , hUic=0x%08X , uic=%d"), ITCSvn(uMsg, itc::BM_xxx), hwnd, uic);

			if( uMsg==BM_CLICK || uMsg==BM_SETCHECK )
			{
				assert(m_pOuter);
				m_pOuter->DataFromUic();
			}

			return lret;
		}
	private:
		CRadioGroup *m_pOuter;
		int m_uic, m_idxInGroup;
	};

	cleanupArrayDelega<RadioBtnPeeker*>::type m_cecPeekers; //CEC_raw_delete m_cecPeekers;

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
		int nUic = uicEnd - uicStart + 1;
		if(uicDefault<=0)
			uicDefault = uicStart;

		assert(nUic>1);
		assert(uicStart<=uicDefault && uicDefault<=uicEnd);

		// Check parameter validity
		for(int i=0; i<nUic; i++)
		{
			HWND hbtn = GetDlgItem(hwndParent, uicStart+i);
			assert(IsWindow(hbtn));

			TCHAR szClassName[10];
			GetClassName(hbtn, szClassName, ARRAYSIZE(szClassName));
			assert(_tcscmp(szClassName, _T("Button"))==0);

			DWORD btnstyle = GetWindowStyle(hbtn) & BS_TYPEMASK;
			assert(btnstyle==BS_RADIOBUTTON || btnstyle==BS_AUTORADIOBUTTON);
		}

		m_uicStart = uicStart; m_uicEnd = uicEnd;
		m_uicActive = m_uicDefault = uicDefault;
		m_hParent = hwndParent;

		DataToUic();

		// Subclass each radio button in the group

		m_cecPeekers = new RadioBtnPeeker*[nUic];

		for(int i=0; i<nUic; i++)
		{
			HWND hbtn = GetDlgItem(hwndParent, uicStart+i);

			TCHAR sig[40];
			_sntprintf_s(sig, _TRUNCATE, _T("liveuic::CRadioGroup[%d]"), i);

			CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
			m_cecPeekers[i] = CxxWindowSubclass::FetchCxxobjFromHwnd<RadioBtnPeeker>(hbtn, 
				sig, TRUE, &err);
			assert(!err);
			m_cecPeekers[i]->Init(this, uicStart+i, i);

			DlgboxPeeker::AddUic(hbtn, this);
		}
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

		assert(wmDataChanged);
		::SendMessage(m_hParent, wmDataChanged, m_uicActive, 0);
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


const UINT wmDataChanged = RegisterWindowMessage(sigwmDataChanged);

bool DlgboxPeeker::AddUic(HWND hUic, LiveUic* plu) // static
{
	assert(IsWindow(hUic));
	if(!hUic)
		return false;

	HWND hdlg = GetParent(hUic);

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	DlgboxPeeker *peeker = GetDlgboxPeeker(hdlg, TRUE, &err);
	if(err && err!=E_Existed)
		return false;

// 	if(m_hdlg)
// 		assert(m_hdlg == hdlg);
// 	else
// 		m_hdlg = hdlg

	auto serr = peeker->m_saLiveUics.AppendTail(plu);
	assert(!serr);
	if(serr)
		return false;

	return true;
}

bool DlgboxPeeker::DelUic(HWND hUic, LiveUic* plu) // static
{
	assert(IsWindow(hUic));
	if(!hUic)
		return false;

	HWND hdlg = GetParent(hUic);

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	DlgboxPeeker *peeker = GetDlgboxPeeker(hdlg, FALSE, &err);
	assert(err);
	if(err)
		return false;

	auto serr = peeker->m_saLiveUics.DeleteMatch(plu);
	if(serr)
		return false; // maybe hUic not found
	else
		return true;
}



void DlgboxPeeker::ResetAllUicContent()
{
// 	if(!m_hdlg)
// 		return;

	for(int i=0; i<m_saLiveUics.CurrentEles(); i++)
	{
		m_saLiveUics[i]->Reset();
	}
}

////////////////////////////////////////////////////////////////////////////
} // namespace liveuic
////////////////////////////////////////////////////////////////////////////



#ifndef LiveUic_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__LiveUic_h_20260313_
