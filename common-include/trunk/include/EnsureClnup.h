#ifndef __ENSURECLNUP_H
#define __ENSURECLNUP_H

/* The idea comes from Jeffrey Richter's CEnsureCleanup template class, 
 which is presented in his book 
   << Programming Server-Side Applications for Microsoft Windows 2000 >>

	But I found that his original CEnsureCleanup is not acceptable when
 compiling with GCC 3.2, and with the compiler from Borland C++ 5.5 .

	So I did the improvement.
*/

#define ENSURECLNUP_PRESENT
	// So that outter headers knows this header has been included.
	// Outter headers check this macro and offer to define MakeCleanupPtrClass(...) for user.


template<typename PTR_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(PTR_TYPE)> 
class CEnsureCleanupPtr
{
	PTR_TYPE m_t;           // The member representing the object

public:
	// Default constructor assumes an invalid value (nothing to cleanup)
	CEnsureCleanupPtr() { m_t = NULL; }
	
	// This constructor sets the value to the specified value
	CEnsureCleanupPtr(PTR_TYPE t) : m_t(t) { }
	
	// The destructor performs the cleanup.
	~CEnsureCleanupPtr() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	bool IsValid() { return(m_t != NULL); }
	bool IsInvalid() { return(!IsValid()); }
	operator bool(){ return IsValid(); }
	bool operator !(){ return !IsValid(); }
	
	// Re-assigning the object forces the current object to be cleaned-up.
	PTR_TYPE operator=(PTR_TYPE t) 
	{ 
		Cleanup(); 
		m_t = t;
		return(*this);  
	}
	
	// Returns the value (supports both 32-bit and 64-bit Windows).
	operator PTR_TYPE() 
	{ 
		return m_t;
	}
	
	PTR_TYPE Ptr()
	{
		return m_t;
	}
	
	PTR_TYPE operator->()
	{
		return m_t;
	}

	PTR_TYPE* operator&()
	{
		return &m_t;
	}

//	<*PTR_TYPE> operator[](int idx); 
		// No need! When operator [] is used on this object, compiler can call ``operator PTR_TYPE()''
		// automatically returning `m_t' so that `m_t[idx]' comes to the scene.

	// Cleanup the object if the value represents a valid object
	void Cleanup() 
	{ 
		if (IsValid()) 
		{
			pfn(m_t);         // Cleanup the object.
			m_t = NULL;   // We no longer represent a valid object.
		}
	}
};


template<class INT_TYPE, class RET_TYPE, RET_TYPE (*pfn)(INT_TYPE), INT_TYPE tInvalid> 
class CEnsureCleanupInt
{
	INT_TYPE m_t;           // The member representing the object

public:
	// Default constructor assumes an invalid value (nothing to cleanup)
	CEnsureCleanupInt() { m_t = tInvalid; }
	
	// This constructor sets the value to the specified value
	CEnsureCleanupInt(INT_TYPE t) : m_t(t) { }
	
	// The destructor performs the cleanup.
	~CEnsureCleanupInt() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	bool IsValid() { return(m_t != tInvalid); }
	bool IsInvalid() { return(!IsValid()); }
	operator bool(){ return IsValid(); }
	bool operator !(){ return !IsValid(); }
	
	// Re-assigning the object forces the current object to be cleaned-up.
	INT_TYPE operator=(INT_TYPE t) 
	{ 
		Cleanup(); 
		m_t = t;
		return(*this);  
	}
	
	// Returns the value (supports both 32-bit and 64-bit Windows).
	operator INT_TYPE() 
	{ 
		return m_t;
	}

	// Cleanup the object if the value represents a valid object
	void Cleanup() 
	{ 
		if (IsValid()) 
		{
			pfn(m_t);         // Cleanup the object.
			m_t = tInvalid;   // We no longer represent a valid object.
		}
	}
	
};



//////// Now, CEnsureCleanupPtrArray and CEnsureCleanupIntArray
// If you have an array, in which every element should receive a cleanup operation, then use these two.

