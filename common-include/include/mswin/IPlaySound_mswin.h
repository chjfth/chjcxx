#ifndef __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_created_ 20260630
#define __CHHI__IPlaySound_mswin_h_updated_ 20260630

#include <IPlaySound.h>

////////////////////////////////////////////////////////////////////////////
// namespace chjns { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.


UINT IPlaySound_RegisterHwndNotify(IPlaySound *vpsobj, HWND hwndNotify);
// -- Return a message value [that will be sent to hwndNotify on sound-playing done].
//    Return 0(FALSE) if notification message unsupported.
// [WPARAM] always 0, meaning MM_WOM_DONE.
// [LPARAM] the IPlaySound object pointer that triggered this message.




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
#include <assert.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
//
#include <CxxVerCheck.h>
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>
#include <sdring.h>
#include <mswin/WinError.itc.h>
#include <mswin/mmsystem.itc.h>
// <<< Include headers required by this lib's implementation




#ifndef IPlaySound_mswin_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


////////////////////////////////////////////////////////////////////////////
// namespace chjns {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.

const Uint MaxWavFormLen = 128*1024*1024;

class CPlaySound : public IPlaySound
{
public:
	virtual ReCode_et OpenWavBin(const void *pWavFileBin, int nBytes) cxx11_override;

	virtual ReCode_et OpenSoundFile(const TCHAR* pszSoundFile) cxx11_override;

	virtual ReCode_et Close() cxx11_override;

	virtual ReCode_et PlayOnce() cxx11_override;

	virtual ReCode_et Stop() cxx11_override;

public:
	CPlaySound();
	virtual ~CPlaySound();
	
	//friend IPlaySound;

	UINT RegisterHwndNotify(HWND hwndNotify);

protected:
	enum State_et { Dumb=0, PlayWavBin=1, PlayFile=2 };

	ReCode_et _delayed_init();

	static ReCode_et CheckWaveFormat(const WAVEFORMATEX& wf);

	void WaveOutProc(UINT wom_msg, DWORD_PTR param1, DWORD_PTR param2);

	static void CALLBACK s_WaveOutProc(HWAVEOUT hwo, UINT wom_msg, 
		DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		CPlaySound *pobj = (CPlaySound*)dwInstance;
		
		if(wom_msg==MM_WOM_OPEN)
			assert(NULL==pobj->m_hWaveout); // that stupid API
		else
			assert(hwo == pobj->m_hWaveout);
		
		pobj->WaveOutProc(wom_msg, dwParam1, dwParam2);
	}

private:
	static int s_notifymsg;

private:
	HWND m_hwndNotify;

	Sdring m_playfile;
	
	HWAVEOUT m_hWaveout;
	
	WAVEFORMATEX m_wavefmtex; // used by waveOutOpen()
	WAVEHDR m_wavehdr;         // used by waveOutPrepareHeader(), waveOutWrite()
	
	const Uchar *m_pwavformbin;
	DWORD m_wavformlen;
};

int CPlaySound::s_notifymsg = FALSE;


CPlaySound::CPlaySound()
{
	m_hwndNotify = NULL;

	m_hWaveout = NULL;
	memset_0_struct(m_wavefmtex);
	memset_0_struct(m_wavehdr);
	m_pwavformbin = NULL;
	m_wavformlen = 0;
}

CPlaySound::~CPlaySound()
{
	Close();
}


UINT IPlaySound_RegisterHwndNotify(IPlaySound *vpsobj, HWND hwndNotify)
{
	CPlaySound *psobj = dynamic_cast<CPlaySound*>(vpsobj);
	if(!psobj)
	{
		vaDBG1(_T("[ERROR] IPlaySound_RegisterHwndNotify(): User pass in an invalid ptr of IPlaySound*, NOT of class CPlaySound."));
		return FALSE;
	}

	return psobj->RegisterHwndNotify(hwndNotify);
}

