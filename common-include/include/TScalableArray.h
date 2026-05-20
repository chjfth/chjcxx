#ifndef __TScalableArray_h_
#define __TScalableArray_h_
#define __TScalableArray_h_created_ 20050101
#define __TScalableArray_h_updated_ 20260520

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>

#include <commdefs.h>
#include <_MINMAX_.h>

// TScalableArray may be written as TSA for brevity.

/*
[2005-10-08] The name TScalableArray starts with a 'T', which indicates it's a template class.

[2026-05-20] User can choose two flavors of Element-type T inside TSA.

  [Flavor One] T can be C++ class object. This is natural. 
	
	T's ctor/dtor will be invoked automatically when required. To be precise:

	[A]	When new element-storage is allocated, every new element will be constructed by
		no-parameter ctor of T. So user's T must supply such ctor to compile success.
	
	[B]	When(just before) an old element is dropped from RAM, including when TSA itself 
		is destructed, old element's C++ destructor is called.
	
	[C]	A TSA object itself can be copied, at that time, all elements in new TSA is 
		constructed by calling T's copy-constructor on elements from old TSA.

  [Flavor Two] TSA manages T's storage. User can delay calling ctor for T.

	[a] User calls `tsa.SetEleQuan(N, true)` to increase/decrease storage for T's array. 
		That second param `true` tells TSA to zero out the new element's storage to zeros,
		instead of calling T's ctor. So user can postpone construction of those T objects
		manually.

	[b] User's T must provide a no-param ctor that initialize T to a plain-old object,
		for example, initialize all object body bytes to zero.
		And, T's dtor should cope with zero-content object body normally.

	[c] There is NOT a object-wide switch that controls whether TSA looks upon T as
		flavor-one or flavor-two, so, even you use flavor-two, TSA still considers T 
		as C++ object after SetEleQuan() gives birth to T's storage. For example:
		* When you use SetEleQuan() to shrink a TSA, those dropped T objects will first
		  be called with their dtor(). TSA does this to avoid memleak from suddenly
		  vanishing T-s.
		* When TSA itself is destructed, the remaining T-s dtor will be called automatically.

	[d] Can a T with virtual functions be used with flavor-two?
		Yes. Although a half-initialized T object is zeroed-out with all vtable-ptr wiped off,
		TSA itself always call T's dtor directly, not through vtable-ptr.
		As long as TSA's user does not return such half-initialized T to outer-user,
		everything will be fine.

	Flavor-Two has been applied on chjds::hashdict template class successfully, with [d].
*/

const int TSA_no_decrease = 0; // use for DecSize param


template<typename T>
class TScalableArray
{
public:
	enum ReCode_et // error codes
	{
		E_Success = 0,
		E_Unknown = -1,
		E_NoMem = -2,
//		E_InvalidParam = -3,
		E_Full = -4,

		// All below means invalid-params
		E_InvalidParam = -10,
		E_IncSizeMustBeAtleast1 = -11,
		E_DecSizeShouldBeMultipleOfIncSize = -12,
		E_MaxEleLessthanCurEle = -13,
		E_MaxEleShouldBeMultipleOfIncSize = -14,
	};
	typedef ReCode_et ReCode_t;

private:
	enum
	{
		DEF_INCSIZE = 8,
		DEF_DECSIZE = DEF_INCSIZE*2,
		DEF_DECTHRES = 1
	};

public:

	TScalableArray(int MaxEle, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE,
		int DecThres=DEF_DECTHRES);
	
	ReCode_t SetTrait(int MaxEle, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE,
		int DecThres=DEF_DECTHRES);

	int CurrentEles() const { return m_nCurEle; }
	int CurrentStorage() const { return m_nCurStorage; }

	ReCode_t GetEles(int pos, T* pEle, int n) const;
	ReCode_t GetEle(int pos, T* pEle) const
	{
		return GetEles(pos, pEle, 1);
	}

	T* GetElePtr(int pos=0);
		// Note: Adding or deleting elements may cause the returned pointer to become invalid.
		// [2008-08-27] Not returning const ptr, since user should be allowed to modify any element.

	T& GetEleRef(int pos) const; // pos can be negative

	T& operator[](int pos) { return GetEleRef(pos); }
	const T& operator[](int pos) const { return GetEleRef(pos); }
	// --
	// [2026-05-06] or shall I define a single:
	//   TU& operator[](int pos) const;
	// instead of the above two?

	operator T* ()
	{
		if(m_nCurEle>0)
			return mar_Ele;
		else
			return NULL;
	}

