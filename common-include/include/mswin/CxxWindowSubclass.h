#ifndef CHHI__CxxWindowSubclass_h_20250606_20260317_
#define CHHI__CxxWindowSubclass_h_20250606_20260317_

// From Jimm Chen's chjcxx repo.
// Modification date at first line as version number.

// C++ encapsulation of SetWindowSubclass/RemoveWindowSubclass API.

#include <tchar.h>
#include <windows.h>
#include <CommCtrl.h> // DefSubclassProc

#include <CxxVerCheck.h>
#include <Cxx_delete_this_helper.h>


class CxxWindowSubclass : public Cxx_delete_this_base
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Fail = -1,

		E_NotExist = -2,
		E_Existed = -3,   // duplicate calling of FetchCxxobjFromHwnd()
		E_BadParam = -4,
		E_BadHwnd = -5,
		E_HwndPropConflict = -6, // User should pick a new sigstr string.
		
		E_CxxObjOccupied = -7,
		E_CxxObjConflict = -8, 

		E_WinapiSubclass = -10,
	};

	// User starts with FetchCxxobjFromHwnd(), passing is_create=true .
	//
	template<typename TChild>
	static TChild* FetchCxxobjFromHwnd(HWND hwnd, const TCHAR *sigstr, BOOL is_create,
		ReCode_et *pErr=nullptr);

	/* Example:

	class FooWndPeeker : public CxxWindowSubclass
	{
	protected:
		virtual LRESULT SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) cxx11_override
		{
			// ... do some pre-processing ...
			
			LRESULT lret = DefSubclassProc(hwnd, uMsg, wParam, lParam);
			
			// ... do some post-processing ...
			
			return lret;
		}
	}
	
	...

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	FooWndPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<FooWndPeeker>(
		hwnd_toSubclass, _T("sig_FooWndPeeker"), TRUE, &err);

	When to C++-delete `peeker` object?
	
	- When hwnd_toSubclass is destroyed by GUI system, `delete peeker` is done automatically,
	  then, you do NOT need to(and must not further do) `delete peeker`.

	- When you call `peeker->DetachHwnd(true)` explicitly, `delete peeker` is done internally.

	- `delete peeker` is a wrapper for `peeker->DetachHwnd(true)`.
	
	- Only when you do `peeker->DetachHwnd(false)`, you should/futher call `delete peeker` to
	  destroy the C++ object. This is not recommended, bcz `peeker->DetachHwnd(false)` is 
	  CxxWindowSubclass's internal action.
	*/

	static ReCode_et FetchCxxobjFromHwnd_as_partial(HWND hwnd, const TCHAR *sigstr,
		CxxWindowSubclass *inplace);
	// -- Similar to FetchCxxobjFromHwnd(), but user provides the already existing object space.
	//    (imply is_create=yes)
	//    This is useful if user creates TChild as part of multiple-inheritance class.
	//    Example (TChild is CEditValue): 
	//        class CEditValue : public LiveUic, public CxxWindowSubclass { ... }

public:
	CxxWindowSubclass();
	virtual ~CxxWindowSubclass();

	ReCode_et AttachHwnd(HWND hwnd, const TCHAR *sigstr);
	ReCode_et DetachHwnd(bool delete_cxxobj=true);

	bool IsAttached() {
		return m_hwnd ? true : false;
	}

private:
	static ReCode_et FetchCxxobj_precheck(HWND hwnd, const TCHAR *sigstr, BOOL is_create,
		PVOID *ppobj_output);

protected:
	virtual LRESULT SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// User's TChild overrides this SubWndProc() to hook into hwnd's message processing.

		return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}

private:
	///////////////////////////////////////////////////////////////////////////
	// This UINT signifies an CxxWindowSubclass object, MUST be the first member.
	UINT m_magic; 

	// This TCHAR string signifies a concrete subclass instance, MUST be the 2nd member.
	TCHAR *m_signature; // with s_winprop_prefix

	bool m_as_partial_cxxobj;
	///////////////////////////////////////////////////////////////////////////

private:
	static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	LRESULT StockWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static ReCode_et CheckHwnd(HWND hwnd, const TCHAR *sigstr); // sigstr no s_winprop_prefix

protected:
	HWND m_hwnd; // Derived class is allowed to see the raw HWND.

private:
	static const UINT const_magic = 0x20250606;
	static const TCHAR s_winprop_prefix[];
};


