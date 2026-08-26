#ifndef __CHHI__MenuTracker_h_
#define __CHHI__MenuTracker_h_
#define __CHHI__MenuTracker_h_created_ 20260826
#define __CHHI__MenuTracker_h_updated_ 20260826

#include <utility>
#include <windows.h>

#include <CxxVerCheck.h>
#include <TScalableArray.h>
#include <sdring.h>


////////////////////////////////////////////////////////////////////////////
//namespace chjns { 
////////////////////////////////////////////////////////////////////////////

class IMenuPop
{
public:
	virtual ~IMenuPop() {}
	
	// Mimic WindowsX: Cls_OnInitMenuPopup()
	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) = 0;

	// Mimic WindowsX: Cls_OnMenuSelect()
	virtual void On_WM_MENUSELECT(
		HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags) = 0;
};


class CMenuTracker
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Unknown = -1,
		
		E_PopNameNotFound = -2,
		E_PopNameExisted = -3, 

		E_Winapi = -4,
		E_GetMenuItemInfo = -5,
	};

public:
	CMenuTracker() {}
	virtual ~CMenuTracker();

	ReCode_et BindMenuTree(HMENU hmenuTop);

	ReCode_et AddListen(const TCHAR *popname, IMenuPop *p_menupop);
	// -- Inside CMenuTracker dtor, `delete popobj` will be executed automatically.
	//    May return E_PopNameNotFound or E_PopNameExisted.

	ReCode_et DelListen(const TCHAR *popname, IMenuPop **pp_oldobj);

	void Do_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, int idxItem, BOOL isSystemMenu);
		// idxItem is almost useless.
	
	void Do_WM_MENUSELECT(
		HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags);

protected:
	ReCode_et r_BindMenuTree(HMENU hmenu);

protected:
	struct SMap // map hMenupop to pMenupop
	{
		HMENU hMenupop;
		IMenuPop *pMenupop;
		Sdring popname;
	};

private:
	HMENU m_hmenuTop;
	TScalableArray<SMap> msa_map;
};


////////////////////////////////////////////////////////////////////////////
//} // namespace chjns
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


#if defined(MenuTracker_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_MenuTracker) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef MenuTracker_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
//namespace chjns {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


int MenuTracker_getversion()
{
	return 1;
}

CMenuTracker::~CMenuTracker()
{
	int i, count = msa_map.CurrentEles();
	for(i=0; i<count; i++)
	{
		delete msa_map[i].pMenupop;
	}
}

CMenuTracker::ReCode_et 
CMenuTracker::BindMenuTree(HMENU hmenuTop)
{
	// Recursively scan the menutree(root from hmenuTop), for each menupop(=submenu)
	// with nobtext ending with //SOME_POPNAME, register that menupop into msa_map[].
	// So that, user later can associate IMenuPop to some registered menupop.

	ReCode_et err = r_BindMenuTree(hmenuTop);
	return err;
}

CMenuTracker::ReCode_et
CMenuTracker::r_BindMenuTree(HMENU hmenu)
{
	// r_ prefix implies recursive calling.
	// We do wide-first searching.

	BOOL b = 0;
	int start_idx = msa_map.CurrentEles();
	int submenu_count = 0;

	TCHAR menutext[256] = {};

	int items = GetMenuItemCount(hmenu);
	for(int i=0; i<items; i++)
	{
		MENUITEMINFO mii = {sizeof(mii)};
		mii.fMask = MIIM_SUBMENU | MIIM_FTYPE | MIIM_STRING | MIIM_ID;
		mii.dwTypeData = menutext;
		mii.cch = ARRAYSIZE(menutext)-1;

		b = GetMenuItemInfo(hmenu, i, MenuitemByPos, &mii);
		if(!b)
			return E_GetMenuItemInfo;

		if(mii.hSubMenu==NULL)
			continue;
		
		// Now we meet a menunob. 
		// Check whether nobtext ends with //SOME_POPNAME

		const TCHAR *pss = StrRStrI(menutext, NULL, _T("//"));
		if(!pss)
			continue;

		Sdring popname = pss+2;
		popname.trim(_T(" \t"));
		if(popname.is_empty())
			continue;

		assert(mii.hSubMenu==IntToPtr(mii.wID)); // MSDN undoc, but true
		
		SMap map = {};
		map.hMenupop = mii.hSubMenu;
		map.popname = std::move(popname);

		msa_map.AppendTail(map);
		submenu_count++;

		// Remove visual popname from nobtext
		Sdring trimmed_popname(menutext, int(pss-menutext));
		trimmed_popname.trim_self(_T(" \t"));
		
		b = Menuitem_SetText(hmenu, i, MenuitemByPos, trimmed_popname);
		if(!b)
			return E_Winapi;
	}

	// Recurse into each submenu

	for(int i=0; i<submenu_count; i++)
	{
		ReCode_et err = r_BindMenuTree(msa_map[start_idx+i].hMenupop);
		if(err)
			return err;
	}

	return E_Success;
}


CMenuTracker::ReCode_et 
CMenuTracker::AddListen(const TCHAR *popname, IMenuPop *p_menupop)
{
	int count = msa_map.CurrentEles();
	for(int i=0; i<count; i++)
	{
		SMap &map = msa_map[i];
		if (_tcscmp(map.popname, popname) == 0)
		{
			if(map.pMenupop)
				return E_PopNameExisted;
			else
			{
				map.pMenupop = p_menupop;
				return E_Success;
			}
		}
	}

	return E_PopNameNotFound;
}

CMenuTracker::ReCode_et 
CMenuTracker::DelListen(const TCHAR *popname, IMenuPop **pp_oldobj)
{
	int count = msa_map.CurrentEles();
	for (int i = 0; i < count; i++)
	{
		SMap &map = msa_map[i];
		if (_tcscmp(map.popname, popname) == 0)
		{
			if (map.pMenupop)
				*pp_oldobj = map.pMenupop;

			map.pMenupop = nullptr;
			return E_Success;
		}
	}

	return E_PopNameNotFound;
}


void CMenuTracker::Do_WM_INITMENUPOPUP(
	HWND hwnd, HMENU hmenuPopup, int idxItem, BOOL isSystemMenu)
{
	int count = msa_map.CurrentEles();
	for (int i = 0; i < count; i++)
	{
		SMap &map = msa_map[i];
		if (hmenuPopup == map.hMenupop)
		{
			map.pMenupop->On_WM_INITMENUPOPUP(hwnd, hmenuPopup, idxItem, isSystemMenu);
			return;
		}
	}
}

void CMenuTracker::Do_WM_MENUSELECT(
	HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags)
{
	int count = msa_map.CurrentEles();
	for (int i = 0; i < count; i++)
	{
		SMap &map = msa_map[i];
		if (hmenu == map.hMenupop)
		{
			map.pMenupop->On_WM_MENUSELECT(hwnd, hmenu, idxItem, hSubmenu, flags);
			return;
		}
	}
}


////////////////////////////////////////////////////////////////////////////
//} // namespace chjns
////////////////////////////////////////////////////////////////////////////



#ifndef MenuTracker_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
