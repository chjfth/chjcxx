#ifndef __CxxWindowSubclass_h_20250618_
#define __CxxWindowSubclass_h_20250618_

// C++ encapsulation of SetWindowSubclass/RemoveWindowSubclass API.

#include <tchar.h>
#include <windows.h>

//template<typename TUserCxx>

class CxxWindowSubclass
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Fail = -1,

		E_NotExist = -2,
		E_Existed = -3,
		E_BadParam = -4,
		E_BadHwnd = -5,
		E_HwndPropConflict = -6, // User should pick a new signature string.
		
		E_CxxObjOccupied = -7,
		E_CxxObjConflict = -8, 

		E_WinapiSubclass = -10,
	};

	// User starts here, passing is_create=true .
	template<typename TChild>
	static TChild* FetchCxxobjFromHwnd(HWND hwnd, const TCHAR *signature, BOOL is_create,
		ReCode_et *pErr=nullptr);

	/* Example:
	
	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg_toSubclass, _T("sig_EasyTooltipMan"), TRUE, &err);

	When to delete `ptm` object?
	If you call ptm->DetachHwnd(true) explicitly, or hdlg_toSubclass destroys, 
	`delete ptm` is done automatically.
	*/

public:
	CxxWindowSubclass();
	virtual ~CxxWindowSubclass();

	ReCode_et AttachHwnd(HWND hwnd, const TCHAR *sigstr);
	ReCode_et DetachHwnd(bool delete_cxxobj=true);

	bool IsAttached() {
		return m_hwnd ? true : false;
	}

protected:
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// User overrides this WndProc() to hook into hwnd's message processing.

		return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}

private:
	// This identifies an CxxWindowSubclass object, must be the first member.
	UINT m_magic; 

	// This identifies a concrete subclass instance, must be the 2nd member.
	TCHAR *m_signature; // with s_winprop_prefix

private:
	static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	LRESULT StockWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static ReCode_et CheckHwnd(HWND hwnd, const TCHAR *sigstr); // sigstr no s_winprop_prefix

protected:

	HWND m_hwnd;

private:
	static const UINT const_magic = 0x20250606;
	static const TCHAR s_winprop_prefix[];
};


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef CxxWindowSubclass_IMPL

#ifndef CxxWindowSubclass_DEBUG
#define vaDBG(...)
#endif

// [2025-06-06] Note: We use SetWindowSubclass to store CxxWindowSubclass pointer as 
// user context. And we use SetProp/GetProp to associate the very CxxWindowSubclass pointer 
// with the HWND, so that user can fetch the CxxWindowSubclass pointer from HWND.

#include <commdefs.h>
#include <mswin/win32cozy.h>

const TCHAR CxxWindowSubclass::s_winprop_prefix[] = _T("CxxWinsubcls-");

CxxWindowSubclass::CxxWindowSubclass()
{
	vaDBG(_T("CxxWindowSubclass@%p ctor()."), this);

	m_magic = const_magic;
	m_signature = nullptr;

	m_hwnd = NULL;
}

CxxWindowSubclass::~CxxWindowSubclass()
{
	bool isatt = IsAttached();
	
	if(isatt)
		vaDBG(_T("CxxWindowSubclass@%p dtor(), will detach HWND=0x%X"), this, m_hwnd);
	else
		vaDBG(_T("CxxWindowSubclass@%p dtor(), already detached from HWND."), this);

	if(isatt)
		DetachHwnd();

	m_magic = 0;
}

CxxWindowSubclass::ReCode_et 
CxxWindowSubclass::CheckHwnd(HWND hwnd, const TCHAR *signature)
{
	assert(signature);

	if (!IsWindow(hwnd))
		return E_BadHwnd;

	CxxWindowSubclass *pOld = (CxxWindowSubclass*)GetProp(hwnd, signature);
	if (pOld)
	{
		if (pOld->m_magic != const_magic)
			return E_HwndPropConflict;

		if ((UINT_PTR)pOld->m_signature < 0x10000)
			return E_HwndPropConflict;

		if(_tcscmp(pOld->m_signature, signature) != 0)
			return E_HwndPropConflict; // Maybe I'm running with a copycat code.

		return E_Existed;
	}
	else
		return E_NotExist;
}

