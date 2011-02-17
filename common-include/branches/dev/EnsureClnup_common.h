#ifndef __EnsureClnup_common_h_
#define __EnsureClnup_common_h_

#include <stdio.h>
#include <stdlib.h>
#include <EnsureClnup.h>

MakeCleanupPtrClass(Cec_free_pVoid, void, free, void*)
	// Use ` void free(void*); ' to cleanup a buffer allocated by 
	// ` void* malloc(size_t); ' .

inline void free_char_ptr(char * p){ free(p); }

MakeCleanupPtrClass(Cec_free_pchar, void, free_char_ptr, char*)

inline void free_unsigned_char_ptr(unsigned char * p){ free(p); }

MakeCleanupPtrClass(Cec_free_pUchar, void, free_unsigned_char_ptr, unsigned char *)
	// Use ` void free_unsigned_char_ptr(unsigned char * p); ' to cleanup a buffer 
	// allocated by ` void* malloc(size_t); ' . For the sake of function pointer
	// type matching, you have to define a wrapper function for free() .


//////////////////////////////////////////////////////////////////////////

inline void _cpp_delete_pchar(char *pchar)
{
	delete []pchar;
}

MakeCleanupPtrClass(Cec_delete_pchar, void, _cpp_delete_pchar, char*)

inline void _cpp_delete_pUchar(unsigned char *pUchar)
{
	delete []pUchar;
}

MakeCleanupPtrClass(Cec_delete_pUchar, void, _cpp_delete_pUchar, unsigned char*)


// For `` FILE* ''

MakeCleanupPtrClass(Cec_fclose, int, fclose, FILE*)


#endif