	// operator bool() { return (T*)(*this) ? true : false; }
	// -- Don't define this `bool operator` bcz sth like
	//	char *pc = sa+1;
	// will cause compilation error. E.g. on VS2010 x86 compiler:
#if 0
d:\ws\common-include\autotest\mytest-ci\test_tscalablearray.cpp(408): error C2593: 'operator +' is ambiguous
	  could be 'built-in C++ operator+(bool, int)'
	  or       'built-in C++ operator+(char *, int)'
	  while trying to match the argument list '(TSA_char, int)'
#endif

	bool operator !() { return (T*)(*this) ? false : true; } // check for empty array
		
	ReCode_t InsertBefore(int pos, const T array[], int n);
		// Insert `n' elements at position `pos'. Insert all or insert none.
	ReCode_t InsertBefore(int pos, const T tobj)
	{
		return InsertBefore(pos, &tobj, 1);
	}

	ReCode_t AppendTail(const T* array, int n)
	{
		return InsertBefore(m_nCurEle, array, n);
	}
	ReCode_t AppendTail(const T tobj)
	{
		return InsertBefore(m_nCurEle, tobj);
	}

	ReCode_t AppendAt(int pos, const T* array, int n);
	ReCode_t AppendAt(int pos, const T tobj);
		// Q: Why not use ``const T &tobj'' as 2nd param ?
		// Answer:
		// 1. Sometimes, user prefer to provide a const(enum value etc) as 2nd param. 
		//    In this case, passing a const to a reference type may be forbidden by the compiler.
		// 2. If user's T object is large and he wants to avoid object memory copy, 
		//    he can instead use ``AppendAt(int pos, const T* array, int n);'' for efficiency.

	ReCode_t DeleteEles(int pos, int n);
		// A too large `n' means to delete all eles starting at `pos'
	ReCode_t DeleteEle(int pos)
	{
		return DeleteEles(pos, 1);
	}
	void DeleteAllEles()
	{
		DeleteEles(0, m_nCurEle);
	}
	
	bool DeleteMatch(const T& match)
	{
		int eles = CurrentEles();
		for(int i=0; i<eles; i++)
		{
			if(mar_Ele[i]==match)
			{
				DeleteEle(i);
			}
		}
		return false;
	}

	void DeleteAllStorage();

	void ZeroEles(int pos=0, int n=-1)
	{
		if(n<0)
			n = m_nCurStorage;
		
		pos = _MID_(0, pos, m_nCurStorage);
		n = _MID_(0, n, m_nCurStorage-pos);
		
		if(n>0) 
			memset(mar_Ele+pos, 0, n*sizeof(T));
	}

	ReCode_t SetEleQuan(int nNewEle, bool isZeroContent=false);
		//!< Change the array size to accommodate at least nNewEle elements.
		/*!< After calling SetEleQuan(), original elements in the array will not be changed.

		@param[in] isZeroContent 
			If true, I will not call T's ctor for those new elements.
			This means, user should/would manually control the construction process on those 
			newly allocated RAM space.
		*/

 	void Cleanup()
 	{
		assert( !(m_nCurEle==0 && mar_Ele!=nullptr) );
 		DeleteAllStorage();
// 
// 		// old code, probably wrong:
// 		if(m_nCurEle==0 && mar_Ele)
// 			DeleteAllStorage();
 	}

	int GetIncSize() { return m_nIncSize; }
	int GetDecSize() { return m_nDecSize; }

	struct Internal_st
	{
		int ele, sto;
		Uint reallocs;
	};
	Internal_st GetInternal() const
	{
		Internal_st st = {m_nCurEle, m_nCurStorage, m_reallocs};
		return st;
	}

public:
	// boilerplate code, no need to modify >>>
	TScalableArray() { _ct0r(); }
	~TScalableArray() { _dtor(); }

	TScalableArray(const TScalableArray& old)            // copy-ctor
	{
		_copy_from_old(old); 
	}
	TScalableArray& operator=(const TScalableArray& old) // copy-assign
	{
		if (this != &old) {
			_dtor();
			_copy_from_old(old);
		}
		return *this;
	}
	TScalableArray(TScalableArray&& old)            // move-ctor
	{
		_steal_from_old(old);
		old._ct0r();
	}
	TScalableArray& operator=(TScalableArray&& old) // move-assign
	{
		if (this != &old) {
			_dtor();
			_steal_from_old(old);
			old._ct0r();
		}
		return *this;
	}
	// boilerplate code, no need to modify <<<

protected:
	void _dtor() { 
		DeleteAllStorage(); 
	}

	void _copy_from_old(const TScalableArray& old);
	void _steal_from_old(TScalableArray& old);

