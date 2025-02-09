#ifndef __EnsureClnup_h_20250209_
#define __EnsureClnup_h_20250209_

/* Jimm Chen from around 2010:
 The idea comes from Jeffrey Richter's CEnsureCleanup template class, 
 which is presented in his book 
   << Programming Server-Side Applications for Microsoft Windows 2000 >>

	But I found that his original CEnsureCleanup is not acceptable when
 compiling with GCC 3.2, and with the compiler from Borland C++ 5.5 .

	So I did the improvement, many many improvements.
*/

#define ENSURECLNUP_PRESENT
	// So that outer headers knows this header has been included.
	// Outer headers check this macro and offer to define MakeCleanupPtrClass(...) for user.


template<typename PTR_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(PTR_TYPE)> 
class CEnsureCleanupPtr
{
	// The data member representing the object:
	// Design requirement: The class can contain ONLY one single data member,
	// that holds user's resource pointer/handle, so that, the memory layout
	// of a CEnsureCleanupPtr object is exactly the SAME as user's raw data.

	PTR_TYPE m_t;           

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

	// operator bool(){ return IsValid(); }
	// Don't define this, bcz it will cause ambiguous error:
/*
	Cec_delete_pUchar cbuf = new unsigned char[22];
	unsigned char *p = cbuf+1;
	==
	1>error C2593: 'operator +' is ambiguous
	1>          could be 'built-in C++ operator+(bool, int)'
	1>          or       'built-in C++ operator+(unsigned char *, int)'
	1>          while trying to match the argument list '(Cec_delete_pUchar, int)'
*/

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


// CHJ MEMO: CEnsureCleanupData's DATA_TYPE can be specialized to any type, 
// including any pointer-type like void*, char*, MyClass* etc.
// In other word, it can supersede CEnsureCleanupPtr, and it is really so for Visual C++ 98 and above.
// However, CEnsureCleanupPtr exists to cope with GCC(3.4~4.8~...)'s long existing stubborn stupid behavior of 
//
//		error: could not convert template argument '0' to 'void*'
//
template<class DATA_TYPE, class RET_TYPE, RET_TYPE (*pfn)(DATA_TYPE), DATA_TYPE tInvalid> 
class CEnsureCleanupData
{
	DATA_TYPE m_t;           // The member representing the object

public:
	// Default constructor assumes an invalid value (nothing to cleanup)
	CEnsureCleanupData() { m_t = tInvalid; }
	
	// This constructor sets the value to the specified value
	CEnsureCleanupData(DATA_TYPE t) : m_t(t) { }
	
	// The destructor performs the cleanup.
	~CEnsureCleanupData() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	bool IsValid() { return(m_t != tInvalid); }
	bool IsInvalid() { return(!IsValid()); }
	operator bool(){ return IsValid(); }
	bool operator !(){ return !IsValid(); }
	
	// Re-assigning the object forces the current object to be cleaned-up.
	DATA_TYPE operator=(DATA_TYPE t) 
	{ 
		Cleanup(); 
		m_t = t;
		return(*this);  
	}
	
	// Returns the value (supports both 32-bit and 64-bit Windows).
	operator DATA_TYPE() 
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



//////// CEnsureCleanupPtrArray and CEnsureCleanupIntArray
//
// If you have an array, in which every element should receive a cleanup operation, then use these two.
// For example, you have `HANDLE *arHandles = new HANDLE[n];` and each `arHandles[i]` stores
// a HANDLE returned from CreateThread(), then you can have CEnsureCleanupPtrArray manage `arHandles`,
// so that each `arHandles[i]` gets CloseHandle().

template<typename USER_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(USER_TYPE)> 
class CEnsureCleanupPtrArray
{
	int m_ArraySize;
	USER_TYPE *m_ar;           // This member representing the object array ptr

public:
	// Default constructor clears all pointer elements to invalid-value
	CEnsureCleanupPtrArray(int ArraySize) : m_ArraySize(ArraySize) {
		m_ar = new USER_TYPE[m_ArraySize];
		for(int i=0; i<m_ArraySize; i++) 
			m_ar[i] = NULL; 
	}
	
