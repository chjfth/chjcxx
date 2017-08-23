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
		E_InvalidParam = -3,
		E_Full = -4
	};
	typedef ReCode_et ReCode_t;

private:
	enum
	{
		DEF_INCSIZE = 8,
		DEF_DECSIZE = DEF_INCSIZE*2
	};

public:
	ReCode_t m_InitErr;
	TScalableArray(){ reset(); }

	TScalableArray(int &err, int MaxEle, int InitEle=0, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE);
		/*!< 
		 Note: User must set err to 0 on input.
		 Note: If InitEle is 0, the object will not cost any heap storage, so it will always succeed.
		*/
	
	~TScalableArray();

	ReCode_t Init(int MaxEle, int InitEle=0, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE)
	{
		// Note: When IncSize==0 && DecSize==0, MaxEle' storage will be allocated now, once and for all.
		return init(MaxEle, InitEle, IncSize, DecSize);
	}

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

	operator T* () { return GetElePtr(0); }
		
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
		// A too large `n' means delete all eles starting at `pos'
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

	int GetIncSize() { return m_nIncSize; }
	int GetDecSize() { return m_nDecSize; }

	ReCode_t SetNewIncDecSize(int IncSize, int DecSize);

protected:
	void reset();
		// reset the non-class-object members

	ReCode_t init(int MaxEle, int InitEle=0, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE);

	void ShiftDownEles(int pos, int n);
	void ShiftUpEles(int pos, int n);

	void CopyInEles(int pos, int n, const T* pIn);
	void CopyOutEles(int pos, int n, T* pOut) const;

	ReCode_t ExtendEles(int nTotalEles);

protected:
	// Memory allocation/free functions, like those of CRTs.
	static void *Malloc(size_t new_size);
	static void *Realloc(void *oldbuf, size_t new_size);
	static void Free(void *ptr);

protected:
	T* mar_Ele;	// pointer to the dynamically allocated array of type T
	
	int m_nMaxEle; // max elements 
	int m_nCurEle; // current elements
	int m_nCurStorage; // current storage allocated (in element)

	int m_nIncSize;
	int m_nDecSize;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


