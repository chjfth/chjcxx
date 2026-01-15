#ifndef CHHI__mmlogfile_h_20260112_
#define CHHI__mmlogfile_h_20260112_

// This API uses mm_snprintf internally to write text strings to logfile.

class CBufferedLogfile;

class CMmLogfile
{
public:
	CMmLogfile(const TCHAR *logfile, bool isAppend=true, int bufmax=8192);
	~CMmLogfile();

	bool mmlog(const TCHAR *fmt, ...);
	// -- Each mmlog call is prepended with current timestamp.

private:
	const TCHAR *m_logfile;
	bool m_isAppend;
	int m_bufmax; // buffer size in bytes, 1 means totally no buffer.

	CBufferedLogfile *m_blg;
	int m_count;
	unsigned int m_prev_msec; // previous millisec
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


#if defined(mmlogfile_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_mmlogfile) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <mm_snprintf.h>
#include <vaDbgTs.h>
#include <BufferedLogfile.h>
#include <utf8wchar.h>

#ifndef _MSC_VER
#include <msvc_extras.h> // for ARRAYSIZE() macro
#endif

// <<< Include headers required by this lib's implementation



#ifndef mmlogfile_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
// namespace ApiNamespace { 
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


using namespace fsapi;

CMmLogfile::CMmLogfile(const TCHAR *logfile, bool isAppend, int bufmax)
{
	m_logfile = logfile;
	m_isAppend = isAppend;
	m_bufmax = bufmax;

	m_blg = nullptr;
	m_count = 0;
	m_prev_msec = 0;
}


CMmLogfile::~CMmLogfile()
{
	m_logfile = nullptr;
	delete m_blg;
}


struct CMmLogfile_ctx
{
	CBufferedLogfile *blg;
	fserror_et fserr;
};

static void mmct_CMmLogfile_write(void *ctx_user, const TCHAR *pcontent, int nchars,
	const struct mmctexi_st *pcti)
{
    // Caution: pcontent is not NUL-terminated.
	(void)pcti;
	if(nchars==0)
		return; // This can happen for %k %t %v etc

	CMmLogfile_ctx &ctx = *(CMmLogfile_ctx*)ctx_user;

#ifdef UNICODE
	// TCHAR is wchar_t. Convert it to utf8 string then write to file.
	int utf8_bytes = 0;
	char *utf8s = utf8::ptr_from_wchars(pcontent, nchars, NULL, &utf8_bytes);

	bool succ = ctx.blg->Append(utf8s, utf8_bytes);

	utf8::ptr_freebuf(utf8s);
#else

	bool succ = ctx.blg->Append(pcontent, nchars);

#endif

	ctx.fserr = succ ? E_success : E_unknown;
}

bool CMmLogfile::mmlog(const TCHAR *fmt, ...)
{
	bool succ = false;

	if(!m_blg)
	{
		m_blg = new CBufferedLogfile();
		if(!m_blg)
			return false;

		succ = m_blg->Start(m_logfile, m_isAppend, m_bufmax);
		if(!succ)
			return false;
	}

	m_count++;

	TCHAR timestr[60];
	va_now_timestr(VaNowtime_ymd|VaNowtime_millisec, 
		timestr, ARRAYSIZE(timestr));

	Uint now_msec = va_millisec();
	if(now_msec==0)
		now_msec = 1; // bcz 0 is reserved for "begin" value

	Uint delta_msec = 0;
	if(m_prev_msec!=0)
		delta_msec = now_msec - m_prev_msec;

	CMmLogfile_ctx ctx = {};
	ctx.blg = m_blg;

	va_list args;
	va_start(args, fmt);

	mm_printf_ct(mmct_CMmLogfile_write, &ctx, 
		_T("[%d][%s](+%3u.%03us) %w\n")
		, 
		m_count,
		timestr,
		delta_msec / 1000, delta_msec % 1000,
		MM_WPAIR_PARAM(fmt, args)
		);

	va_end(args);

	m_prev_msec = now_msec;

	if(!ctx.fserr)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////
// } // namespace ApiNamespace
////////////////////////////////////////////////////////////////////////////



#ifndef mmlogfile_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // CHHI__mmlogfile_h_20260112_