template<typename TChild> // TChild should be child class of CxxWindowSubclass
TChild* 
CxxWindowSubclass::FetchCxxobjFromHwnd(HWND hwnd, const TCHAR *sigstr, BOOL is_create,
	ReCode_et *pErr)
{
	SETTLE_OUTPUT_PTR(ReCode_et, pErr, E_Fail);

	TChild *pOldobj = nullptr;

	*pErr = FetchCxxobj_precheck(hwnd, sigstr, is_create, (PVOID*)&pOldobj);
	
	if(*pErr == E_Existed)
		return pOldobj;
	else if(*pErr)
		return nullptr;
	
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

#if defined(CxxWindowSubclass_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_CxxWindowSubclass) // [IMPL]

// >>> Include headers required by this lib's implementation
#include <commdefs.h>
#include <mswin/win32cozy.h>
// <<< Include headers required by this lib's implementation


#ifndef CxxWindowSubclass_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

// [2025-06-06] Note: We use SetWindowSubclass to store CxxWindowSubclass pointer as 
// user context. And we use SetProp/GetProp to associate the very CxxWindowSubclass pointer 
// with the HWND, so that user can fetch the CxxWindowSubclass pointer from HWND.


const TCHAR CxxWindowSubclass::s_winprop_prefix[] = _T("CxxWinsubcls-");

CxxWindowSubclass::CxxWindowSubclass()
{
	vaDBG2(_T("CxxWindowSubclass@%p ctor()."), this);

	m_magic = const_magic;
	m_signature = nullptr;
	m_as_partial_cxxobj = false;

	m_hwnd = NULL;
}

CxxWindowSubclass::~CxxWindowSubclass()
{
	bool isatt = IsAttached();
	
	if(isatt)
		vaDBG2(_T("CxxWindowSubclass@%p dtor(), will detach HWND=0x%X"), this, m_hwnd);
	else
		vaDBG2(_T("CxxWindowSubclass@%p dtor(), already detached from HWND."), this);

	if(isatt)
		DetachHwnd(true);

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

		vaDBG1(_T("SetWindowSubclass(hwnd=0x%X) fails with winerr=%d."), PtrToUint(m_hwnd), GetLastError());

		err_ret = E_WinapiSubclass;
		goto FAIL_END;
	}

	vaDBG2(_T("SetWindowSubclass(hwnd=0x%X, cxxobj=%p) success."), PtrToUint(m_hwnd), this);

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

	Cxx_delete_this_helper dth(this);

	vaDBG2(_T("CxxWindowSubclass@%p in DetachHwnd(0x%X)."), this, m_hwnd);

	// Detaching order is reverse of AttachHwnd()

	BOOL succ = RemoveWindowSubclass(m_hwnd, SubclassProc, (UINT_PTR)this);
	assert(succ);

	HANDLE hret = RemoveProp(m_hwnd, m_signature);
	assert(hret==this);

	delete m_signature;
	m_signature = nullptr;

	m_hwnd = NULL;

	if(delete_cxxobj && !m_as_partial_cxxobj)
		dth.MarkDelete();

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
	Cxx_delete_this_helper dth(this); // due to DetachHwnd() internally may `delete this`

	// Call child-class virtual function
	LRESULT lre = this->SubWndProc(hwnd, uMsg, wParam, lParam);

	if(uMsg==WM_NCDESTROY)
	{
		vaDBG2(_T("CxxWindowSubclass@%p sees WM_NCDESTROY, now detach."), this);

		this->DetachHwnd(true);

		vaDBG2(_T("hedit WM_NCDESTROY done"));
	}

	return lre;
}


CxxWindowSubclass::ReCode_et 
CxxWindowSubclass::FetchCxxobj_precheck(HWND hwnd, const TCHAR *sigstr, BOOL is_create,
	PVOID *ppobj)
{
	ReCode_et err = E_Fail;
	if(sigstr && sigstr[0])
	{
		// Check whether old subclass-instance exists.

		TCHAR signature[80] = {};
		_sntprintf_s(signature, _TRUNCATE, _T("%s%s"), s_winprop_prefix, sigstr);

		err = CheckHwnd(hwnd, signature);
		assert(err); // deliberate not get E_Success.

		if(err == E_Existed)
		{
			assert(ppobj);
			*ppobj = GetProp(hwnd, signature);
			return err;
		}

		if( ! (err==E_NotExist && is_create) )
		{
			return err; // something wrong
		}
	}
	else
	{
		// empty sigstr input

		if(!is_create)
		{
			return E_BadParam;
		}
	}

	return E_Success;
}


CxxWindowSubclass::ReCode_et
CxxWindowSubclass::FetchCxxobjFromHwnd_as_partial(HWND hwnd, const TCHAR *sigstr,
	CxxWindowSubclass *input_partial_cxxobj)
{
	void *pOldobj = nullptr;
	ReCode_et err = FetchCxxobj_precheck(hwnd, sigstr, TRUE, &pOldobj);
	if(err == E_Existed)
	{
		assert(pOldobj==input_partial_cxxobj);
		return E_Existed;
	}
	else if(err)
		return err;
	
	// Initialize the object in-place
	err = input_partial_cxxobj->AttachHwnd(hwnd, sigstr);
	input_partial_cxxobj->m_as_partial_cxxobj = true;
	return err;
}



#ifndef CxxWindowSubclass_DEBUG
#include <CHHI_vaDBG_show.h>
#endif

#endif // [IMPL]

#endif
