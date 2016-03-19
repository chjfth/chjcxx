#include <stdio.h>

#ifdef _WIN32 
// Note: _WIN32 is defined for PC Windows and WinCE, 
// but <io.h> is only available on PC Windows, not WinCE.
// Looking for a reserved macro that distinguishes between the two.
# include <io.h>
#else // Unix style below
# include <unistd.h>
#endif

#include <gadgetlib/fstream-aux.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__fstream_aux__DLL_AUTO_EXPORT_STUB(void){}


bool 
ggt_fstream_change_file_size(FILE *fp, int newsize)
{
	if(newsize<0)
		return false;

#ifdef _MSC_VER
	int fd = _fileno(fp);
#else
	int fd = fileno(fp);
#endif
	if(fd<0)
		return false;

//	bool bRet = false;
	int ret = 0;

	fseek(fp, 0, SEEK_END);
	int oldsize = ftell(fp);
	if(oldsize<0)
		return false;

	ret = fseek(fp, newsize, SEEK_SET); // Adding this may be safer.
	if(ret!=0)
		return false;

#ifdef WIN32
	ret = _chsize(fd, newsize);
	if(ret!=0)
		return false;
#else 
	ret = ftruncate(fd, newsize);
	if(ret!=0)
	{	// Maybe ftruncate is not able to increase file size, try using fwrite to increase it.
		fseek(fp, oldsize, SEEK_SET);
		char wbuf[4096] = {0};
		for(;oldsize<newsize;)
		{
			int nWrOnce = newsize - oldsize;
			if(nWrOnce>sizeof(wbuf))
				nWrOnce = sizeof(wbuf);
			int nwr = fwrite(wbuf, 1, nWrOnce, fp);
			if(nwr!=nWrOnce)
				return false;

			oldsize += nwr;
		}
	}
#endif

	ret = fseek(fp, newsize, SEEK_SET); // Adding this may be safer.
	if(ret!=0)
		return false;

	return true;
}


