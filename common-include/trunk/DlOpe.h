#ifndef __DlOpe_h_20071014_h_
#define __DlOpe_h_20071014_h_


#include <stddef.h> // for definition of NULL
#include <assert.h>

template<typename T> // T is a node-type
inline T* 
DlOpe_InitHead(T *&r_pVHeadNode, T *&r_pPrevNode, T* T::*ofsPrev, T* T::*ofsNext)
{
//	T* px = ((T*)0)->*ofsPrev;
	int prev_offset = (int) &( ((T*)0)->*ofsPrev );
	r_pVHeadNode = (T*)((char*)&r_pPrevNode - prev_offset);

	r_pVHeadNode->*ofsPrev = r_pVHeadNode->*ofsNext = r_pVHeadNode;
	
	return r_pVHeadNode; // return the virtual head node address
}

template<typename T> 
inline T* 
DlOpe_InitNode(T *pNode, T* T::*ofsPrev, T* T::*ofsNext)
{
	pNode->*ofsPrev = pNode->*ofsNext = pNode;
	return pNode;
}

template<typename T>
inline void 
DlOpe_NodeInsert(T *pNodeToInsert, T *pFrontNode, T *pRearNode, 
	T* T::*ofsPrev, T* T::*ofsNext)
{
	pRearNode->*ofsPrev = pNodeToInsert;
	pNodeToInsert->*ofsNext = pRearNode;

	pFrontNode->*ofsNext = pNodeToInsert;
	pNodeToInsert->*ofsPrev = pFrontNode;
}

template<typename T> 
inline T* 
DlOpe_nInsBefore(T* pNodeToIns, T* pRearNode, T* T::*ofsPrev, T* T::*ofsNext)
{
	DlOpe_NodeInsert(pNodeToIns, pRearNode->*ofsPrev, pRearNode, ofsPrev, ofsNext);
	return pNodeToIns;
}

template<typename T> 
inline T* 
DlOpe_nInsAfter(T* pNodeToIns, T* pFrontNode, T* T::*ofsPrev, T* T::*ofsNext)
{
	DlOpe_NodeInsert(pNodeToIns, pFrontNode, pFrontNode->*ofsNext, ofsPrev, ofsNext);
	return pNodeToIns;
}

template<typename T> 
inline void 
DlOpe_NodeDel(T* pFrontNode, T* pRearNode, T* T::*ofsPrev, T* T::*ofsNext)
{
	pRearNode->*ofsPrev = pFrontNode;
	pFrontNode->*ofsNext = pRearNode;
}

template<typename T> 
inline T* 
DlOpe_nDel(T* pNodeToDel, T* T::*ofsPrev, T* T::*ofsNext)
{
	DlOpe_NodeDel(pNodeToDel->*ofsPrev, pNodeToDel->*ofsNext, ofsPrev, ofsNext);
	return pNodeToDel;
}


#endif
