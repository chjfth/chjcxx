#ifndef __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_
#define __CHHI__IPlaySound_mswin_h_created_ 20260630
#define __CHHI__IPlaySound_mswin_h_updated_ 20260704

#include <windows.h>
#include <IPlaySound.h>

////////////////////////////////////////////////////////////////////////////
// namespace chjns { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

// [2026-07-01] Implementation Note: OpenSoundBin() playing .wav file-content,
// currently need to lock all wave content. 
// Double buffering with small buffers has not been implemented yet.


UINT IPlaySound_RegisterHwndNotify(IPlaySound *vpsobj, HWND hwndNotify);
// -- Return a message value [that will be sent to hwndNotify on SoundBin-playing done].
//    Return 0(FALSE) if notification message unsupported.
//    If hwndNotify is NULL, notification will be turned off.
// When notification window-message arrives:
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
#include <stdarg.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
//
#include <CxxVerCheck.h>
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>
#include <sdring.h>
#include <chj_mishmash.h>
#include <CxxWindowSubclass.h>
//#include <mswin/WinError.itc.h> // This bloats EXE by 300KB+
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

	virtual bool IsOpened() cxx11_override;

	virtual ReCode_et Close() cxx11_override;

	virtual ReCode_et PlayOnce() cxx11_override;

	virtual PlayingQuery_et IsPlaying() cxx11_override;

	virtual ReCode_et Stop() cxx11_override;

public:
	CPlaySound() { _ctor(); }
	virtual ~CPlaySound() { _dtor(); }
	
	UINT RegisterHwndNotify(HWND hwndNotify);

protected:
	void _ctor();
	void _dtor();
	void reset_playstop_count() {
		m_nStarted = m_nFinished = 0;
	}

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

	MCIERROR vaExecMciString(const TCHAR *szfmt, ...);

	bool IsWavBinOpened() { return m_pwavformbin ? true : false; }

	bool IsMciOpened() { return m_mciDevId>0 ; }

private:
	static int s_notifymsg;
	static const TCHAR *s_msgkeystr;
	static const TCHAR *s_mciAliasPrefix; // and suffix is this-object address

	struct MciNotifyPeeker : public CxxWindowSubclass
	{
		CPlaySound *m_psParent;

		virtual LRESULT SubWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) cxx11_override
		{
			if( CPlaySound::s_notifymsg>0 
				&& m_psParent->m_hwndNotify
				&& uMsg==MM_MCINOTIFY )
			{
				LONG mciDevId = (LONG)lParam;
				UINT notiflags = (UINT)wParam;

				if(mciDevId == m_psParent->m_mciDevId)
				{
					if(notiflags==MCI_NOTIFY_SUCCESSFUL || notiflags==MCI_NOTIFY_ABORTED)
					{
						m_psParent->m_nFinished++;

						vaDBG2(_T("[nFinished#%u] IPlaySound: MciNotifyPeeker will notify user HWND(=0x%08X) play-done, msgval=0x%X"), 
							m_psParent->m_nFinished, m_psParent->m_hwndNotify, CPlaySound::s_notifymsg);

						::PostMessage(m_psParent->m_hwndNotify, CPlaySound::s_notifymsg, 
							0, (LPARAM)m_psParent);
					}
				}
			}

			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
	};

private:
	MciNotifyPeeker *m_peeker; // Peek into user's HWND to post play-done notification

	Uint m_nStarted, m_nFinished;

	//// PlayBin members:

	HWND m_hwndNotify;

	HWAVEOUT m_hWaveout;
	
	WAVEFORMATEX m_wavefmtex; // used by waveOutOpen()
	WAVEHDR m_wavehdr;        // used by waveOutPrepareHeader(), waveOutWrite()
	
	const Uchar *m_pwavformbin;
	DWORD m_wavformlen;

	//// PlayFile members:

//	Sdring m_playfile;
	Sdring m_devalias; // sound file 'alias' used by MCI command
	UINT m_mciDevId; // 1, 2, 3 etc
};

int CPlaySound::s_notifymsg = FALSE;
const TCHAR *CPlaySound::s_msgkeystr = _T("IPlaySound_NotifyMessage_20260630");
const TCHAR *CPlaySound::s_mciAliasPrefix = _T("IPlaySound_alias");

