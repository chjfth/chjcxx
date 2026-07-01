#ifndef __CHHI__IPlaySound_h_
#define __CHHI__IPlaySound_h_
#define __CHHI__IPlaySound_h_created_ 20260630
#define __CHHI__IPlaySound_h_updated_ 20260630


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
	~IPlaySound() {}

	virtual ReCode_et OpenWavBin(const void *pWavFileBin, int nBytes) = 0;
	// -- [pWavFileBin, nBytes] is whole .wav file content in RAM.
	//    User should keep pWavBin buffer until the playing is done/stop.

	virtual ReCode_et OpenSoundFile(const TCHAR* pszSoundFile) = 0;

	virtual ReCode_et Close() = 0;

	virtual ReCode_et PlayOnce() = 0;

	virtual ReCode_et Stop() = 0;
};

#ifdef __CHHI__InterpretConst_h_
namespace itc {
extern const CInterpretConst& IPlaySound_ReCode();
}
#endif


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
namespace itc {

const Enum2Val_st _e2v_IPlaySound_ReCode[] =
{
	ITC_NAMEPAIR(IPlaySound::E_Success),
	ITC_NAMEPAIR(IPlaySound::E_Unknown),
	ITC_NAMEPAIR(IPlaySound::E_BadParam),
	ITC_NAMEPAIR(IPlaySound::E_SysApi),
	
	ITC_NAMEPAIR(IPlaySound::E_FileNotFound),
	ITC_NAMEPAIR(IPlaySound::E_ErrorFormat),
	ITC_NAMEPAIR(IPlaySound::E_UnsupportFormat),
	ITC_NAMEPAIR(IPlaySound::E_NotOpenYet),
};
ITC_MAKE_OBJECT(IPlaySound_ReCode, _e2v_IPlaySound_ReCode, ITCF_SINT)

} // namespace itc
#endif // __CHHI__InterpretConst_h_
#endif // [IMPL]


#endif // include once guard
