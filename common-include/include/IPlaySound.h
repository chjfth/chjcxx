#ifndef __CHHI__IPlaySound_h_
#define __CHHI__IPlaySound_h_
#define __CHHI__IPlaySound_h_created_ 20260630
#define __CHHI__IPlaySound_h_updated_ 20260705


#include <ps_TCHAR.h>

class IPlaySound
{
public:
	enum ReCode_et
	{
		E_Success = 0,
		E_Unknown = -1,
		E_BadParam = -2,
		E_SysApi = -3,

		E_FileNotFound = -5,
		E_ErrorFormat = -6,
		E_UnsupportFormat = -7,
		E_NotOpenYet = -8,
	};

public:
	static IPlaySound* CreateObj();

	static const TCHAR* GetVersion();

public:
	virtual ~IPlaySound() {}

	virtual ReCode_et OpenSoundBin(const void *pFileBin, int nBytes) = 0;
	// -- [pFileBin, nBytes] is whole .wav file content in RAM.
	//    User should keep pFileBin buffer until the playing is done/stop.
	// [2026-07-05] Currently, my Mswin port only support .wav content as pFileBin.

	virtual ReCode_et OpenSoundFile(const TCHAR* pszSoundFile) = 0;
	// -- pszSoundFile can be any media file type, typically an .mp3 file.
	//    Underlying implementation determines whether the input file is supported,
	//    if not, an ReCode_et is returned.

	virtual bool IsOpened() = 0;

	virtual ReCode_et Close() = 0;

	virtual ReCode_et PlayOnce() = 0;
	// -- The sound-playing should be asynchronous, i.e. thread not block while playing.

	enum PlayingQuery_et { PlayingQuery_NotSupport=0, Stopped=1, Playing=2 };

	virtual PlayingQuery_et IsPlaying() = 0;

	virtual ReCode_et Stop() = 0;
	// -- Stop() is asynchronous. If user calls Stop() then immediately query IsPlaying(),
	//    IsPlaying() may still return true. Actual implementation can design an 
	//    out-of-band(asynchronous) notification/message that tells the playing has stopped.
	//    On receiving that notification, IsPlaying() may still return Playing or Stopped.
	//    - If Playing, it means current notification is triggered by *previous* PlayOnce();
	//      a further notification must appear to tell play-done of *current* PlayOnce().
	//    - If Stopped, it really means *current* PlayOnce() has ended.

#ifdef __CHHI__InterpretConst_h_
	static const itc::Enum2Val_st _e2v_ReCode[];
	static const itc::CInterpretConst& ReCode_itc();

	static const itc::Enum2Val_st _e2v_PlayingQuery[];
	static const itc::CInterpretConst& PlayingQuery_itc();
#endif
};



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


#if defined(IPlaySound_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_IPlaySound) // [IMPL]


#ifdef __CHHI__InterpretConst_h_

const itc::Enum2Val_st IPlaySound::_e2v_ReCode[] =
{
	ITC_NAMEPAIR(E_Success),
	ITC_NAMEPAIR(E_Unknown),
	ITC_NAMEPAIR(E_BadParam),
	ITC_NAMEPAIR(E_SysApi),

	ITC_NAMEPAIR(E_FileNotFound),
	ITC_NAMEPAIR(E_ErrorFormat),
	ITC_NAMEPAIR(E_UnsupportFormat),
	ITC_NAMEPAIR(E_NotOpenYet),
};
ITC_MAKE_OBJECT(IPlaySound::ReCode_itc, IPlaySound::_e2v_ReCode, ITCF_SINT)

const itc::Enum2Val_st IPlaySound::_e2v_PlayingQuery[] =
{
	ITC_NAMEPAIR(PlayingQuery_NotSupport),
	ITC_NAMEPAIR(Stopped),
	ITC_NAMEPAIR(Playing),
};
ITC_MAKE_OBJECT(IPlaySound::PlayingQuery_itc, IPlaySound::_e2v_PlayingQuery, ITCF_SINT)

#endif // __CHHI__InterpretConst_h_


#endif // [IMPL]


#endif // include once guard
