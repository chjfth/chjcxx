#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>


#include <gadgetlib/simple_thread.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__simple_thread__DLL_AUTO_EXPORT_STUB(void){}


struct SwThreadParam
{
	PROC_ggt_simple_thread proc;
	void *param;
	pthread_t th; // th: thread handle
	
	SwThreadParam(){ memset(this, 0, sizeof(this)); }
};


static void *
_LinuxThreadWrapper(void * param)
{
	SwThreadParam *pw = (SwThreadParam*)param;
	pw->proc(pw->param);
	return 0;
}


ggt_hsimplethread 
ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param)
{
	SwThreadParam *pwp = new SwThreadParam;
	pwp->proc = proc;
	pwp->param = param;
	
	int err = pthread_create(&pwp->th, NULL, _LinuxThreadWrapper, pwp);
	if(!err)
		return pwp;
	else
		return NULL;
}

bool 
ggt_simple_thread_waitend(ggt_hsimplethread h)
{
	if(!h) {
		// make it dead
		while(1) usleep(1000*100);
	}
	
	SwThreadParam *pwp = (SwThreadParam*)h;
	
	int err = pthread_join(pwp->th, NULL);
	if(!err)
	{
		delete pwp;
		return true;
	}
	else 
		return false;
}