void CPlaySound::_ctor()
{
	m_peeker = nullptr;
	reset_playstop_count();

	m_hwndNotify = NULL;

	m_hWaveout = NULL;
	memset_0_struct(m_wavefmtex);
	memset_0_struct(m_wavehdr);
	m_pwavformbin = NULL;
	m_wavformlen = 0;

	m_mciDevId = 0;
}

void CPlaySound::_dtor()
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
	
	if(m_hwndNotify == hwndNotify)
	{
		// duplicate calling, easy return
		return hwndNotify ? s_notifymsg : FALSE; 
	}

	reset_playstop_count();

	if(m_hwndNotify)
	{
		// Remove old association first
		assert(m_peeker);
		
		if(IsWindow(m_hwndNotify))
			m_peeker->DetachHwnd(true);
		
		m_peeker = nullptr;
		m_hwndNotify = NULL;
	}

	if(hwndNotify)
	{
		if( !IsWindow(hwndNotify) )
			return FALSE;

		m_peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<MciNotifyPeeker>(
			hwndNotify, nullptr, 
			TRUE // TRUE: Each IPlaySound object will associate a peeker object.
			);
		if(!m_peeker)
		{
			vaDBG1(_T("CPlaySound::RegisterHwndNotify() fails to create peeker object for HWND=0x%08X"), (UINT)hwndNotify);
			return FALSE;
		}
		m_peeker->m_psParent = this;

		m_hwndNotify = hwndNotify;
	}

	return hwndNotify ? s_notifymsg : FALSE; 
}