UINT CPlaySound::RegisterHwndNotify(HWND hwndNotify)
{
	ReCode_et err = _delayed_init();
	if(err)
		return FALSE;
	
	m_hwndNotify = hwndNotify;

	return s_notifymsg;
}

IPlaySound::ReCode_et 
CPlaySound::_delayed_init()
{
	if(s_notifymsg == FALSE)
	{
		const TCHAR *msgkeystr = _T("IPlaySound_NotifyMessage_20260630");
		UINT msg = RegisterWindowMessage(msgkeystr);
		if(msg>0)
		{
			vaDBG2(_T("IPlaySound::RegisterNotifyMessage() got msgvalue: %u(=0x%X)"), msg, msg);
		}
		else
		{
			vaDBG1(_T("Panic! RegisterWindowMessage(\"%s\") returns FAIL. WinErr=%s"), msgkeystr, ITCS_WinError);
			msg = FALSE;
		}

		s_notifymsg = msg;
	}

	if(s_notifymsg == FALSE)
	{
		return E_SysApi;
	}

	return E_Success;
}

IPlaySound::ReCode_et 
CPlaySound::CheckWaveFormat(const WAVEFORMATEX& wf)
{
#define ERROR_CheckWaveFormat _T("ERROR: CheckWaveFormat() ")
	if(! (wf.wFormatTag==WAVE_FORMAT_PCM) )
	{
		vaDBG1(ERROR_CheckWaveFormat _T("Expect .wFormatTag==1, Actual: %d (unsupported)"), wf.wFormatTag);
		return E_UnsupportFormat;
	}
	if(! (wf.nChannels==1 || wf.nChannels==2) )
	{
		vaDBG1(ERROR_CheckWaveFormat _T("Expect .nChannels==1 or .nChannels==2, Actual: %d (unsupported)"), wf.nChannels);
		return E_UnsupportFormat;
	}
	
	// wf.nSamplesPerSec ... not check this.

	if(! (wf.wBitsPerSample%8==0 && wf.wBitsPerSample<=24) )
	{
		vaDBG1(ERROR_CheckWaveFormat _T("wf.wBitsPerSample(=%d) not supported."), wf.wBitsPerSample);
		return E_UnsupportFormat;
	}

	if(! (wf.nAvgBytesPerSec == wf.nChannels*wf.nSamplesPerSec*wf.wBitsPerSample/8) )
	{
		vaDBG1(ERROR_CheckWaveFormat _T("wf.nAvgBytesPerSec(=%d) does NOT match wf.nChannels*wf.nSamplesPerSec*wf.wBitsPerSample/8"), wf.nAvgBytesPerSec);
		return E_ErrorFormat;
	}

	if(! (wf.nBlockAlign == wf.nChannels*wf.wBitsPerSample/8) )
	{
		vaDBG1(ERROR_CheckWaveFormat _T("wf.nBlockAlign(=%d) does NOT match wf.nChannels*wf.wBitsPerSample/8"), wf.nBlockAlign);
		return E_ErrorFormat;
	}

	return E_Success;
}

