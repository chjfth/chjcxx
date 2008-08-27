#include <stdio.h>

#ifdef WIN32 // <io.h> only available on PC Windows, not WinCE
# include <io.h>
#else // Unix style below
# include <unixstd.h>
#endif

#include <gadgetlib/fstream-aux.h>


bool 
ggt_fstream_change_file_size(FILE *fp, int newsize)
{
	if(newsize<0)
		return false;

	int fd = fileno(fp);
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


