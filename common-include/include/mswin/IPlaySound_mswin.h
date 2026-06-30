#ifndef __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_created_ 20260630
#define __CHHI__IPlaySound_mswin_h_updated_ 20260630

#include <IPlaySound.h>

////////////////////////////////////////////////////////////////////////////
// namespace chjns { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


// int getversion();


////////////////////////////////////////////////////////////////////////////
// } // namespace chjns
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


#if defined(IPlaySound_mswin_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_IPlaySound_mswin) // [IMPL]


// >>> Include headers required by this lib's implementation
#include <windows.h>
#include <mmsystem.h>
//
#include <CxxVerCheck.h>
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>
#include <sdring.h>
// <<< Include headers required by this lib's implementation




#ifndef IPlaySound_mswin_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
// namespace chjns {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


class CPlaySound : public IPlaySound
{
public:
	virtual ReCode_et PlayFile(const TCHAR* pszSoundFile) cxx11_override;

	virtual ReCode_et PlayWaveBin(const void *pWaveBin, int nBytes) cxx11_override;

	virtual ReCode_et Stop() cxx11_override;

	friend IPlaySound;

private:
	static int s_notifymsg;

private:
	Sdring m_playfile;
	HWAVEOUT m_hWaveout;

};

int CPlaySound::s_notifymsg = -1;


int IPlaySound::RegisterNotifyMessage()
{
	if(CPlaySound::s_notifymsg == -1)
	{
		UINT msg = RegisterWindowMessage(_T("IPlaySound_NotifyMessage_20260630"));
		if(msg==0)
			msg = -1;

		CPlaySound::s_notifymsg = msg;
	}
	
	return CPlaySound::s_notifymsg;
}

IPlaySound::ReCode_et
IPlaySound::PlayFile(const TCHAR* pszSoundFile)
{

	return E_Unknown;
}

IPlaySound::ReCode_et
IPlaySound::PlayWaveBin(const void *pWaveBin, int nBytes)
{

	return E_Success;
}

IPlaySound::ReCode_et
ReCode_et IPlaySound::Stop()
{
	return E_Success;
}


const TCHAR* IPlaySound::GetVersion()
{
	static TCHAR s_verstr[] = _T("1.0.0");
	return s_verstr;
}


////////////////////////////////////////////////////////////////////////////
// } // namespace chjns
////////////////////////////////////////////////////////////////////////////



#ifndef IPlaySound_mswin_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