	// The destructor performs the cleanup.
	~CEnsureCleanupPtrArray() { 
		Cleanup(); 
	}
	
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
		for(int i=0; i<m_ArraySize; i++) {
			if (IsValid(i)) {
				pfn(m_ar[i]);         // Cleanup the object.
			}
		}
		delete m_ar;
	}
};

template<typename USER_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(USER_TYPE), USER_TYPE valInvalid> 
class CEnsureCleanupIntArray // Should give it a better name: no 'Int' in name.
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
	~CEnsureCleanupIntArray() { 
		Cleanup(); 
	}
	
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
		for(int i=0; i<m_ArraySize; i++) {
			if (IsValid(i)) {
				pfn(m_ar[i]);         // Cleanup the object.
			}
		}
		delete m_ar;
	}
};



#define MakeDelega_CleanupPtr(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, PTR_TYPE) \
	typedef CEnsureCleanupPtr< PTR_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction > CecClassName;
	// Note: There should be a space between pCleanupfunction and the right angle bracket, because
	// pCleanupFunction will later be replaced by _EnsureClnup_cpp_delete<PTR_TYPE> which would 
	// otherwise result in a bogus >> operator .

#define MakeDelega_CleanupAny(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, DATA_TYPE, ValueInvalid) \
	typedef CEnsureCleanupData<DATA_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, ValueInvalid> CecClassName;

#define MakeDelega_CleanupPtr_winapi(CecClassName, RetType, pCleanupFunction, PTR_TYPE) \
	inline RetType pCleanupFunction ## _Ptr__plain(PTR_TYPE ptr){ return pCleanupFunction(ptr); } \
	typedef CEnsureCleanupPtr< PTR_TYPE, RetType, pCleanupFunction ## _Ptr__plain > CecClassName;

#define MakeDelega_CleanupAny_winapi(CecClassName, RetType, pCleanupFunction, DATA_TYPE, ValueInvalid) \
	inline RetType pCleanupFunction ## __plain(DATA_TYPE h){ return pCleanupFunction(h); } \
	typedef CEnsureCleanupData<DATA_TYPE, RetType, pCleanupFunction ## __plain, ValueInvalid> CecClassName;
	// MakeCleanupClass_winapi provide a non-__stdcall wrapper so that CEnsureCleanupData can be used smoothly.


//

#define MakeCleanupPtrArrayClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, USER_TYPE) \
	typedef CEnsureCleanupPtrArray< USER_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction > CecClassName;

#define MakeCleanupIntArrayClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, USER_TYPE, valInvalid) \
	typedef CEnsureCleanupIntArray<USER_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, valInvalid> CecClassName;

/////////////////////////////////// C++ ///////////////////////////////////////


// Define a class named Cec_cxx_delete representing a void*-pointed memory block,
// that would be de-allocated by C++-delete.
inline void _EnsureClnup_cpp_delete_pvoid(void *p){ delete (char*)p; }
MakeDelega_CleanupPtr(Cec_cxx_delete, void, _EnsureClnup_cpp_delete_pvoid, void*)
	// 
	// Usage example:
	//
	//	Cec_cxx_delete cec_memblock = new unsigned char[1000];
	//	memcpy(cec_memblock, src, 1000);


template<typename T>
inline void _cxx_delete_1(T *pobj)
{
	delete pobj;
}

template<typename T>
inline void _cxx_delete_N(T *arobj)
{
	delete[] arobj;
}


// [2017-10-20]
#define MakeDelega_CleanupCxxPtr_en(UserClass, Classname_delete1, Classname_deleteN) \
	typedef CEnsureCleanupPtr< UserClass*, void, _cxx_delete_1<UserClass> > Classname_delete1; \
	typedef CEnsureCleanupPtr< UserClass*, void, _cxx_delete_N<UserClass> > Classname_deleteN; \
	// -- en: Explicit Naming of the two Cec class names
