#ifndef __JAutoBuf_h_20250329_
#define __JAutoBuf_h_20250329_
/******************************************************************************
Module:  JAutoBuf.h
Notices: Copyright (c) 2000 Jeffrey Richter
Purpose: This class manages an auto-sizing data buffer.
Origin:  [PSSA2000] book Appendix B.

Updates:
	* Chj uses new/delete to allocate/free memory-buffer.

Chj Note:
	* (TO DEL)This code cannot work on systems that sizeof(int)!=sizeof(long), 64-bit Linux for example.

******************************************************************************/

/////////////////// JAutoBuf Template C++ Class Description ///////////////////

/*  ==== Usage ====

The JAutoBuf template C++ class implements type safe buffers that
automatically grow to meet the needs of your code.  Memory is also
automatically freed when the object is destroyed (typically when your
code goes out of scope and it is popped off of the stack).

Examples of use:

   // Create a buffer with no explicit data type, 
   // the buffer grown in increments of a byte
   JAutoBuf<void> buf;

   // Create a buffer of TCHARs, 
   // the buffer grows in increments of sizeof(TCHAR)
   JAutoBuf<TCHAR, sizeof(TCHAR)> buf; 

   // Force the buffer to be 10 TCHARs big
   buf = 10;

Note: To use this lib, pick one and only one of your xxx.cpp, write at its start:

#define JAUTOBUF_IMPL
#include "JAutoBuf.h"

*/


///////////////////////////////////////////////////////////////////////////////


// This class is only ever used as a base class of the JAutoBuf template class.
// The base class exists so that all instances of the template class share
// a single instance of the common code.
class JAutoBufBase 
{
public:
	typedef unsigned long BufLen_t; // in eles. 

	BufLen_t SizeMin()
	{
		if(m_ReqEle<=m_CurEle)
			return m_ReqEle;
		else
			return m_CurEle;
	}

	BufLen_t Size() 
	{ 
		// AutoBuf user calls Size() to get a value to tell WinAPI his 
		// "current" buffer size (in eles). 
		//
		// If current m_ReqEle>m_CurEle, Size() will try to increase buffer to m_ReqEle.
		// * If increase success, m_CurEle gets updated to be equal to m_ReqEle.
		// * If increase fail, m_CurEle remains intact.
		AdjustBuffer();
		return SizeMin(); 
	}

	BufLen_t Size(BufLen_t uSize);

	BufLen_t* PSize() 
	{ 
		// AutoBuf user calls PSize() to get a value-ptr so that WinAPI will fill this value
		// with required buffer size(in eles). 
		// If current m_ReqEle>m_CurEle, Size() will try to increase buffer to m_ReqEle.
		// * If increase success, m_CurEle gets updated to be equal to m_ReqEle, all well.
		// * If increase fail, m_ReqEle remains intact(no need to reverted to m_CurEle).
		AdjustBuffer();
		return &m_ReqEle; 
	}
	
	void Free() { 
		Reconstruct(); 
	}

	void Discard() {
		// [2025-03-29] User should have grabbed the buffer ptr(via JAutoBuf.BufPtr())
		// and user should C++-delete the buffer later.
		Reconstruct(true);
	}

	bool IsOK(){
		return m_CurEle>=m_ReqEle;
	}

	typedef unsigned char *PBYTE_t;

protected:
	JAutoBufBase(PBYTE_t *ppbData, int nMult, int ExtraEle)
	{
		m_nMult = nMult;
		m_ppbBuffer = ppbData; 
			// Derived class holds address of buffer to allow
			// debugger's Quick Watch to work with typed data.
		m_ExtraEle = ExtraEle;
		Reconstruct(true);
	}

	// virtual? // Chj: I don't think it needs virtual
	~JAutoBufBase() { 
		Free();
	}

	void Reconstruct(bool fFirstTime = false);

	PBYTE_t Buffer() { 
		AdjustBuffer(); 
		return(*m_ppbBuffer); 
	}

private:
	void AdjustBuffer();  
	// Chj: Try to increase class internal buffer to m_ReqEle, but can possibly fail.
	// If fail, the original buffer content remains intact, and m_ReqEle is reset to m_CurEle.

private:
	PBYTE_t *m_ppbBuffer;    // Address of address of data buffer
	int      m_nMult;        // Multiplier (in bytes) used for buffer growth
	BufLen_t m_ReqEle; // Requested buffer size (in m_nMult units)
	BufLen_t m_CurEle; // Actual size (in m_nMult units)

	int m_ExtraEle;  
		// User can request extra bytes, for example, for possible NUL to append.
};


///////////////////////////////////////////////////////////////////////////////


template <class Type, int Mult=1, int Extra=0> 
class JAutoBuf : public JAutoBufBase 
{
public:
	JAutoBuf() : JAutoBufBase((PBYTE_t*) &m_pData, Mult, Extra) {}

	JAutoBuf(int init_size) : JAutoBufBase((PBYTE_t*) &m_pData, Mult, Extra) 
	{
		Size(init_size);
	}
	
	//	void Free() { JAutoBufBase::Free(); } // Chj comments it.

	void *Bufptr() { return m_pData; }

public:
	operator Type*()  { return Buffer(); }

unsigned int operator=(unsigned int eleCount) { return JAutoBufBase::Size(eleCount); }

