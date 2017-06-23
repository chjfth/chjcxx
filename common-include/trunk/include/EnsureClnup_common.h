#ifndef __EnsureClnup_common_h_
#define __EnsureClnup_common_h_

#include <stdio.h>
#include <stdlib.h>
#include <EnsureClnup.h>

inline void free_void_ptr(void * p){ free(p); }
MakeCleanupPtrClass(Cec_free_pVoid, void, free_void_ptr, void*)
	// Use ` void free(void*); ' to cleanup a buffer allocated by ``void* malloc(size_t);``.
	// Memo: Using a wrapper function free_void_ptr is necessary if you use this with WDK7,
	// because WDK7 use __stdcall as x86 default calling convention(__stdcall compiler env)
	// while CRT `free` is always __cdecl.

inline void free_char_ptr(char * p){ free(p); }
MakeCleanupPtrClass(Cec_free_pchar, void, free_char_ptr, char*)

inline void free_unsigned_char_ptr(unsigned char * p){ free(p); }
MakeCleanupPtrClass(Cec_free_pUchar, void, free_unsigned_char_ptr, unsigned char *)
	// Use ` void free_unsigned_char_ptr(unsigned char * p); ' to cleanup a buffer 
	// allocated by ` void* malloc(size_t); ' . For the sake of function pointer
	// type matching, you have to define a wrapper function for free() .


//////////////////////////////////////////////////////////////////////////

inline void _cpp_delete_pchar(char *pchar) { delete []pchar; }
MakeCleanupPtrClass(Cec_delete_pchar, void, _cpp_delete_pchar, char*)

inline void _cpp_delete_pUchar(unsigned char *pUchar) { delete []pUchar; }
MakeCleanupPtrClass(Cec_delete_pUchar, void, _cpp_delete_pUchar, unsigned char*)

inline void _cpp_delete_int_array(int *p){ delete []p; }
MakeCleanupPtrClass(Cec_delete_int, void, _cpp_delete_int_array, int*)

inline void _cpp_delete_uint_array(unsigned int *p){ delete []p; }
MakeCleanupPtrClass(Cec_delete_uint, void, _cpp_delete_uint_array, unsigned int*)



// For `` FILE* ''
inline int fclose_FILE_ptr(FILE *fp){ return fclose(fp); } // wrapper for __stdcall compiler env
MakeCleanupPtrClass(Cec_fclose, int, fclose_FILE_ptr, FILE*)


#endif