template<typename T>
void 
TScalableArray<T>::reset()
{
	m_InitErr = E_Success;

	mar_Ele = NULL;
	m_nCurEle = m_nCurStorage = 0;

	m_nMaxEle = 0x7FFFfff;

	m_nIncSize = DEF_INCSIZE; 
	m_nDecSize = DEF_DECSIZE;
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
	/* [2005-09-20] Note of using
		typename TScalableArray<T>::ReCode_t 
	instead of 
		TScalableArray<T>::ReCode_t 
	as the return value: 
		Although Visual C++ 6.0 SP5 happily accept the two forms, but gcc-3.3.x
	issue a warning on the latter one: "implicit typename is deprecated".
		Someone's words may give you a clue:
			C++ standard rule is that every identifier that involves a template 
			is interpreted as a value unless it is explicitly qualified as a typename.
	*/
TScalableArray<T>::init(int MaxEle, int InitEle, int IncSize, int DecSize)
{
	// Check error params

	if(MaxEle<0 || InitEle<0 || IncSize<0 || DecSize<0 ||
		MaxEle<InitEle)
	{
		return E_InvalidParam;
	}

	if(IncSize==0)
	{	// Allocate MaxEle storage once and for all
		m_nCurStorage = MaxEle;
	}
	else
	{
		m_nCurStorage = UP_ROUND(InitEle, IncSize);
	}

	if(m_nCurStorage) // ARM C++'s C-lib returns 0 for malloc(0), therefore...
	{
		mar_Ele = (T*)Malloc(m_nCurStorage*sizeof(T)); 
		if(!mar_Ele) 
			return E_NoMem;
		memset(mar_Ele, 0, m_nCurStorage*sizeof(T)); // clear to 0
	}

	//if(isClearToZero) memset(mar_Ele, 0, m_nCurStorage*sizeof(T));
		// If `T' is a simple type, such as int, double ... , you'd better
		//set `isClearToZero' and have this done.

	m_nMaxEle = MaxEle;
	m_nCurEle = InitEle;
	m_nIncSize = IncSize;
	m_nDecSize = DecSize;

// 	if(m_nIncSize==0)  // pre-201611 old code
// 		m_nIncSize = DEF_INCSIZE;
// 	if(m_nDecSize==0) 
// 		m_nDecSize = DEF_DECSIZE;

	return E_Success;
}

template<typename T>
TScalableArray<T>::TScalableArray(int &err, 
	int MaxEle, int InitEle, int IncSize, int DecSize)
{
	reset();
	if(err) 
		return;

	err = init(MaxEle, InitEle, IncSize, DecSize);
	return;
}

template<typename T>
TScalableArray<T>::~TScalableArray()
{
	Free(mar_Ele);
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
	if(pos<0 || pos>m_nCurEle || !mar_Ele) 
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

	if(m_nCurEle >= m_nCurStorage-m_nDecSize)
	{	//[2008-12-16] Optimize: Check this to avoid doing OCC_DIVIDE every time.
		return E_Success;
	}

	int occn_orig = OCC_DIVIDE(m_nCurStorage, m_nDecSize); //?
	int occn_new = OCC_DIVIDE(m_nCurEle+m_nIncSize, m_nDecSize);

	void *pNew = NULL;
	int nNewStorage = UP_ROUND(m_nCurEle+m_nIncSize, m_nIncSize);

	if(occn_new<occn_orig && nNewStorage<m_nCurStorage)
	{
		pNew = Realloc(mar_Ele, nNewStorage*sizeof(T)); // shrink storage
		if(pNew) 
		{
			mar_Ele = (T*)pNew;
		}
		else
		{
			//If fail(rarely possible), do nothing, just use original storage.
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
	}
	m_nCurStorage = m_nCurEle = 0;

}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::ExtendEles(int nTotalEles)
{
	if(nTotalEles<=m_nCurEle) 
		return E_Success; //Do nothing

	if(nTotalEles > m_nMaxEle) 
		return E_Full;

	if(m_nIncSize>0)
	{
		assert(m_nCurStorage%m_nIncSize==0); //`m_nCurStorage' should be multiple of m_nIncSize
		int nNewStorage = UP_ROUND(nTotalEles, m_nIncSize);

		void *pNew = NULL;

		if(nNewStorage>m_nCurStorage)
		{
			pNew = Realloc(mar_Ele, nNewStorage*sizeof(T));
			if(!pNew) 
				return E_NoMem;

			// Update the related members ...
			mar_Ele = (T*)pNew;
			m_nCurStorage = nNewStorage;
		}
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

	if(nNewEle==m_nCurEle) 
		return E_Success; // Do nothing
	else if(nNewEle<m_nCurEle) 
		return DeleteEles(nNewEle, m_nCurEle-nNewEle);
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
TScalableArray<T>::SetNewIncDecSize(int IncSize, int DecSize)
{
	if(IncSize<=0 || DecSize<=0) 
		return E_InvalidParam;

	int nNewStorage = UP_ROUND(m_nCurEle, IncSize);
	
	if(nNewStorage!=m_nCurStorage)
	{
		void *pNew = Realloc(mar_Ele, nNewStorage*sizeof(T));
		if(!pNew) 
			return E_NoMem;

		// Update related members ...
		mar_Ele = (T*)pNew;
		m_nCurStorage = nNewStorage;
	}

	m_nIncSize = IncSize;
	m_nDecSize = DecSize;
	
	return E_Success;
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
TScalableArray<T>::Malloc(size_t new_size)
{
#ifdef TScalableArray_malloc
	void *p = TScalableArray_malloc(new_size);
#else
	void *p = malloc(new_size); // Call standard CRT
#endif
	return p;
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
	return TScalableArray_free(ptr);
#else
	return free(ptr); // Call standard CRT
#endif

}


/* Sample porting code:

#define TScalableArray_malloc ufcom_malloc
#define TScalableArray_realloc ufcom_realloc
#define TScalableArray_free ufcom_free

void* ufcom_realloc(void *oldbuf, size_t new_size)
{
	...
}

void* ufcom_malloc(size_t new_size)
{
	...
}

void ufcom_free(void *p)
{
	...
}

*/


#endif // __ScalableArray_h