	// unsigned short/int/long returns the m_CurEle value
	operator unsigned short() { return Size(); }
	operator int()            { return Size(); }
	operator unsigned int()   { return Size(); }
	operator long()           { return Size(); }
	operator unsigned long()  { return Size(); }

	// unsigned short*/int*/long* returns the m_ReqEle's address
	operator unsigned short*(){ return (unsigned short*)PSize(); }
	operator int*()           { return (int*)PSize(); }
	operator unsigned int*()  { return (unsigned int*)PSize(); }
	operator long*()          { return (long*)PSize(); }
	operator unsigned long*() { return (unsigned long*)PSize(); }

	/// unsigned char*/void* returns the buffer address.
//	operator PBYTE_t()  { return (PBYTE_t)Buffer(); }
//	operator char*()  { return (char*)Buffer(); }
	operator void*()  { return Buffer(); }

	Type& operator[](int nIndex) { return *(Buffer() + nIndex); }

	Type* Buffer() 
	{ 
		return (Type*)JAutoBufBase::Buffer(); // buffer size adjusted(verified).
	}

private:
	Type* m_pData;
};


///////////////////////////////////////////////////////////////////////////////


#define GROWUNTIL(fail, func)                        \
   do {                                              \
      if ((func) != (fail))                          \
         break;                                      \
   } while ((GetLastError() == ERROR_MORE_DATA) ||   \
            (GetLastError() == ERROR_INSUFFICIENT_BUFFER));


///////////////////////////////////////////////////////////////////////////////


#ifdef JAUTOBUF_IMPL // only one .cpp should define this to get the implementation code

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////

void JAutoBufBase::Reconstruct(bool fFirstTime) 
{
	if (!fFirstTime) 
	{
		if (*m_ppbBuffer != NULL) {
			//HeapFree(GetProcessHeap(), 0, *m_ppbBuffer);
			delete *m_ppbBuffer;
		}
	}

	*m_ppbBuffer = NULL; // Derived class doesn't point to a data buffer
	m_ReqEle = 0;      // Initially, buffer has no bytes in it
	m_CurEle = 0;  // Initially, buffer has no bytes in it
}

///////////////////////////////////////////////////////////////////////////////

JAutoBufBase::BufLen_t JAutoBufBase::Size(BufLen_t eleCount) 
{
	// Set buffer to desired number of m_nMult bytes.
	if (eleCount == 0) {
		Reconstruct();
	} else {
		m_ReqEle = eleCount;
		AdjustBuffer();
	}
	return SizeMin();
}

///////////////////////////////////////////////////////////////////////////////

void JAutoBufBase::AdjustBuffer()
{
	if (m_CurEle < m_ReqEle) 
	{
		// We're growing the buffer
/*
		HANDLE hHeap = GetProcessHeap();

		if (*m_ppbBuffer != NULL) {
			// We already have a buffer, re-size it
			PBYTE pNew = (PBYTE) 
				HeapReAlloc(hHeap, 0, *m_ppbBuffer, m_uNewSize * m_nMult);
			if (pNew != NULL) {
				m_uCurrentSize = m_uNewSize;
				*m_ppbBuffer = pNew;
			} 
		} else {
			// We don't have a buffer, create new one.
			*m_ppbBuffer = (PBYTE) HeapAlloc(hHeap, 0, m_uNewSize * m_nMult);
			if (*m_ppbBuffer != NULL) 
				m_uCurrentSize = m_uNewSize;
		}
*/
		assert(m_ReqEle>0);
		int newsize = (m_ReqEle+m_ExtraEle)*m_nMult;
		PBYTE_t newbuf = new unsigned char[newsize];
		if(!newbuf)
			return; 

		// For best compatibility to various Windows API, we'd better copy 
		// old content to new buffer. E.g. SetupDiGetDeviceInterfaceDetail() needs this.
		if(*m_ppbBuffer)
		{
			memcpy(newbuf, *m_ppbBuffer, (m_CurEle+m_ExtraEle)*m_nMult);
		}
		else
		{
			// We nullify first bytes of the new space, so that, when this space is 
			// used to store C string, user will see a NUL-terminated string.
			memset(newbuf, 0, (newsize<4 ? newsize : 4));
		}

		delete *m_ppbBuffer;
		*m_ppbBuffer = newbuf;
		m_CurEle = m_ReqEle;
	}
}

///////////////////////////////////////////////////////////////////////////////


#endif   // JAUTOBUF_IMPL



typedef JAutoBuf<wchar_t, sizeof(wchar_t), 1>  CWstring_autobuf;
typedef JAutoBuf<char, sizeof(char), 1>        CAstring_autobuf;

typedef JAutoBuf<unsigned char>   Jautobuf; // to store byte stream (void* buffer)

// Suggestion on Windows for TCHAR buffer:
// typedef JAutoBuf<TCHAR, sizeof(TCHAR), 1> AutoTCHARs;



template<typename TAutobuf>
bool abIsEmptyString(TAutobuf &ab)
{
	if( (void*)ab==NULL || ab[0]==0 )
		return true;
	else 
		return false;
}


#endif // include once guard
