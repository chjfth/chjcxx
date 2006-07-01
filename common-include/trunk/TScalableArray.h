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

template<typename T>
class TScalableArray
{
public:
	enum ReCode_t// error codes
	{
		E_Success = 0,
		E_Unknown = -1,
		E_NoMem = -2,
		E_InvalidParam = -3,
		E_Full = -4
	};

private:
	enum
	{
		DEF_INCSIZE = 8,
		DEF_DECSIZE = DEF_INCSIZE*2
	};

public:
	ReCode_t m_InitErr;
	TScalableArray(){ reset(); }

	ReCode_t init(int MaxEle, int InitEle=0, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE);
	TScalableArray(ReCode_t &r_re, int MaxEle, int InitEle=0, int IncSize=DEF_INCSIZE, int DecSize=DEF_DECSIZE);

	~TScalableArray();

	int CurrentEles() const { return m_nCurEle; }

	ReCode_t GetEles(int pos, T* pEle, int n) const;
	ReCode_t GetEle(int pos, T* pEle) const
	{
		return GetEles(pos, pEle, 1);
	}

	const T* GetElePtr(int pos);
		// Note: Adding or deleting elements may cause the returned pointer to become invalid.

	T& operator[](int pos) const ;
		
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

	ReCode_t SetEleQuan(int nNewEle, int isZeroContent);

	ReCode_t SetNewIncDecSize(int IncSize, int DecSize);

protected:
	void reset();
		// reset the non-class-object members

	void ShiftDownEles(int pos, int n);
	void ShiftUpEles(int pos, int n);

	void CopyInEles(int pos, int n, const T* pIn);
	void CopyOutEles(int pos, int n, T* pOut) const;

	ReCode_t ExtendEles(int nNewEle);
	
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
	m_nMaxEle = m_nCurEle = m_nCurStorage = 0;

	m_nIncSize = DEF_INCSIZE; m_nDecSize = m_nIncSize*DEF_DECSIZE;
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

	m_nCurStorage = UP_ROUND(InitEle, IncSize);

	if(m_nCurStorage) // ARM C++'s C-lib returns 0 for malloc(0), therefore...
	{
		mar_Ele = (T*)malloc(m_nCurStorage*sizeof(T)); 
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

	if(m_nIncSize==0) 
		m_nIncSize = DEF_INCSIZE;
	if(m_nDecSize==0) 
		m_nDecSize = DEF_DECSIZE;

	return E_Success;
}

template<typename T>
TScalableArray<T>::TScalableArray(ReCode_t &r_re, 
	int MaxEle, int InitEle, int IncSize, int DecSize)
{
	reset();
	if(r_re!=NOERROR_0) 
		return;

	r_re = init(MaxEle, InitEle, IncSize, DecSize);
	return;
}

template<typename T>
TScalableArray<T>::~TScalableArray()
{
	FREE_MALLOCED(mar_Ele);
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
const T* 
TScalableArray<T>::GetElePtr(int pos)
{
	if(pos<0 || pos>m_nCurEle) 
		return NULL;
	
	return mar_Ele+pos;
}

template<typename T>
T& 
TScalableArray<T>::operator[](int pos) const 
{
	if(pos<0 || pos>=m_nCurEle) 
	{
		//return s_InvlEle; // return the static invalid element
		return *(T*)0; // User can determine whether the element returned is valid
			// by checking whether its address is NULL.
	}
	else
	{
		return mar_Ele[pos];
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
	
	int occn_orig = OCC_DIVIDE(m_nCurStorage, m_nDecSize);
	int occn_new = OCC_DIVIDE(m_nCurEle+m_nIncSize, m_nDecSize);

	void *pNew = NULL;
	int nNewStorage = UP_ROUND(m_nCurEle+m_nIncSize, m_nIncSize);

	if(occn_new<occn_orig && nNewStorage<m_nCurStorage)
	{
		pNew = realloc(mar_Ele, nNewStorage*sizeof(T));
		if(pNew) // It could rarely happen.
		{
			mar_Ele = (T*)pNew;
		}
		else{} //If fail(rarely possible), do nothing.
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
		free(mar_Ele);
		mar_Ele = NULL;
	}
	m_nCurStorage = m_nCurEle = 0;

}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::ExtendEles(int nNewEle)
{
	if(nNewEle<=m_nCurEle) 
		return E_Success; //Do nothing

	if(nNewEle > m_nMaxEle) 
		return E_Full;

	assert(m_nCurStorage%m_nIncSize==0); //`m_nCurStorage' should be multiple of m_nIncSize
	int nNewStorage = UP_ROUND(nNewEle, m_nIncSize);

	void *pNew = NULL;

	if(nNewStorage>m_nCurStorage)
	{
		pNew = realloc(mar_Ele, nNewStorage*sizeof(T));
		if(!pNew) 
			return E_NoMem;

		// Update the related members ...
		mar_Ele = (T*)pNew;
		m_nCurStorage = nNewStorage;
	}

	m_nCurEle = nNewEle;

	return E_Success;
}

template<typename T>
typename TScalableArray<T>::ReCode_t 
TScalableArray<T>::SetEleQuan(int nNewEle, int isZeroContent)
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
	if(IncSize<0 || DecSize<0) 
		return E_InvalidParam;

	int nNewStorage = UP_ROUND(m_nCurEle, IncSize);
	
	if(nNewStorage!=m_nCurStorage)
	{
		void *pNew = realloc(mar_Ele, nNewStorage*sizeof(T));
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


#endif // __ScalableArray_h