	void ShiftDownEles(int pos, int n);
	void ShiftUpEles(int pos, int n);

	void CopyInEles(int pos, int n, const T* pIn);
	void CopyOutEles(int pos, int n, T* pOut) const;

	ReCode_t ExtendEles(int nTotalEles, bool isCtorNow);

protected:
	// Memory allocation/free functions, like ANSI C's realloc() and free().
//	static void *Malloc(size_t new_size);
	static void *Realloc(void *oldbuf, size_t new_size);
	static void Free(void *ptr);

protected:
	T* mar_Ele;	// pointer to the dynamically allocated array of type T
	
	int m_nCurEle; // current elements
	int m_nCurStorage; // current storage allocated (in element)

	int m_nMaxEle; // max elements 
	int m_nIncSize;
	int m_nDecSize;
	int m_nDecThres; // storage decreasing threshold

	Uint m_reallocs; // memory reallocation count, for self test

protected:
	void _ct0r()
	{
		mar_Ele = NULL;
		m_nCurEle = 0;
		m_nCurStorage = 0;

		m_nMaxEle = 0x7FFFffff;
		m_nIncSize = DEF_INCSIZE; 
		m_nDecSize = DEF_DECSIZE;
		m_nDecThres = DEF_DECTHRES;

		m_reallocs = 0;
	}
};