//
#define MakeDelega_CleanupCxxPtr(UserClass) MakeDelega_CleanupCxxPtr_en(UserClass, Cec_ ## UserClass, CecArray_ ## UserClass)
//
// So, a statement on global statement
//
//	MakeDelega_CleanupCxxPtr(CFoo)
//
// enables these features in function scope:
//
// Writing:
//		Cec_CFoo        pfoobar = new CFoo;
//		CecArray_CFoo  arfoobar = new CFoo[3];
// will automatically execute these destructions on leaving function scope:
//		delete   pfoobar;
//		delete[] arfoobar;


//// oldnames section >>>
////    =========== old names ===========  =========== new names ===========

#define MakeCleanupPtrClass                MakeDelega_CleanupPtr

#ifndef MakeCleanupClass // #if-check to avoid conflict with Jeffrey's old MakeCleanupClass
#define MakeCleanupClass                   MakeDelega_CleanupAny
#endif

#define MakeCleanupPtrClass_winapi         MakeDelega_CleanupPtr_winapi
#define MakeCleanupClass_winapi            MakeDelega_CleanupAny_winapi

#define Cec_NewMemory                      Cec_cxx_delete

//// oldnames section <<<


// [2025-02-09] Modern C++ style to apply `delete` 

#if __cplusplus >= 201103L || _MSC_VER >= 1800 // 1800 is VS2013

// C++11 or later, we have `using` keyword

template<typename T>
using CleanupDelega = CEnsureCleanupPtr< T*, void, _cxx_delete_1<T> >;

template<typename T>
using CleanupArrayDelega = CEnsureCleanupPtr< T*, void, _cxx_delete_N<T> >;

#endif

// Workaround for old-date C++98 and prior, including VS2010

template<typename T>
struct cleanupDelega // Staring 'c' letter in lowercase
{
	typedef CEnsureCleanupPtr< T*, void, _cxx_delete_1<T> > type;
};

template<typename T>
struct cleanupArrayDelega  // Staring 'c' letter in lowercase
{
	typedef CEnsureCleanupPtr< T*, void, _cxx_delete_N<T> > type;
};



//////////////////////////////////////////////////////////////////////////

/* Some usage examples:

MakeCleanupClass(Cec_LibcFh, int, close, int, -1)
	// Use ` int close(int); ' to cleanup a file handle opened by 
	// ` int open (const char *, int, ...); ' , and the invalid(default) value is -1 .

MakeCleanupPtrClass(Cec_pVoid, void, free, void*)
	// Use ` void free(void*); ' to cleanup a buffer allocated by 
	// ` void* malloc(size_t); ' .

// Windows API examples:
MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE);
	// This is for OpenProcess, who returns NULL as fail.
MakeCleanupClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE); 
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.



inline void free_unsigned_char_ptr(unsigned char * p){ free(p); }

MakeCleanupPtrClass(Cec_pUchar, void, free_unsigned_char_ptr, unsigned char *)
	// Use ` void free_unsigned_char_ptr(unsigned char * p); ' to cleanup a buffer 
	// allocated by ` void* malloc(size_t); ' . For the sake of function pointer
	// type matching, you have to define a wrapper function for free() .

class CFoo { ... };
MakeDelega_CleanupCxxPtr(CFoo) // equivalent to:
MakeDelega_CleanupCxxPtr_en(CMyClass, Cec_MyFoo, CecArray_MyFoo)


int main()
{
	Cec_LibcFh fh = open("_TestFile.txt", O_CREAT|O_RDWR|O_TRUNC ,_S_IWRITE);

	Cec_pVoid pv = malloc(100);

	Cec_pUchar puc = (unsigned char*)malloc(200);

	// [2025-02-09]
	// For C++11 or newer:
	CleanupDelega<CFoo> obj1 = new CFoo;
	CleanupArrayDelega<CFoo> objs = new CFoo[2];

	CleanupDelega<CFoo>::type      obj1 = new CFoo;
	CleanupArrayDelega<CFoo>::type objs = new CFoo[2];

	// Prior to C++11 (including VS2010):
	cleanupDelega<CFoo>::type      obj1 = new CFoo;
	cleanupArrayDelega<CFoo>::type objs = new CFoo[2];

	return 0;
}
*/

#endif
