#ifndef __JAutoBuf_h_20250608_
#define __JAutoBuf_h_20250608_
/******************************************************************************
Module:  JAutoBuf.h
Notices: Copyright (c) 2000 Jeffrey Richter
Purpose: This class manages an auto-sizing data buffer.
Origin:  [PSSA2000] book Appendix B.

Updates:
* Chj uses new/delete to allocate/free memory-buffer.
* Add move-constructors, move-assignment for C++11.

Chj Note:
* (TO DEL)This code cannot work on systems that sizeof(int)!=sizeof(long), 64-bit Linux for example.
* Need `union` update before porting to big-endian CPU.

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
	typedef __int64 BufEles_t; // in eles. 
	typedef __int64 BufBytes_t;

	BufEles_t SizeMin()
	{
		if(m_ReqEle<=m_CurEle)
			return m_ReqEle;
		else
			return m_CurEle;
	}

	BufBytes_t Bytes() const
	{
		return Size() * m_nMult;
	}

	BufEles_t Size() const
	{
		return m_ReqEle * m_nMult;
	}

	BufBytes_t Bytes()
	{
		return Size() * m_nMult;
	}

	BufEles_t Size()
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

	BufEles_t Size(BufEles_t uSize);

	BufEles_t* PSize() 
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
	void _steal_from_old(JAutoBufBase &old)
	{
		*m_ppbBuffer = *(old.m_ppbBuffer);
		m_nMult = old.m_nMult;
		m_ReqEle = old.m_ReqEle;
		m_CurEle = old.m_CurEle;
		m_ExtraEle = old.m_ExtraEle;

		old.Discard();
	}

	void _base_move_ctor(JAutoBufBase& old);
	void _base_move_assignment(JAutoBufBase& old);

protected:
	JAutoBufBase(PBYTE_t *ppbData, int nMult, int ExtraEle);

	// virtual? // Chj: I don't think it needs virtual
	~JAutoBufBase();

	void Reconstruct(bool fFirstTime = false);

	PBYTE_t Buffer() { 
		AdjustBuffer(); 
		return(*m_ppbBuffer); 
	}

private:
	void AdjustBuffer();  
	// -- Chj: Try to increase class internal buffer to m_ReqEle, but can possibly fail.
	// If fail, the original buffer content remains intact, and m_ReqEle is reset to m_CurEle.

private:
	PBYTE_t  *m_ppbBuffer;    // Address of address of data buffer
	int       m_nMult;        // Multiplier (in bytes) used for buffer growth
	BufEles_t m_ReqEle; // Requested buffer size (in m_nMult units)
	BufEles_t m_CurEle; // Actual storage size (in m_nMult units)

	int m_ExtraEle;  // User can request extra bytes, for example, for possible NUL to append.
};


///////////////////////////////////////////////////////////////////////////////


template <class Type, int Mult=sizeof(Type), int Extra=0> 
class JAutoBuf : public JAutoBufBase 
{
public:
	JAutoBuf() : JAutoBufBase((PBYTE_t*)&m_pData, Mult, Extra) {}

	JAutoBuf(int init_size) : JAutoBufBase((PBYTE_t*)&m_pData, Mult, Extra) 
	{
		Size(init_size);
	}
	
	JAutoBuf(JAutoBuf&& old) // Move-constructor
		: JAutoBufBase((PBYTE_t*)&m_pData, Mult, Extra)
	{
		// Note: avoid using `std::move(old)` in public "user" header,
		// bcz that would introduce extra <xutility> .
		_base_move_ctor(old); 
	}

	JAutoBuf& operator=(JAutoBuf&& old) // Move-assignment
	{
		// Note: avoid using `std::move(old)` in public "user" header
		// bcz that would introduce extra <xutility> .
		_base_move_assignment(old);
		return *this;
	}

	Type *Bufptr() { return m_pData; }
	const Type *Bufptr() const { return m_pData; }

	Type *Ptr() { return m_pData; }
	const Type *Ptr() const { return m_pData; }

public:
	operator Type*()  { return Buffer(); }

	unsigned int operator=(unsigned int eleCount) { return JAutoBufBase::Size(eleCount); }

	// unsigned short/int/long returns the m_CurEle value
	operator unsigned short() { return (unsigned short)Size(); }
	operator int()            { return (int)          Size(); }
	operator unsigned int()   { return (unsigned int) Size(); }
	operator long()           { return (long)         Size(); }
	operator unsigned long()  { return (unsigned long)Size(); }
	operator __int64()        { return Size(); }
	operator unsigned __int64() { return Size(); }

	// unsigned short*/int*/long* returns the m_ReqEle's address
	operator unsigned short*(){ return (unsigned short*)PSize(); }
	operator int*()           { return (int*)PSize(); }
	operator unsigned int*()  { return (unsigned int*)PSize(); }
	operator long*()          { return (long*)PSize(); }
	operator unsigned long*() { return (unsigned long*)PSize(); }
	operator __int64*()       { return (__int64*)PSize(); }
	operator unsigned __int64*() { return (unsigned __int64*)PSize(); }

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
	// Chj: Bcz we want the `Type` to be a template-type param, we have to define 
	// `m_pData` in derived class, a template-class.
	//
	Type* m_pData;