template<typename USER_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(USER_TYPE)> 
class CEnsureCleanupPtrArray
{
	int m_ArraySize;
	USER_TYPE *m_ar;           // This member representing the object array ptr

public:
	// Default constructor clears all pointer elements to invalid-value
	CEnsureCleanupPtrArray(int ArraySize) : m_ArraySize(ArraySize) {
		m_ar = new USER_TYPE[m_ArraySize];
		for(int i=0; i<m_ArraySize; i++) m_ar[i] = NULL; 
	}
	
	// The destructor performs the cleanup.
	~CEnsureCleanupPtrArray() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	bool IsValid(int idx) { return(m_ar[idx] != NULL); }
	bool IsInvalid(int idx) { return(!IsValid(idx)); }

	USER_TYPE& operator[](int idx) {
		return m_ar[idx];
	}

	operator USER_TYPE*() {
		return m_ar; // Return the address of the first array element.
	}

	void Cleanup() { 
		for(int i=0; i<m_ArraySize; i++)
			if (IsValid(i)) {
				pfn(m_ar[i]);         // Cleanup the object.
			}
		delete m_ar;
	}
};

template<typename USER_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(USER_TYPE), USER_TYPE valInvalid> 
class CEnsureCleanupIntArray
{
	int m_ArraySize;
	USER_TYPE *m_ar;           // This member representing the object array ptr

public:
	// Default constructor clears all pointer elements to invalid-value
	CEnsureCleanupIntArray(int ArraySize) : m_ArraySize(ArraySize) {
		m_ar = new USER_TYPE[m_ArraySize];
		for(int i=0; i<m_ArraySize; i++) m_ar[i] = valInvalid; 
	}
	
	// The destructor performs the cleanup.
	~CEnsureCleanupIntArray() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	int IsValid(int idx) { return(m_ar[idx] != valInvalid); }
	int IsInvalid(int idx) { return(!IsValid(idx)); }

	USER_TYPE& operator[](int idx) {
		return m_ar[idx];
	}

	operator USER_TYPE*() {
		return m_ar; // Return the address of the first array element.
	}

	void Cleanup() { 
		for(int i=0; i<m_ArraySize; i++)
			if (IsValid(i)) {
				pfn(m_ar[i]);         // Cleanup the object.
			}
		delete m_ar;
	}
};



#define MakeCleanupPtrClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, PTR_TYPE) \
	typedef CEnsureCleanupPtr< PTR_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction > CecClassName;
	// Note: There should be a space between pCleanupfunction and the right angle bracket, because
	// pCleanupFunction will later be replaced by _EnsureClnup_cpp_delete<PTR_TYPE> which would 
	// otherwise result in a bogus >> operator .

#define MakeCleanupIntClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, INT_TYPE, IntValueInvalid) \
	typedef CEnsureCleanupInt<INT_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, IntValueInvalid> CecClassName;

#define MakeCleanupPtrClass_winapi(CecClassName, RetType, pCleanupFunction, PTR_TYPE) \
	inline RetType pCleanupFunction ## _Ptr__plain(PTR_TYPE ptr){ return pCleanupFunction(ptr); } \
	typedef CEnsureCleanupPtr< PTR_TYPE, RetType, pCleanupFunction ## _Ptr__plain > CecClassName;

#define MakeCleanupIntClass_winapi(CecClassName, RetType, pCleanupFunction, INT_TYPE, IntValueInvalid) \
	inline RetType pCleanupFunction ## _Int__plain(INT_TYPE h){ return pCleanupFunction(h); } \
	typedef CEnsureCleanupInt<INT_TYPE, RetType, pCleanupFunction ## _Int__plain, IntValueInvalid> CecClassName;
	// MakeCleanupIntClass_winapi provide a non-__stdcall wrapper so that CEnsureCleanupInt can be used smoothly.


//

#define MakeCleanupPtrArrayClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, USER_TYPE) \
	typedef CEnsureCleanupPtrArray< USER_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction > CecClassName;

#define MakeCleanupIntArrayClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, USER_TYPE, valInvalid) \
	typedef CEnsureCleanupIntArray<USER_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, valInvalid> CecClassName;

/////////////////////////////////// C++ ///////////////////////////////////////


template<typename PTR_TYPE> // 2017-10-20: deprecated?
inline void _EnsureClnup_cpp_delete(PTR_TYPE p){ delete p; }
//
#define MakeCleanupPtrClass_delete(CecClassName, PTR_TYPE) \
	MakeCleanupPtrClass(CecClassName, void, _EnsureClnup_cpp_delete<PTR_TYPE>, PTR_TYPE)


template<typename PTR_TYPE> // 2017-10-20: deprecated?
inline void _EnsureClnup_cpp_delete_array(PTR_TYPE p){ delete[] p; }
//
#define MakeCleanupPtrClass_delete_array(CecClassName, PTR_TYPE) \
	MakeCleanupPtrClass(CecClassName, void, _EnsureClnup_cpp_delete_array<PTR_TYPE>, PTR_TYPE)

// Now define a class named Cec_NewMemory representing a void*-pointed memory block,
// that would be de-allocated by C++-delete.
inline void _EnsureClnup_cpp_delete_pvoid(void *p){ delete (char*)p; }
MakeCleanupPtrClass(Cec_NewMemory, void, _EnsureClnup_cpp_delete_pvoid, void*)
	// 
	// Usage example:
	//
	//	Cec_NewMemory cec_memblock = new unsigned char[1000];
	//	memcpy(cec_memblock, src, 1000);



// [2017-10-20]
#define MakeCleanupCxxClass(CMyClass) \
	inline void _cxx_delete_1_ ## CMyClass(CMyClass *pobj) { delete pobj; } \
	inline void _cxx_delete_n_ ## CMyClass(CMyClass *arobj) { delete []arobj; } \
	MakeCleanupPtrClass(Cec_ ## CMyClass,      void, _cxx_delete_1_ ## CMyClass, CMyClass*) \
	MakeCleanupPtrClass(CecArray_ ## CMyClass, void, _cxx_delete_n_ ## CMyClass, CMyClass*) \
// So, a statement on global statement
//
//	MakeCleanupCxxClass(Cfoobar)
//
// enables these features in function scope:
//
// Writing:
//		Cec_Cfoobar       pfoobar = new Cfoobar;
//		CecArray_Cfoobar arfoobar = new Cfoobar[3];
// will automatically execute these destructions on leaving function scope:
//		delete    pfoobar;
//		delete []arfoobar;



//////////////////////////////////////////////////////////////////////////

/* Some usage examples:

MakeCleanupIntClass(Cec_LibcFh, int, close, int, -1)
	// Use ` int close(int); ' to cleanup a file handle opened by 
	// ` int open (const char *, int, ...); ' , and the invalid(default) value is -1 .

MakeCleanupPtrClass(Cec_pVoid, void, free, void*)
	// Use ` void free(void*); ' to cleanup a buffer allocated by 
	// ` void* malloc(size_t); ' .

// Windows API examples:
MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE);
	// This is for OpenProcess, who returns NULL as fail.
MakeCleanupIntClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE); 
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.



inline void free_unsigned_char_ptr(unsigned char * p){ free(p); }

MakeCleanupPtrClass(Cec_pUchar, void, free_unsigned_char_ptr, unsigned char *)
	// Use ` void free_unsigned_char_ptr(unsigned char * p); ' to cleanup a buffer 
	// allocated by ` void* malloc(size_t); ' . For the sake of function pointer
	// type matching, you have to define a wrapper function for free() .

class MyClass { ... };
MakeCleanupPtrClass_delete(Cec_MyClass, MyClass*)


int main()
{
	Cec_LibcFh fh = open("_TestFile.txt", O_CREAT|O_RDWR|O_TRUNC ,_S_IWRITE);

	Cec_pVoid pv = malloc(100);

	Cec_pUchar puc = (unsigned char*)malloc(200);

	return 0;
}
*/

#endif