IPlaySound::ReCode_et
CPlaySound::OpenWavBin(const void *pWavFileBin, int nBytes)
{
	ReCode_et err = _delayed_init();
	if(err)
		return err;

	Close(); // Free old resources

	// WAV file format reference: PRWIN5 Chap22.2, p940.

#define ERROR_PlayWavBin _T("ERROR: PlayWavBin() ")

	//
	// Load .wav file data
	//

	const int minbytes = 100;
	if(nBytes<minbytes)
	{
		vaDBG1(ERROR_PlayWavBin _T("input nBytes(=%d) less than %d."), nBytes, minbytes);
		return E_BadParam;
	}

	struct WavFileHeader_st
	{
		Uchar Riff[4];
		Uint WAVEsize; // nBytes-4
		Uchar Wave[4];
	};
	assert(sizeof(WavFileHeader_st)==12);
	const WavFileHeader_st& wfh = *(WavFileHeader_st*)pWavFileBin;

	if(memcmp(wfh.Riff, "RIFF", 4)!=0)
	{
		vaDBG1(ERROR_PlayWavBin _T("input missing 'RIFF' at offset 0. You give '%c%c%c%c'"),
			wfh.Riff[0], wfh.Riff[1], wfh.Riff[2], wfh.Riff[3]);
		return E_ErrorFormat;
	}

	if(memcmp(wfh.Wave, "WAVE", 4)!=0)
	{
		vaDBG1(ERROR_PlayWavBin _T("input missing 'WAVE' at offset 8. You give '%c%c%c%c'"),
			wfh.Wave[0], wfh.Wave[1], wfh.Wave[2], wfh.Wave[3]);
		return E_ErrorFormat;
	}

	if(wfh.WAVEsize != nBytes-8)
	{
		vaDBG1(ERROR_PlayWavBin _T("input UINT at input offset 4 does NOT match nBytes-8. Input .WAVEsize=%d , nBytes=%d"), 
			wfh.WAVEsize, nBytes);
		return E_ErrorFormat;
	}

	// Search for "fmt " and "data" chunk that contains waveform data 

	bool seen_fmt = false;
	bool seen_data = false;

	const Uchar *wavfilebin = (Uchar*)pWavFileBin;
	const Uchar *ptag = wavfilebin + sizeof(WavFileHeader_st);

	for(;;)
	{
		Uint chunklen = *(Uint*)(ptag+4);
		const Uchar *ptag_next = ptag + 8 + chunklen;

		vaDBG2(_T("PlayWavBin() at offset 0x%X, meet chunk tag '%c%c%c%c', chunk size 0x%X(=%u)"),  
			(Uint)(ptag-wavfilebin), // offset 0x%X
			ptag[0], ptag[1], ptag[2], ptag[3],
			chunklen, chunklen // chunk size 0x%X(=%u)
			);

		if( (Uint)(ptag_next-wavfilebin) > (Uint)nBytes )
		{
			vaDBG1(ERROR_PlayWavBin _T("Semantic data length(%u) exceeds input bytes(=%d)"), (Uint)(ptag_next-wavfilebin), nBytes);
			return E_ErrorFormat;
		}

		if( memcmp(ptag, "fmt ", 4)==0 )
		{
			if( chunklen != 16 )
			{
				vaDBG1(ERROR_PlayWavBin _T("meet 'fmt ' chunk, but size(0x%X) is wrong, should be 16."), chunklen);
				return E_UnsupportFormat;
			}

			m_wavefmtex = *(WAVEFORMATEX*)(ptag+8);

			err = CheckWaveFormat(m_wavefmtex);
			if(err)
			{
				memset_0_struct(m_wavefmtex);
				return err;
			}

			seen_fmt = true;
		}
		else if( memcmp(ptag, "data", 4)==0 )
		{
			if( chunklen > MaxWavFormLen)
			{
				vaDBG1(ERROR_PlayWavBin _T("'data' chunk exceeds maximum lib limit: %u"), MaxWavFormLen);
				return E_UnsupportFormat;
			}

			m_pwavformbin = (const Uchar*)ptag + 8;
			m_wavformlen = chunklen;

			seen_data = true;
		}
		else
		{
			vaDBG2(_T("-- This tag unrecognized, skip."));
		}

		ptag = ptag_next;

		if(seen_fmt && seen_data)
			break;
	}

	//
	// Open waveOut device for this WaveBin instance.
	//

	vaDBG3(_T("waveOutOpen() >>> [tid=%d]"), GetCurrentThreadId());
	//
	MMRESULT mmerr = waveOutOpen(&m_hWaveout, WAVE_MAPPER, &m_wavefmtex,
		(DWORD_PTR)s_WaveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	//
	if(!mmerr)
	{
		vaDBG3(_T("waveOutOpen() <<< OK ret: hWaveOut=0x%p"), m_hWaveout);
	}
	else
	{
		vaDBG3(_T("waveOutOpen() <<< mmerr=%s"), ITCS_MmsystemError(mmerr));
		assert(!m_hWaveout);
	}

	memset_0_struct(m_wavehdr);
	m_wavehdr.lpData = (LPSTR)m_pwavformbin;
	m_wavehdr.dwBufferLength = m_wavformlen;

	mmerr = waveOutPrepareHeader(m_hWaveout, &m_wavehdr, sizeof(m_wavehdr));
	if(mmerr)
	{
		vaDBG1(_T("Panic! waveOutPrepareHeader() fails with merr=%s"), ITCS_MmsystemError(mmerr));
		Close();
		return E_SysApi;
	}

	return E_Success;
}


IPlaySound::ReCode_et
CPlaySound::Close()
{
	MMRESULT mmerr = 0;
	if(m_hWaveout)
	{
		mmerr = waveOutReset(m_hWaveout);
		if(mmerr)
		{
			vaDBG1(_T("Unexpect! waveOutReset() returns error: %s"), ITCSvn(mmerr, itc::MmsystemError));
		}

		if(m_wavehdr.lpData)
		{
			mmerr = waveOutUnprepareHeader(m_hWaveout, &m_wavehdr, sizeof(m_wavehdr));
			assert(!mmerr);
		}


		mmerr = waveOutClose(m_hWaveout);
		if(mmerr)
		{
			vaDBG1(_T("Unexpect! waveOutClose() returns error: %s"), ITCSvn(mmerr, itc::MmsystemError));
		}

		m_hWaveout = NULL;
	}

	memset_0_struct(m_wavefmtex);
	memset_0_struct(m_wavehdr);
	m_pwavformbin = NULL;

	return E_Success;
}


IPlaySound::ReCode_et 
CPlaySound::PlayOnce()
{
	if(!m_pwavformbin)
		return E_NotOpenYet;

	assert(m_wavefmtex.wFormatTag==WAVE_FORMAT_PCM);
	assert(m_pwavformbin);
	assert(m_hWaveout);

	vaDBG3(_T("waveOutWrite() >>> lenbytes: %d [tid=%u]"), m_wavehdr.dwBufferLength, GetCurrentThreadId());
	//
	MMRESULT mmerr = waveOutWrite(m_hWaveout, &m_wavehdr, sizeof(m_wavehdr));
	//
	if(!mmerr)
	{
		vaDBG3(_T("waveOutWrite() <<<"));
		return E_Success;
	}
	else
	{
		vaDBG3(_T("waveOutWrite() <<< mmerr=%s"), ITCS_MmsystemError(mmerr));
		return E_SysApi;
	}
}


void CPlaySound::WaveOutProc(UINT wom_msg, DWORD_PTR param1, DWORD_PTR param2)
{
	vaDBG2(_T("[tid=%u] In WaveOutProc(), wom_msg=%s"), 
		GetCurrentThreadId(), ITCSnv(wom_msg, itc::MM_xxx_winmsg));

	if(wom_msg==MM_WOM_DONE && m_hwndNotify)
	{
		assert(s_notifymsg>0);

		vaDBG2(_T("Due to MM_WOM_DONE, PostMessage to HWND=0x%X, msgval=0x%X"), (UINT)m_hwndNotify, (UINT)s_notifymsg);
		::PostMessage(m_hwndNotify, s_notifymsg, 0, (LPARAM)this);
	}
}


IPlaySound::ReCode_et
CPlaySound::OpenSoundFile(const TCHAR* pszSoundFile)
{
	ReCode_et err = _delayed_init();
	if(err)
		return err;

	// TODO

	return E_Unknown;
}


IPlaySound::ReCode_et
CPlaySound::Stop()
{
	if(m_hWaveout)
	{
		waveOutReset(m_hWaveout);
		return E_Success;
	}
	else
	{
		return E_NotOpenYet;
	}
}



IPlaySound* IPlaySound::CreateObj()
{
	return new CPlaySound;
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
