#ifndef __TScalableArray_h
#define __TScalableArray_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commdefs.h>
#include <_MINMAX_.h>

//#ifndef OLDC_NO_NAMESPACE_SUPPORT
//using namespace NsNeCode;
//#endif

// [2005-10-08] The name TScalableArray starts with a 'T', which indicates it's a template class.

// [2008-08-26] NOTE: This template class is not aware of class ctor, dtor or copy-ctor.
// If using this template class to hold objects who needs special destruction process,
// users have to do it themselves!



template<typename T>
class TScalableArray
{
public:
	enum ReCode_et // error codes
	{
		E_Success = 0,
		E_Unknown = -1,
		E_NoMem = -2,
		// E_InvalidParam = -3,
		E_Full = -4,

		// All below means invalid-params
		E_InvalidParam = -10,
		E_IncSizeMustBeAtleast1 = -11,
		E_DecSizeShouldBeMultipleOfIncSize = -12,
		E_MaxEleLessthanCurEle = -13
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
	TScalableArray(){ _init0(); }
	TScalableArray(int MaxEle, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE,
		int DecThres=DEF_DECTHRES);
	
	~TScalableArray(){ DeleteAllStorage(); }

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

	T& operator[](int pos) const ;

	operator T* ()
	{
		if(m_nCurEle>0)
			return mar_Ele;
		else
			return NULL;
	}

	operator bool() { return (T*)(*this) ? true : false; }

	bool operator !() { return (T*)(*this) ? false : true; } // check for empty array
		
	ReCode_t InsertBefore(int pos, const T* array, int n);
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

	void DeleteAllStorage();

	void ZeroEles(int pos, int n)
	{
		pos = _MID_(0, pos, CurrentEles());
		n = _MID_(0, n, CurrentEles()-pos);
		if(n>0) memset(mar_Ele+pos, 0, n*sizeof(T));
	}

	ReCode_t SetEleQuan(int nNewEle, bool isZeroContent=false);
		//!< Change the array size to accommodate at least nNewEle elements.
		/*!< After calling SetEleQuan(), original elements in the array will not be changed.

		@param[in] isZeroContent
			If array size is extended, whether clear new array element to zero.
		*/

	void Cleanup()
	{
		if(m_nCurEle==0 && mar_Ele)
			DeleteAllStorage();
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

protected:
	void _init0();
		// reset the non-class-object members

	void ShiftDownEles(int pos, int n);
	void ShiftUpEles(int pos, int n);

	void CopyInEles(int pos, int n, const T* pIn);
	void CopyOutEles(int pos, int n, T* pOut) const;

	ReCode_t ExtendEles(int nTotalEles);

protected:
	// Memory allocation/free functions, like those of CRTs.
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
};

template<typename T>
bool IsTsaErr(T err)
{ 
	return err ? true : false; 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


template<typename T>
void 
TScalableArray<T>::_init0()
{
	mar_Ele = NULL;
	m_nCurEle = m_nCurStorage = 0;

	m_nMaxEle = 0x7FFFffff;

	m_nIncSize = DEF_INCSIZE; 
	m_nDecSize = DEF_DECSIZE;
	m_nDecThres = DEF_DECTHRES;
	m_reallocs = 0;
}

template<typename T>
TScalableArray<T>::TScalableArray(int MaxEle, int IncSize, int DecSize, int DecThres)
{
	_init0();

	ReCode_t err = SetTrait(MaxEle, IncSize, DecSize, DecThres);
	if(err) {
		assert(err<=E_InvalidParam);
		_init0();
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

	if(IncSize<=0)
		return E_IncSizeMustBeAtleast1;

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
TScalableArray<T>::operator[](int pos) const 
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
}


template<typename T>
void 
TScalableArray<T>::ShiftUpEles(int pos, int n)
{
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
TScalableArray<T>::InsertBefore(int pos, const T* array, int n)
{
	if(array==NULL || n==0) 
		return E_Success;

	if(n<0 || pos<0 || pos>m_nCurEle) 
		return E_InvalidParam;

	int nNewEle = m_nCurEle+n;

	ReCode_t re = ExtendEles(nNewEle);
	if(re!=E_Success) 
		return re; // return the error-code

	// move the memory block
	ShiftDownEles(pos, n);

	// copy the incoming eles
	CopyInEles(pos, n, array);

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
	int occn_new = OCC_DIVIDE(m_nCurEle+m_nDecThres, m_nDecSize);

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
TScalableArray<T>::ExtendEles(int nTotalEles)
{
	if(nTotalEles<=m_nCurEle) 
		return E_Success; //Do nothing

	if(nTotalEles > m_nMaxEle) 
		return E_Full;

	assert(m_nCurStorage%m_nIncSize==0); //`m_nCurStorage' should be multiple of m_nIncSize

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
		ReCode_t re = ExtendEles(nNewEle);
		if(re!=E_Success) 
			return re; //return the error code

		if(isZeroContent) 
			ZeroEles(nOrigEle, nNewEle-nOrigEle);

		return E_Success;
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
#ifdef TScalableArray_realloc
	void *p = TScalableArray_realloc(oldbuf, new_size);
#else
	void *p = realloc(oldbuf, new_size); // Call standard CRT
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