template<typename T>
bool IsTsaErr(T err)
{ 
	return err ? true : false; 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


template<typename T>
void TScalableArray<T>::_copy_from_old(const TScalableArray& old)
{
	m_nMaxEle = old.m_nMaxEle;
	m_nIncSize = old.m_nIncSize;
	m_nDecSize = old.m_nDecSize;
	m_nDecThres = old.m_nDecThres;
	m_reallocs = 0;

#ifndef _XXX_SEE_REALLOC_CRASH // should NOT #define this, just for experiment
	mar_Ele = nullptr;
#endif
	m_nCurEle = m_nCurStorage = 0;

	ReCode_et err = ExtendEles(old.m_nCurEle, false);
	assert(!err); // probably no-mem
	if(err)
		throw std::bad_alloc();

	assert(m_nCurEle==old.m_nCurEle);

	// We must use T's copy-ctor instead of raw memcpy();
	for(int i=0; i<m_nCurEle; i++)
	{
		new(mar_Ele+i) T( old[i] );
	}
}

template<typename T>
void TScalableArray<T>::_steal_from_old(TScalableArray& old)
{
	m_nCurEle = old.m_nCurEle;
	m_nCurStorage = old.m_nCurStorage;
	mar_Ele = old.mar_Ele;

	m_nMaxEle = old.m_nMaxEle;
	m_nIncSize = old.m_nIncSize;
	m_nDecSize = old.m_nDecSize;
	m_nDecThres = old.m_nDecThres;
	m_reallocs = old.m_reallocs;

	old.mar_Ele = NULL;
	old.m_nCurEle = old.m_nCurStorage = 0;
}


template<typename T>
TScalableArray<T>::TScalableArray(int MaxEle, int IncSize, int DecSize, int DecThres)
{
	_ct0r();

	ReCode_t err = SetTrait(MaxEle, IncSize, DecSize, DecThres);
	assert(!err);
	if(err) {
		_ct0r();
	}
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::SetTrait(int MaxEle, int IncSize, int DecSize, int DecThres)
{
	if(MaxEle<=0 || DecSize<0 || DecThres<0)
	{
		return E_InvalidParam;
	}

	if(DecSize==TSA_no_decrease)
		DecSize = MaxEle;

	if(IncSize<=0)
		return E_IncSizeMustBeAtleast1;

	if(MaxEle%IncSize != 0)
		return E_MaxEleShouldBeMultipleOfIncSize;

	if(DecSize%IncSize!=0)
		return E_DecSizeShouldBeMultipleOfIncSize;

	if(MaxEle<m_nCurEle)
		return E_MaxEleLessthanCurEle;

	// If DecSize==0, it means the storage will increase forever, never decrease,
	// unless a Cleanup() is called.

	m_nMaxEle = MaxEle;
	m_nIncSize = IncSize;
	m_nDecSize = DecSize;
	m_nDecThres = DecThres;

	return E_Success;
}


template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::GetEles(int pos, T* pEle, int n) const
{
	if(pos<0 || pos>m_nCurEle || n<0 || pEle==NULL) 
		return E_InvalidParam;
	
	CopyOutEles(pos, _MIN_(n, m_nCurEle-pos), pEle);
	return E_Success;
}

template<typename T>
T* 
TScalableArray<T>::GetElePtr(int pos)
{
	if(pos<0 || pos>=m_nCurEle || !mar_Ele) 
		return NULL;
	
	return mar_Ele+pos;
}

template<typename T>
T& 
TScalableArray<T>::GetEleRef(int pos) const
{
	if(pos>=0 && pos<m_nCurEle)
	{
		return mar_Ele[pos];
	}
	else if(pos<0 && pos>=-m_nCurEle)
	{
		// Python style, index from array tail
		return mar_Ele[m_nCurEle+pos];
	}
	else
	{
		// User can determine whether the element returned is valid
		// by checking whether its address is NULL.
		return *(T*)0; 
	}
}


template<typename T>
void 
TScalableArray<T>::ShiftDownEles(int pos, int n)
{
	memmove(mar_Ele+pos+n, mar_Ele+pos, (m_nCurEle-pos-n)*sizeof(T));

	// Call T's ctor for those inserted n elements
	for(int i=0; i<n; i++)
		new(&mar_Ele[pos+i]) T;
}


template<typename T>
void 
TScalableArray<T>::ShiftUpEles(int pos, int n)
{
	// Call T's dtor for those n elements
	for(int i=0; i<n; i++)
		mar_Ele[pos+i].~T();

	memmove(mar_Ele+pos, mar_Ele+pos+n, (m_nCurEle-pos-n)*sizeof(T));
}

template<typename T>
void 
TScalableArray<T>::CopyInEles(int pos, int n, const T* pIn)
{
	memcpy(mar_Ele+pos, pIn, n*sizeof(T));
}

template<typename T>
void 
TScalableArray<T>::CopyOutEles(int pos, int n, T* pOut) const
{
	memcpy(pOut, mar_Ele+pos, n*sizeof(T));
}


template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::InsertBefore(int pos, const T array[], int n)
{
	if(array==NULL || n==0) 
		return E_Success;

	if(n<0 || pos<0 || pos>m_nCurEle) 
		return E_InvalidParam;

	int nNewEle = m_nCurEle+n;

	ReCode_t re = ExtendEles(nNewEle, false);
	if(re!=E_Success) 
		return re; // return the error-code

	// move the memory block
	ShiftDownEles(pos, n);

	// copy the incoming eles
	if(array)
	{
		CopyInEles(pos, n, array); 
	
		// [2026-04-14] If user wants to explicitly deal with the newly inserted memblock
		// (e.g. construct new C++ object within etc), he can pass array==NULL, so that
		// the newly inserted memblock is considered uninitialized data.
	}

	return E_Success;
}


template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::DeleteEles(int pos, int n)
{
	if(n<0 || pos<0 || pos>m_nCurEle) 
		return E_InvalidParam;

	if(n==0 || pos==m_nCurEle) 
		return E_Success;

	int nEleToDelete = _MIN_(n, m_nCurEle-pos);	

	ShiftUpEles(pos, nEleToDelete);

	m_nCurEle -= nEleToDelete;

	if(m_nDecSize==0)
		return E_Success;

	// We cannot use this "optimize", bcz m_nCurStorage may not be multiples of m_nCurEle.
// 	if(m_nCurEle > m_nCurStorage-m_nDecSize-m_nDecThres)
// 	{	// the optimize: Check this to avoid doing OCC_DIVIDE every time.
// 		return E_Success;
// 	}

	int occn_orig = OCC_DIVIDE(m_nCurStorage, m_nDecSize);
	int occn_new = OCC_DIVIDE(m_nCurEle+m_nDecThres, m_nDecSize); // [2026-05-01] suspicious!

	assert(occn_new<=occn_orig);

	if(occn_new==0)
	{
		assert(occn_orig!=0);
		assert(m_nCurEle==0 && m_nDecThres==0);

		DeleteAllStorage();
	}
	else if(occn_new<occn_orig)
	{
		int nNewStorage = occn_new * m_nDecSize;

		void *pNew = Realloc(mar_Ele, nNewStorage*sizeof(T)); // shrink storage
		if(pNew) 
		{
			mar_Ele = (T*)pNew;
			m_reallocs++;
		}
		else
		{
			// If fail(rarely possible on shrink), do nothing, just use original storage.
		}
		m_nCurStorage = nNewStorage;
	}

	return E_Success;
}

template<typename T>
void 
TScalableArray<T>::DeleteAllStorage()
{
	if(mar_Ele)
	{
		// Destruct all C++ elements
		for(int i=0; i<m_nCurEle; i++)
		{
			mar_Ele[i].~T();
		}

		Free(mar_Ele);
		mar_Ele = NULL;
		m_nCurEle = m_nCurStorage = 0;
		m_reallocs++;
	}
	else
	{
		assert(m_nCurStorage==0 && m_nCurEle==0);
	}
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::ExtendEles(int nTotalEles, bool isCtorNow)
{
	if(nTotalEles<=m_nCurEle) 
		return E_Success; //Do nothing

	if(nTotalEles > m_nMaxEle) 
		return E_Full;

	assert(m_nCurStorage%m_nIncSize==0); //`m_nCurStorage' should be multiple of m_nIncSize

	int nOldEle = m_nCurEle;
	int nOldStorage = m_nCurStorage;

	int nNewStorage = UP_ROUND(nTotalEles, m_nIncSize);

	if(nNewStorage>m_nMaxEle)
		nNewStorage = m_nMaxEle;

	void *pNew = NULL;

	if(nNewStorage>m_nCurStorage)
	{
		pNew = Realloc(mar_Ele, nNewStorage*sizeof(T));
		if(!pNew) 
			return E_NoMem;

		m_reallocs++;

		// Update the related members
		mar_Ele = (T*)pNew;
		m_nCurStorage = nNewStorage;
	}

	m_nCurEle = nTotalEles;

	ZeroEles(nOldStorage, nNewStorage - nOldStorage);

	if(m_nCurEle>nOldEle)
	{
		if(isCtorNow)
		{
			// Call T's ctor for new elements
			for(int i=nOldEle; i<m_nCurEle; i++)
				new(&mar_Ele[i]) T;
		}
		else
		{	// Zero-out the new elements' storage
			ZeroEles(nOldEle, m_nCurEle-nOldEle);
		}
	}


	return E_Success;
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::SetEleQuan(int nNewEle, bool isZeroContent)
{
	if(nNewEle<0) 
		return E_InvalidParam;
	if(nNewEle>m_nMaxEle)
		return E_Full;

	if(nNewEle==m_nCurEle) 
	{
		return E_Success; // Do nothing
	}
	else if(nNewEle<m_nCurEle) 
	{
		return DeleteEles(nNewEle, m_nCurEle-nNewEle);
	}
	else 
	{
		int nOrigEle = m_nCurEle;
		ReCode_t re = ExtendEles(nNewEle, isZeroContent ? false : true);
		return re;
	}
}


template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::AppendAt(int pos, const T* array, int n)
{
	if(pos<0 || pos>m_nCurEle)
		return E_InvalidParam;

	if(pos==m_nCurEle)
	{
		return AppendTail(array, n);
	}
	else if(pos+n<=m_nCurEle)
	{
		CopyInEles(pos, n, array);
		return E_Success;
	}
	else if(pos+n>m_nMaxEle)
	{
		return E_Full;
	}
	else
	{
		int nOverWrite = m_nCurEle-pos;
		CopyInEles(pos, nOverWrite, array);
		return AppendTail(array+nOverWrite, n-nOverWrite);
	}
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::AppendAt(int pos, const T tobj)
{
	if(pos<0 || pos>m_nCurEle)
		return E_InvalidParam;

	if(pos==m_nCurEle)
		AppendTail(tobj);
	else
		mar_Ele[pos] = tobj;

	return E_Success;
}


template<typename T>
void *
TScalableArray<T>::Realloc(void *oldbuf, size_t new_size)
{
#ifdef _XXX_SEE_REALLOC_CRASH
	static int ga_count = 0;
	ga_count++;
	printf("[%d]Realloc(oldbuf=%p, new_size: %d)\n", ga_count, oldbuf, (int)new_size);
#endif

#ifdef TScalableArray_realloc
	void *p = TScalableArray_realloc(oldbuf, new_size);
#else
	void *p = realloc(oldbuf, new_size); // Call standard CRT
#endif

#ifdef _XXX_SEE_REALLOC_CRASH
	printf("[%d]Realloc(oldbuf=%p, newbuf=%p)\n", ga_count, oldbuf, p);
#endif

	return p;
}

template<typename T>
void 
TScalableArray<T>::Free(void *ptr)
{
	if(!ptr)
		return;

#ifdef TScalableArray_free
	TScalableArray_free(ptr);
#else
	free(ptr); // Call standard CRT
#endif

}


/* Sample porting code:

#define TScalableArray_realloc ufcom_realloc
#define TScalableArray_free ufcom_free

void* ufcom_realloc(void *oldbuf, size_t new_size)
{
	...
}

void ufcom_free(void *p)
{
	...
}

*/


#endif // __ScalableArray_h

