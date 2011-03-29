#ifndef __DlOpe_h_20071014_h_
#define __DlOpe_h_20071014_h_

#include <stddef.h>
	//[2009-11-30] WinCE SDK in VS2005 needs <crtdef.h> for ptrdiff_t .
	// crtdef.h is not ANSI standard, so we use <stddef.h> .

#include <assert.h>

template<typename Tnode, Tnode* Tnode::*ofsPrev, Tnode* Tnode::*ofsNext>
class DlOpe
{
public:
	static Tnode* 
	InitHead(Tnode* &r_pVHeadNode, Tnode* &r_pPrevNode)
	{
		ptrdiff_t prev_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsPrev );
		r_pVHeadNode = (Tnode*)((char*)&r_pPrevNode - prev_offset);

		r_pVHeadNode->*ofsPrev = r_pVHeadNode->*ofsNext = r_pVHeadNode;
		
		return r_pVHeadNode; // return the virtual head node address
	}

	static Tnode* 
	InitNode(Tnode *pNode)
	{
		pNode->*ofsPrev = pNode->*ofsNext = pNode;
		return pNode;
	}

	static void 
	NodeInsert(Tnode *pNodeToInsert, Tnode *pFrontNode, Tnode *pRearNode)
	{
		pRearNode->*ofsPrev = pNodeToInsert;
		pNodeToInsert->*ofsNext = pRearNode;

		pFrontNode->*ofsNext = pNodeToInsert;
		pNodeToInsert->*ofsPrev = pFrontNode;
	}

	static Tnode* 
	nInsBefore(Tnode* pNodeToIns, Tnode* pRearNode)
	{
		NodeInsert(pNodeToIns, pRearNode->*ofsPrev, pRearNode);
		return pNodeToIns;
	}

	static Tnode* 
	nInsAfter(Tnode* pNodeToIns, Tnode* pFrontNode)
	{
		NodeInsert(pNodeToIns, pFrontNode, pFrontNode->*ofsNext);
		return pNodeToIns;
	}

	static void 
	NodeDel(Tnode* pFrontNode, Tnode* pRearNode)
	{
		pRearNode->*ofsPrev = pFrontNode;
		pFrontNode->*ofsNext = pRearNode;
	}

	static Tnode* 
	nDel(Tnode* pNodeToDel)
	{
		NodeDel(pNodeToDel->*ofsPrev, pNodeToDel->*ofsNext);
		pNodeToDel->*ofsPrev = pNodeToDel->*ofsNext = pNodeToDel;
		return pNodeToDel;
	}


};

/*
//////////////////////////////////////////////////////////////////////////
Work around:
If your compiler is too old to not be able to support the DlOpe template class,
you can use the following DlOpe_XXX functions instead. Functions in DlOpe class
and DlOpe_XXX functions have the same binary interface, but the latter has
a more verbose interface.

  Old Compilers include: Visual C++ 6 SP6 
//////////////////////////////////////////////////////////////////////////
*/

template<typename T> // T is a node-type
inline T* 
DlOpe_InitHead(T *&r_pVHeadNode, T *&r_pPrevNode, T* T::*ofsPrev, T* T::*ofsNext)
{
//	T* px = ((T*)0)->*ofsPrev;
	ptrdiff_t prev_offset = (ptrdiff_t) &( ((T*)0)->*ofsPrev );
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
	
	pNodeToDel->*ofsNext = pNodeToDel->*ofsPrev = pNodeToDel;

	return pNodeToDel;
}

template<typename T> 
inline bool 
DlOpe_nFind(T* pNodeToFind, T* pNodeExist, T* T::*ofsNext)
{
	T *pcur = pNodeExist;
	for(;;)
	{
		if(pcur==pNodeToFind)
			return true; // found

		pcur = pcur->*ofsNext;
		if(pcur==pNodeExist)
			return false;
	}
}


#endif
