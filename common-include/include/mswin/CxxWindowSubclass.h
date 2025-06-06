#ifndef __CxxWindowSubclass_h_20250606_
#define __CxxWindowSubclass_h_20250606_

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

		E_Existed = -3,
		E_BadParam = -4,
		E_BadHwnd = -5,
		E_HwndPropConflict = -6,
	};

public:
	CxxWindowSubclass();
	~CxxWindowSubclass();

	ReCode_et AttachHwnd(HWND hwnd, const TCHAR *signature, bool isAutoCxxDelete=true);
	ReCode_et DetachHwnd();

	bool IsAttached() {
		return m_hwnd ? true : false;
	}

private:
	// This identifies an CxxWindowSubclass object, must be the first member.
	UINT m_magic; 

	// This identifies a concrete subclass instance, must be the 2nd member.
	TCHAR *m_signature;

private:
	static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	LRESULT StockWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual LRESULT WndProc(HWND hwnd,UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

protected:

	bool m_isAutoDelete; // auto delete this C++ object in WM_DESTROY

	HWND m_hwnd;
	ATOM m_atom;

private:
	static const UINT const_magic = 0x20250606;
};


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef CxxWindowSubclass_IMPL

#ifndef CxxWindowSubclass_DEBUG
#define vaDBG(...)
#endif

CxxWindowSubclass::CxxWindowSubclass()
{
	vaDBG(_T("CxxWindowSubclass@%p ctor()."), this);

	m_magic = const_magic;
	m_signature = nullptr;

	m_isAutoDelete = false;

	m_hwnd = NULL;
	m_atom = 0;
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
CxxWindowSubclass::AttachHwnd(HWND hwnd, const TCHAR *signature, bool isAutoCxxDelete)
{
	if(!IsWindow(hwnd))
		return E_BadHwnd;

	if(!signature || !signature[0])
		return E_BadParam;

	CxxWindowSubclass *pOld = (CxxWindowSubclass*)GetProp(hwnd, signature);
	if(pOld)
	{
		if(pOld->m_magic != const_magic)
			return E_HwndPropConflict;

		if( (UINT_PTR)pOld->m_signature < 0x10000 )
			return E_HwndPropConflict;

		if(_tcscmp(pOld->m_signature, signature) !=0 )
			return E_HwndPropConflict;

		return E_Existed;
	}

	m_hwnd = hwnd;
	m_isAutoDelete = isAutoCxxDelete;

	int slen = (int)_tcslen(signature);
	m_signature = new TCHAR[slen+1];
	_tcscpy_s(m_signature, slen+1, signature);

	BOOL succ = SetProp(m_hwnd, m_signature, (HANDLE)this);
	if(!succ)
	{
		return E_Fail;
	}

	m_atom = AddAtom(m_signature);
	if(!m_atom)
		goto FAIL_END;

	// Subclass the hwnd.

	succ = SetWindowSubclass(m_hwnd, SubclassProc, m_atom, (DWORD_PTR)this);
	if(!succ)
		goto FAIL_END;

	return E_Success;

FAIL_END:

	RemoveProp(hwnd, signature);

	delete m_signature;
	m_signature = nullptr;

	DeleteAtom(m_atom);

	m_hwnd = NULL;

	return E_Fail;
}

CxxWindowSubclass::ReCode_et 
CxxWindowSubclass::DetachHwnd()
{
	if(!m_hwnd)
		return E_BadParam;

	vaDBG(_T("CxxWindowSubclass@%p in DetachHwnd(0x%X)."), this, m_hwnd);

	// Detaching order is reverse of AttachHwnd()

	BOOL succ = RemoveWindowSubclass(m_hwnd, SubclassProc, m_atom);
	assert(succ);

	ATOM aret = DeleteAtom(m_atom);
	assert(aret==0);
	m_atom = 0;

	HANDLE hret = RemoveProp(m_hwnd, m_signature);
	assert(hret==this);

	delete m_signature;
	m_signature = nullptr;

	m_hwnd = NULL;

	return E_Success;
}


LRESULT CALLBACK 
CxxWindowSubclass::SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CxxWindowSubclass *pobj = (CxxWindowSubclass*)dwRefData;

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

		DetachHwnd();

		if (m_isAutoDelete)
		{
			delete this;
		}
	}

	return lre;
}


#ifndef CxxWindowSubclass_DEBUG
#undef vaDBG
#endif

#endif // CxxWindowSubclass_IMPL

#endif
