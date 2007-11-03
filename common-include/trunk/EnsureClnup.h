#ifndef __ENSURECLNUP_H
#define __ENSURECLNUP_H

/* The idea comes from Jeffrey Richter's CEnsureCleanup template class, 
 which is presented in his book 
   << Programming Server-Side Applications for Microsoft Windows 2000 >>

	But I found that his original CEnsureCleanup is not acceptable when
 compiling with GCC 3.2, and with the compiler from Borland C++ 5.5 .

	So I did the improvement.
*/


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
	int IsValid() { return(m_t != NULL); }
	int IsInvalid() { return(!IsValid()); }
	
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
	
	PTR_TYPE operator->()
	{
		return m_t;
	}

	PTR_TYPE operator[](int idx)
	{
		return &m_t[idx]; // User should guarantee m_t really points to an array
	}

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
	int IsValid() { return(m_t != tInvalid); }
	int IsInvalid() { return(!IsValid()); }
	
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

//////// CEnsureCleanup_array: 
// If you have an array, in which every element should receive a cleanup operation, then use this.

template<typename USER_TYPE, typename RET_TYPE, RET_TYPE (*pfn)(USER_TYPE), USER_TYPE valInvalid> 
class CEnsureCleanup_array
{
	int m_ArraySize;
	USER_TYPE *m_ar;           // This member representing the object array ptr

public:
	// Default constructor clears all pointer elements to invalid-value
	CEnsureCleanup_array(int ArraySize) : m_ArraySize(ArraySize) {
		m_ar = new USER_TYPE[m_ArraySize];
		for(int i=0; i<m_ArraySize; i++) m_ar[i] = valInvalid; 
	}
	
	// The destructor performs the cleanup.
	~CEnsureCleanup_array() { Cleanup(); }
	
	// Helper methods to tell if the value represents a valid object or not..
	int IsValid(int idx) { return(m_ar[idx] != NULL); }
	int IsInvalid(int idx) { return(!IsValid(idx)); }

	USER_TYPE& operator[](int idx) {
		return m_ar[idx];
	}

	operator USER_TYPE*() {
		return m_ar; // Return the address of the first array element.
	}

	// Re-assigning the object forces the current object to be cleaned-up.
// 	PTR_TYPE operator=(PTR_TYPE t) 
// 	{ 
// 		Cleanup(); 
// 		m_t = t;
// 		return(*this);  
// 	}
	
	void Cleanup() 
	{ 
		for(int i=0; i<m_ArraySize; i++)
			if (IsValid(i)) {
				pfn(m_ar[i]);         // Cleanup the object.
			}
		delete m_ar;
	}
};


#define MakeCleanupPtrClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, PTR_TYPE) \
	typedef CEnsureCleanupPtr<PTR_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction> CecClassName;

#define MakeCleanupIntClass(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, INT_TYPE, IntValueInvalid) \
	typedef CEnsureCleanupInt<INT_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, IntValueInvalid> CecClassName;


#define MakeCleanupPtrClass_delete(CecClassName, PTR_TYPE) \
	inline void __delete_##CecClassName(PTR_TYPE p) { delete p; } \
	MakeCleanupPtrClass(CecClassName, void, __delete_##CecClassName, PTR_TYPE)

#define MakeCleanupPtrClass_delete_array(CecClassName, PTR_TYPE) \
	inline void __delete_##CecClassName(PTR_TYPE p) { delete []p; } \
	MakeCleanupPtrClass(CecClassName, void, __delete_##CecClassName, PTR_TYPE)


#define MakeCleanupClass_array(CecClassName, RET_TYPE_of_CleanupFunction, pCleanupFunction, USER_TYPE, valInvalid) \
	typedef CEnsureCleanup_array<USER_TYPE, RET_TYPE_of_CleanupFunction, pCleanupFunction, valInvalid> CecClassName;

#define MakeCleanupClass_delete_ptr_array(CecClassName, PTR_TYPE) \
	inline void __delete_##CecClassName(PTR_TYPE p) { delete p; } \
	MakeCleanupClass_array(CecClassName, void, __delete_##CecClassName, PTR_TYPE, 0)

//////////////////////////////////////////////////////////////////////////

/* Some usage examples:

MakeCleanupIntClass(Cec_LibcFh, int, close, int, -1)
	// Use ` int close(int); ' to cleanup a file handle opened by 
	// ` int open (const char *, int, ...); ' , and the invalid(default) value is -1 .


MakeCleanupPtrClass(Cec_pVoid, void, free, void*)
	// Use ` void free(void*); ' to cleanup a buffer allocated by 
	// ` void* malloc(size_t); ' .

MakeCleanupClass_array(CecFpAr, int, fclose, FILE*, NULL)

MakeCleanupClass_array(CecFhAr, int, close, int, -1)


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