CxxWindowSubclass::ReCode_et 
CxxWindowSubclass::AttachHwnd(HWND hwnd, const TCHAR *sigstr)
{
	// The sigstr is used to identify a subclass instance.
	// This sigstr(appending an extra prefix) will be passed to SetProp(),
	// so that FetchCxxobjFromHwnd() can later find out this Subclass-Cxxobj via HWND
	// by querying GetProp().
	//
	// If user do not need to query Cxxobj from HWND, he can pass sigstr=nullptr.
	// In this case, a unique string is generated as sigstr.

	if(m_hwnd)
	{
		// We do not allow one CxxObj to attach TWO hwnds at the same time.
		return E_CxxObjOccupied;
	}

	TCHAR szAddr[40] = {};

	// Prepare window-prop signature.

	if(!sigstr || !sigstr[0])
	{
		// Make unique sigstr for user.
		_sntprintf_s(szAddr, _TRUNCATE, _T("cxxobj-%p"), this);
		sigstr = szAddr;
	}

	assert(ARRAYSIZE(s_winprop_prefix)>8);

	int slen = (int)_tcslen(sigstr);
	int allbuflen = ARRAYSIZE(s_winprop_prefix) + slen; // NUL-term included
	m_signature = new TCHAR[allbuflen];
	_sntprintf_s(m_signature, allbuflen, _TRUNCATE, _T("%s%s"),	s_winprop_prefix, sigstr);

	ReCode_et err = CheckHwnd(hwnd, m_signature);
	if (err != E_NotExist)
	{
		assert(err!=E_Success);

		if(err == E_Existed) {
			err = E_CxxObjConflict;
		}

		return err; 
	}

	m_hwnd = hwnd;

	BOOL succ = SetProp(m_hwnd, m_signature, (HANDLE)this);
	if(!succ)
	{
		return E_Fail;
	}

	ReCode_et err_ret = E_Fail;

	// Subclass the hwnd.

	succ = SetWindowSubclass(m_hwnd, SubclassProc, (UINT_PTR)this, 0);

	if (!succ)
	{
		// If user pass in a HWND from another process, it fails with WinErr=2(ERROR_FILE_NOT_FOUND).

		vaDBG(_T("SetWindowSubclass(hwnd=0x%X) fails with winerr=%d."), PtrToUint(m_hwnd), GetLastError());

		err_ret = E_WinapiSubclass;
		goto FAIL_END;
	}

	vaDBG(_T("SetWindowSubclass(hwnd=0x%X, cxxobj=%p) success."), PtrToUint(m_hwnd), this);

	return E_Success;

FAIL_END:

	RemoveProp(hwnd, m_signature);

	delete m_signature;
	m_signature = nullptr;

	m_hwnd = NULL;

	return err_ret;
}

CxxWindowSubclass::ReCode_et 
CxxWindowSubclass::DetachHwnd(bool delete_cxxobj)
{
	if(!m_hwnd)
		return E_BadParam;

	vaDBG(_T("CxxWindowSubclass@%p in DetachHwnd(0x%X)."), this, m_hwnd);

	// Detaching order is reverse of AttachHwnd()

	BOOL succ = RemoveWindowSubclass(m_hwnd, SubclassProc, (UINT_PTR)this);
	assert(succ);

	HANDLE hret = RemoveProp(m_hwnd, m_signature);
	assert(hret==this);

	delete m_signature;
	m_signature = nullptr;

	m_hwnd = NULL;

	if(delete_cxxobj)
		delete this;

	return E_Success;
}


LRESULT CALLBACK 
CxxWindowSubclass::SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	(void)dwRefData;
	CxxWindowSubclass *pobj = (CxxWindowSubclass*)uIdSubclass;

	HANDLE verify = GetProp(hwnd, pobj->m_signature);

	assert(verify==pobj);

	LRESULT lre = pobj->StockWndProc(hwnd, uMsg, wParam, lParam);
	return lre;
}


LRESULT 
CxxWindowSubclass::StockWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Call child-class virtual function
	LRESULT lre = this->WndProc(hwnd, uMsg, wParam, lParam);

	if(uMsg==WM_NCDESTROY)
	{
		vaDBG(_T("CxxWindowSubclass@%p sees WM_NCDESTROY, now detach."), this);

		DetachHwnd(true);
	}

	return lre;
}


template<typename TChild> // TChild should be child class of CxxWindowSubclass
TChild* 
CxxWindowSubclass::FetchCxxobjFromHwnd(HWND hwnd, const TCHAR *sigstr, BOOL is_create,
	ReCode_et *pErr)
{
	SETTLE_OUTPUT_PTR(ReCode_et, pErr, E_Fail);

	if(sigstr && sigstr[0])
	{
		// Check whether old subclass-instance exists.

		TCHAR signature[80] = {};
		_sntprintf_s(signature, _TRUNCATE, _T("%s%s"), s_winprop_prefix, sigstr);

		*pErr = CheckHwnd(hwnd, signature);

		if(*pErr == E_Existed)
		{
			return (TChild*)GetProp(hwnd, signature);
		}

		if( ! (*pErr==E_NotExist && is_create) )
			return nullptr; // something wrong
	}
	else
	{
		// empty sigstr input

		if(!is_create)
		{
			*pErr = E_BadParam;
			return nullptr;
		}
	}
	
	// Create a new subclass-instance.

	CxxWindowSubclass *pobj = new TChild;
	*pErr = pobj->AttachHwnd(hwnd, sigstr);

	if (*pErr == E_Success)
	{
		return reinterpret_cast<TChild*>(pobj);
	}
	else 
	{
		delete pobj;
		return nullptr;
	}

	return nullptr;
}




#ifndef CxxWindowSubclass_DEBUG
#undef vaDBG
#endif

#endif // CxxWindowSubclass_IMPL

#endif
