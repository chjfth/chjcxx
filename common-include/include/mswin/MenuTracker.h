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
	
	// On_Init() is called inside CMenuTracker::AddPopAction().
	// Resource allocated should be free-ed in dtor.
	virtual void On_Init(HMENU hmenuPopup, const TCHAR *popname) {}

	// Mimic WindowsX: Cls_OnInitMenuPopup()
	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) {}

	// Mimic WindowsX: Cls_OnMenuSelect()
	virtual void On_WM_MENUSELECT(
		HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags) {}
};


class CMenuTracker
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Unknown = -1,
		E_ReInit = -2, 

		E_PopNameNotFound = -3,
		E_PopNameExisted = -4, 

		E_Winapi = -5,
		E_GetMenuItemInfo = -6,
	};

	static const TCHAR * const s_root_popname; // ="_root_"

public:
	CMenuTracker() {}
	virtual ~CMenuTracker();

	ReCode_et BindMenuTree(HMENU hmenuTop);

	ReCode_et AddPopAction(const TCHAR *popname, IMenuPop *p_menupop);
	// -- Inside CMenuTracker dtor, `delete p_menupop` will be executed automatically.
	//    May return E_PopNameNotFound or E_PopNameExisted.

	ReCode_et DelPopAction(const TCHAR *popname, IMenuPop **pp_oldobj);

	void Do_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, int idxItem, BOOL isSystemMenu);
		// idxItem is almost useless.
	
	void Do_WM_MENUSELECT(
		HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags);

protected:
	ReCode_et r_BindMenuTree(HMENU hmenu, const Sdring &popname, const Sdring &trimtext);
	
	Sdring GetPopnameFromMenutext(const TCHAR *menutext, Sdring &out_trimmed_menutext);

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

const TCHAR * const CMenuTracker::s_root_popname = _T("_root_");

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

	if(m_hmenuTop)
		return E_ReInit;

	m_hmenuTop = hmenuTop;

	ReCode_et err = r_BindMenuTree(hmenuTop, s_root_popname, _T(""));
	return err;
}

CMenuTracker::ReCode_et
CMenuTracker::r_BindMenuTree(HMENU hmenu, const Sdring &popname, const Sdring &trimtext)
{
	// r_ prefix implies recursive calling.
	// We do depth-first searching, so that msa_map[] holds menunob elements
	// the same sequence as that in .rc script.
	//
	// trimtext is the menutext with popname tail trimmed, only for debugging.

	BOOL b = 0;

	// First, if popname yes, append this hmenu to msa_map[].

	if(popname.not_empty())
	{
		SMap map = {};
		map.hMenupop = hmenu;
		map.popname = std::move(popname);
		msa_map.AppendTail(map);
	}

	// Second, iterate menuitems and recurse into all submenus.

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
		assert(mii.hSubMenu == IntToPtr(mii.wID)); // MSDN undoc, but true

		// Check whether nobtext ends with "//SOME_POPNAME"

		Sdring trimmed_menutext;
		Sdring sub_popname = GetPopnameFromMenutext(menutext, trimmed_menutext);

		if(sub_popname)
		{
			b = Menuitem_SetText(hmenu, i, MenuitemByPos, trimmed_menutext);
			if(!b)
				return E_Winapi;
		}
		else
			assert(trimmed_menutext.is_empty());

		ReCode_et err = r_BindMenuTree(mii.hSubMenu, sub_popname,
			trimmed_menutext.is_empty() ? menutext : trimmed_menutext);
		if(err)
			return err;
	}

	return E_Success;
}

Sdring CMenuTracker::GetPopnameFromMenutext(const TCHAR *menutext, Sdring &out_trimmed_menutext)
{
	// A menutext with popname "ShowDate" looks like this:
	//		"Show Date //ShowDate"
	// Human user intends to see only "Show Date" on menu item.
	// But to support MenuTracker facility, programmer should write "Show Date //ShowDate"
	// in .rc script.
	//
	// This function extract "ShowDate" as return value.

	out_trimmed_menutext.set_empty();

	const TCHAR *pss = StrRStrI(menutext, NULL, _T("//"));
	if(!pss)
		return nullptr;

	Sdring popname = pss+2;
	popname.trim_self(_T(" \t"));
	if(popname.is_empty())
		return nullptr;

	// Non-empty popname confirmed.

	// Remove "//SOME_POPNAME" from menutext, so they don't appear to human user.
	out_trimmed_menutext.setsn(menutext, int(pss-menutext));
	out_trimmed_menutext.trim_self(_T(" \t"));
		
	return popname;
}


CMenuTracker::ReCode_et 
CMenuTracker::AddPopAction(const TCHAR *popname, IMenuPop *p_menupop)
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
				p_menupop->On_Init(map.hMenupop, popname);
				return E_Success;
			}
		}
	}

	return E_PopNameNotFound;
}

CMenuTracker::ReCode_et 
CMenuTracker::DelPopAction(const TCHAR *popname, IMenuPop **pp_oldobj)
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
		if (hmenuPopup==map.hMenupop && map.pMenupop)
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
		if (hmenu==map.hMenupop && map.pMenupop)
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
