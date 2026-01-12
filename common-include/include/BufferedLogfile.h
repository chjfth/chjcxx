#ifndef CHHI__BufferedLogfile_h_20260112_
#define CHHI__BufferedLogfile_h_20260112_

// no namespace

#include <fsapi.h>

class CBufferedLogfile
{
public:
	CBufferedLogfile();
	~CBufferedLogfile();

	bool Start(const TCHAR *filepath, bool isAppend, int bufmax);
	bool End();

	bool Append(const void *mem, int wrbytes);
	bool IsStarted()
	{ 
		return m_hfile==fsapi::bad_filehandle ? false : true; 
	}

	bool FlushBuffer();

private:
	void Cleanup();

private:
	fsapi::filehandle_t m_hfile;
//	HANDLE m_hmutex;

	char *m_wrbuf;
	int m_bufmax;
	int m_ndirty; // pending/dirty bytes in m_wrbuf[]

	__int64 m_total_wrcount;
	__int64 m_total_wrbytes;

	fsapi::fserror_et m_fserr; // set on disk full
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


#if defined(BufferedLogfile_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_BufferedLogfile) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <_MINMAX_.h>

// <<< Include headers required by this lib's implementation




#ifndef BufferedLogfile_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
// namespace ApiNamespace { 
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.

using namespace fsapi;

CBufferedLogfile::CBufferedLogfile()
{
	memset(this, 0, sizeof(*this));

	m_hfile = fsapi::bad_filehandle;
}

CBufferedLogfile::~CBufferedLogfile()
{
	End();
}

bool 
CBufferedLogfile::Start(const TCHAR *filepath, bool isAppend, int bufmax)
{
	if(bufmax<=0)
		return false;

	m_ndirty = 0;
	m_bufmax = bufmax;
	m_wrbuf = new char[bufmax];
	if(!m_wrbuf)
		return false;

// 	m_hmutex = CreateMutex(NULL, FALSE, NULL);
// 	if(!m_hmutex)
// 		goto ERROR_END;

	m_hfile = file_open(filepath, open_for_append, open_share_read);
	if(m_hfile<0)
		goto ERROR_END;

	if(isAppend)
	{
		file_seek(m_hfile, 0, seek_end);
	}
	else
	{	
		fserror_et err = file_setsize(m_hfile, 0); // clear file
		if(err)
			goto ERROR_END;
	}

	return true;

ERROR_END:
	Cleanup();
	return false;
}

void 
CBufferedLogfile::Cleanup()
{
	if(m_wrbuf)
	{
		delete m_wrbuf;
		m_wrbuf = NULL;
	}

	if(m_hfile>=0)
	{
		file_close(m_hfile);
		m_hfile = bad_filehandle;
	}

// 	if(m_hmutex)
// 	{
// 		CloseHandle(m_hmutex);
// 		m_hmutex = NULL;
// 	}
}

bool 
CBufferedLogfile::End()
{
	if(!IsStarted())
		return false;

	bool succ = FlushBuffer();

	Cleanup();

	return succ ? true : false;
}

bool 
CBufferedLogfile::FlushBuffer()
{
	if(!IsStarted())
		return false;
	if(m_fserr)
		return false;

//	Win32AutoMutex am(m_hmutex);

	if(m_ndirty==0)
		return true;

	int ndone = file_write(m_hfile, m_wrbuf, m_ndirty);
	if(ndone>0)
	{
		assert(ndone==m_ndirty);
		m_ndirty = 0;
		m_total_wrbytes += ndone;
		m_total_wrcount++;
		return true;
	}
	else
	{
		m_fserr = (fserror_et)ndone;
		return false;
	}
}

bool 
CBufferedLogfile::Append(const void *_mem, int newbytes)
{
	if(!IsStarted())
		return false;
	if(m_fserr)
		return false;

	if(newbytes==0)
		return true;
	else if(newbytes<0)
		return false;

//	Win32AutoMutex am(m_hmutex);

	assert(m_ndirty < m_bufmax); // a dirty-full buffer should have been flushed earlier

	const char *mem = (const char*)_mem;

	int totpending = m_ndirty + newbytes;

	if( (totpending >= m_bufmax*2) ||
		(m_ndirty==0 && totpending>=m_bufmax) )
	{
		// call file_write twice, no memcpy 

		if(m_ndirty)
		{
			bool succ = FlushBuffer();
			if(!succ)
			{
				assert(m_fserr); // set by FlushBuffer();
				return false;
			}
		}

		DWORD ndone = file_write(m_hfile, mem, newbytes);
		if(ndone>0)
		{
			assert(ndone==newbytes);
			m_total_wrbytes += newbytes;
			m_total_wrcount++;
		}
		else
		{
			m_fserr = (fserror_et)ndone;
			return false;
		}

	}
	else if(totpending >= m_bufmax)
	{
		// memcpy once, write_file once then (optional)memcpy once again

		int nmove = _MIN_(newbytes, m_bufmax-m_ndirty);

		memcpy(m_wrbuf+m_ndirty, mem, nmove);
		m_ndirty += nmove;
		mem += nmove;
		newbytes -= nmove;

		bool succ = FlushBuffer();
		if(!succ)
			return false;

		if(newbytes>0)
		{
			assert(newbytes<m_bufmax);
			memcpy(m_wrbuf, mem, newbytes);
			m_ndirty += newbytes;
		}
	}
	else 
	{	// memcpy once, no WriteFile
		memcpy(m_wrbuf+m_ndirty, mem, newbytes);
		m_ndirty += newbytes;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////
// } // namespace ApiNamespace
////////////////////////////////////////////////////////////////////////////



#ifndef BufferedLogfile_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__BufferedLogfile_h_20260112_
