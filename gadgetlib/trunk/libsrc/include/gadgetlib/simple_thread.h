#ifndef __ggt_simple_thread_h_
#define __ggt_simple_thread_h_

#ifdef __cplusplus
extern"C" {
#endif


#ifndef DLLEXPORT_gadgetlib
#define DLLEXPORT_gadgetlib
#endif

typedef void *ggt_hsimplethread;
typedef void (*PROC_ggt_simple_thread)(void *param);
	// Note: On x86 Windows, this defaults to __cdecl, not __stdcall .

DLLEXPORT_gadgetlib
ggt_hsimplethread ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param, int stack_size);
	// return non-NULL handle on thread creation success.

DLLEXPORT_gadgetlib
bool ggt_simple_thread_waitend(ggt_hsimplethread h);


#ifdef __cplusplus
} // extern"C" {
#endif



// C++ code below:

// >>>> A C++ template to launch C++ object as thread-function
#ifdef __cplusplus

template<typename TClass>
inline void cxx_threadbegin_stub(void *_param)
{
	struct SParam
	{
		TClass *obj;
		void (TClass::*Func)();
	} *param = (SParam*)_param;;

	TClass *obj = param->obj;
	void (TClass::*Func)() = param->Func;
	(obj->*Func)();

	delete param;
}

template<typename TClass>
inline ggt_hsimplethread 
ggt_cxx_thread_create(TClass &cxxobj, void (TClass::*memberfunc)(), int stack_size=0)
{
	struct SParam
	{
		TClass *obj;
		void (TClass::*Func)();
	};

	SParam *param = new SParam; // delete in cxx_threadbegin_stub()
	param->obj = &cxxobj;
	param->Func = memberfunc;

	ggt_hsimplethread handle = 
		ggt_simple_thread_create(&cxx_threadbegin_stub<TClass>, param, stack_size);

//	Sleep(2000);
	return handle;
}


/* Usage:

struct A {
	int x;
	void thread_start() { printf("A::thread_start. x=%u\n", x); }
};

ggt_hsimplethread ht = ggt_cxx_thread_create(a1, &A::thread_func);

ggt_simple_thread_waitend(ht);

*/

#endif
// <<<< A C++ template to launch C++ object as thread-function


#endif