IPlaySound::ReCode_et 
CPlaySound::_delayed_init()
{
	if(s_notifymsg == FALSE)
	{
		UINT msg = RegisterWindowMessage(s_msgkeystr);
		if(msg>0)
		{
			vaDBG2(_T("IPlaySound::RegisterNotifyMessage() got msgvalue: %u(=0x%X)"), msg, msg);
		}
		else
		{
			vaDBG1(_T("Panic! RegisterWindowMessage(\"%s\") returns FAIL. WinErr=%d"), s_msgkeystr, GetLastError());
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
CPlaySound::PlayOnce()
{
	ReCode_et pserr = E_Unknown;

	if( IsWavBinOpened() )
	{
		assert(!IsMciOpened());
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
			pserr = E_Success;
		}
		else
		{
			vaDBG3(_T("waveOutWrite() <<< mmerr=%s"), ITCS_MmsystemError(mmerr));
			pserr = E_SysApi;
		}
	}
	else if( IsMciOpened() )
	{
		assert(!IsWavBinOpened());
		assert(m_devalias.not_empty());

		MCIERROR mcierr = vaExecMciString(_T("play %s from 0 notify"), m_devalias.c_str());

		pserr = mcierr ? E_SysApi : E_Success;
	}
	else 
	{
		pserr = E_NotOpenYet;
	}

	if(!pserr)
	{
		m_nStarted++;
		vaDBG2(_T("[nStarted#%u] CPlaySound::PlayOnce() success"), m_nStarted);
	}

	return pserr;
}


CPlaySound::PlayingQuery_et 
CPlaySound::IsPlaying()
{
	if(!m_hwndNotify)
		return PlayingQuery_NotSupport;

	if( int(m_nStarted - m_nFinished) > 0 )
		return Playing;
	else
		return Stopped;
}


void CPlaySound::WaveOutProc(UINT wom_msg, DWORD_PTR param1, DWORD_PTR param2)
{
	vaDBG2(_T("[tid=%u] In WaveOutProc(), wom_msg=%s"), 
		GetCurrentThreadId(), ITCSnv(wom_msg, itc::MM_xxx_winmsg));

	if(wom_msg==MM_WOM_DONE && m_hwndNotify)
	{
		assert(s_notifymsg>0);

		m_nFinished++;

		vaDBG2(_T("[nFinished#%u] Due to MM_WOM_DONE, PostMessage to HWND=0x%X, msgval=0x%X"), 
			m_nFinished, (UINT)m_hwndNotify, (UINT)s_notifymsg);

		::PostMessage(m_hwndNotify, s_notifymsg, 0, (LPARAM)this);
	}
}


MCIERROR 
CPlaySound::vaExecMciString(const TCHAR *szfmt, ...)
{
	Sdring mcistr;

	va_list args;
	va_start(args, szfmt);
	vlSdringSet(mcistr, szfmt, args);
	va_end(args);

	vaDBG2(_T("Executing MCI string:\n> %s"), mcistr.c_str());

	MCIERROR mcierr = mciSendString(mcistr, NULL, 0, m_hwndNotify);

	if(!mcierr)
	{
		vaDBG2(_T("Executing MCI done, success."));
	}
	else
	{
		vaDBG2(_T("Executing MCI error, mcierr=%s."), ITCSvn(mcierr, itc::MmsystemError));
	}

	return mcierr;
}

IPlaySound::ReCode_et
CPlaySound::OpenSoundFile(const TCHAR* pszSoundFile)
{
	ReCode_et err = _delayed_init();
	if(err)
		return err;

	Close(); // Free old resources

	vaSdringSet(m_devalias, _T("%s-0x%llX"), s_mciAliasPrefix, (Uint64)(UINT_PTR)this);
	// -- Use %llX instead of %p, bcz %p has too many leading 0s for 64bit pointer.

	MCI_OPEN_PARMS mop = {};
	mop.lpstrElementName = pszSoundFile;
	mop.lpstrDeviceType  = NULL;
	mop.lpstrAlias       = m_devalias.c_str();
	mop.dwCallback       = 0;

	DWORD flags = 
		MCI_OPEN_ELEMENT // open-type according to mop.lpstrElementName
		| MCI_OPEN_ALIAS // an alias is included in the mop.lpstrAlias 
		;

	MCIERROR mcierr = mciSendCommand(
		0, // MCI_OPEN does not use this param
		MCI_OPEN,
		flags,
		(DWORD_PTR)&mop);

	if(mcierr)
	{
		vaDBG1(_T("mciSendCommand('%s') fails with mcierr=%s"), mop.lpstrElementName, ITCSvn(mcierr, itc::MmsystemError));
		
		if(mcierr==MCIERR_FILE_NOT_FOUND)
		{
			// pszSoundFile filename not found on disk
			return E_FileNotFound; 
		}
		else if(mcierr==MCIERR_EXTENSION_NOT_FOUND)
		{
			// pszSoundFile's extname is unrecognized(not .wav/.mp3 etc)
			return E_UnsupportFormat; 
		}
		else
		{
			// [2026-07-01] If I rename a foo.jpg to foo.jpg.mp3 and pass it to MCI_OPEN,
			// we'll get mcierr==MCIERR_INTERNAL .
			return E_SysApi;
		}
	}

	assert(mop.wDeviceID>0);
	m_mciDevId = mop.wDeviceID;

	return E_Success;
}


IPlaySound::ReCode_et
CPlaySound::Stop()
{
	if( IsWavBinOpened() )
	{
		assert(m_hWaveout);
		waveOutReset(m_hWaveout);

		return E_Success;
	}
	else if( IsMciOpened() )
	{
		assert(m_devalias.not_empty());

		MCIERROR mcierr = vaExecMciString(_T("stop %s"), m_devalias.c_str());

		return mcierr ? E_SysApi : E_Success;
	}
	else
	{
		return E_NotOpenYet;
	}
}


IPlaySound::ReCode_et
CPlaySound::Close()
{
	MMRESULT mmerr = 0;

	if(IsWavBinOpened())
	{
		vaDBG2(_T("Close() PlayBin"));

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

	if(IsMciOpened())
	{
		vaDBG2(_T("Close() PlayFile"));

		assert(m_devalias.not_empty());

		vaExecMciString(_T("close %s"), m_devalias.c_str());
	}

	memset_0_struct(m_wavefmtex);
	memset_0_struct(m_wavehdr);
	m_pwavformbin = NULL;

//	m_playfile.set_empty();
	m_devalias.set_empty();
	m_mciDevId = 0;

	return E_Success;
}


bool CPlaySound::IsOpened()
{
	if(IsWavBinOpened() || IsMciOpened())
		return true;
	else
		return false;
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
