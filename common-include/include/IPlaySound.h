#ifndef __IPlaySound_h_
#define __IPlaySound_h_
#define __IPlaySound_h_created_ 20260630
#define __IPlaySound_h_updated_ 20260630

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

	static int RegisterNotifyMessage();
	// -- Return a message value [that will be sent to user program on playing done].
	// -1 if notification message unsupported.

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



#endif