#if (__cplusplus>=201402L) || (_MSC_VER>=1900) 	// C++14 or VC2015+

	// Disable copy-ctor and copy-assignment.
	JAutoBuf(const JAutoBuf&) = delete;
	JAutoBuf& operator=(const JAutoBuf&) = delete;

#endif
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
#include <utility>

#ifdef JAUTOBUF_DEBUG
#undef  vaDBG      // revoke empty effect 
#else
#define vaDBG(...) // make vaDBG empty, no debugging message
#endif

///////////////////////////////////////////////////////////////////////////////

JAutoBufBase::JAutoBufBase(PBYTE_t *ppbData, int nMult, int ExtraEle)
{
	vaDBG(_T("[JAutoBuf@%p] ctor (+%d *%d)."), this, ExtraEle, nMult);

	m_nMult = nMult;
	m_ppbBuffer = ppbData;
	// -- Jeffrey: Derived class holds address of buffer to allow
	//    debugger's Quick Watch to work with typed data.
	
	m_ExtraEle = ExtraEle;
	Reconstruct(true);

}

JAutoBufBase::~JAutoBufBase() 
{
	vaDBG(_T("[JAutoBuf@%p] dtor."), this);

	Free();
}

void JAutoBufBase::Reconstruct(bool fFirstTime) 
{
	if (!fFirstTime) 
	{
		if (*m_ppbBuffer != NULL) 
		{
			vaDBG(_T("[JAutoBuf@%p] delete memblock @%p."), this, *m_ppbBuffer);

			//HeapFree(GetProcessHeap(), 0, *m_ppbBuffer);
			delete *m_ppbBuffer;
		}
	}

	*m_ppbBuffer = NULL; // Derived class doesn't point to a data buffer
	m_ReqEle = 0;  // Initially, buffer has no bytes in it
	m_CurEle = 0;  // Initially, buffer has no bytes in it
}


JAutoBufBase::BufEles_t JAutoBufBase::Size(BufEles_t eleCount) 
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


void JAutoBufBase::AdjustBuffer()
{
	if (m_CurEle < m_ReqEle) 
	{
		// We're growing the buffer

		assert(m_ReqEle>0);
		__int64 newsize = (m_ReqEle+m_ExtraEle)*m_nMult;
		PBYTE_t newbuf = new unsigned char[(size_t)newsize]; // 32bit compiler hopes to have [uint]
		if(!newbuf)
			return; 

		vaDBG(_T("[JAutoBuf@%p] size inc %llu -> %llu (%lld bytes) ; moving memblock @%p -> @%p."), this, 
			m_CurEle, m_ReqEle,  newsize,  *m_ppbBuffer, newbuf);

		// For best compatibility to various Windows API, we'd better copy 
		// old content to new buffer. E.g. SetupDiGetDeviceInterfaceDetail() needs this.
		if(*m_ppbBuffer)
		{
			memcpy(newbuf, *m_ppbBuffer, (size_t)((m_CurEle+m_ExtraEle)*m_nMult));
		}
		else
		{
			// We nullify first bytes of the new space, so that, when this space is 
			// used to store C string, user will see a NUL-terminated string.
			memset(newbuf, 0, size_t(newsize<4 ? newsize : 4));
		}

		delete *m_ppbBuffer;
		*m_ppbBuffer = newbuf;
		m_CurEle = m_ReqEle;
	}
}

void JAutoBufBase::_base_move_ctor(JAutoBufBase& old)
{
	vaDBG(_T("[JAutoBuf@%p] move-ctor from old @%p"), this, &old);

	_steal_from_old(std::move(old));
}

void JAutoBufBase::_base_move_assignment(JAutoBufBase& old)
{
	if (this != &old)
	{
		vaDBG(_T("[JAutoBuf@%p] move-assignment from old @%p"), this, &old);

		this->Free();

		_steal_from_old(old);
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
bool abIsEmptyString(const TAutobuf &ab)
{
	if( (void*)ab==NULL || ab[0]==0 )
		return true;
	else 
		return false;
}


#endif // include once guard
