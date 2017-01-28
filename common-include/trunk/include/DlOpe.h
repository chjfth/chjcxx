#ifndef __DlOpe_h_20071014_h_
#define __DlOpe_h_20071014_h_

// Usage: http://wiki.dev.nls:8080/pages/viewpage.action?pageId=9371663


#include <stddef.h>
	//[2009-11-30] WinCE SDK in VS2005 needs <crtdef.h> for ptrdiff_t .
	// crtdef.h is not ANSI standard, so we use <stddef.h> .

#include <assert.h>

template<typename Tnode, 
	Tnode* Tnode::*ofsHead,
	Tnode* Tnode::*ofsPrev, 
	Tnode* Tnode::*ofsNext>
class DlOpeH // H implies each node has a pointer to the head-node.
{
public:
	static Tnode* 
	InitHead(
		Tnode* &r_pHeadNode, // [out] true location
		Tnode* &r_pPrevNode, // [out] true location
		Tnode* &r_pNextNode  // [out] true location
		)
	{
		ptrdiff_t head_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsHead );
		ptrdiff_t prev_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsPrev );
		ptrdiff_t next_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsNext );

		Tnode *h1 = (Tnode*)((char*)&r_pHeadNode - head_offset);
		Tnode *h2 = (Tnode*)((char*)&r_pPrevNode - prev_offset);
		Tnode *h3 = (Tnode*)((char*)&r_pNextNode - next_offset);

		if(! (h1==h2 && h1==h3) )
		{
			// This mean that r_pHeadNode,r_pPrevNode,r_pNextNode's layout
			// does not match that in real Tnode.
			assert(0);
			while(1); // make it dead
		}

		// OK now
		r_pHeadNode = r_pPrevNode = r_pNextNode = h1;

		return r_pHeadNode; // return the virtual head node address
	}

	static Tnode* 
	InitNode(Tnode *pNode)
	{
		pNode->*ofsHead = pNode->*ofsPrev = pNode->*ofsNext = pNode;
		return pNode;
	}

	static void 
	NodeInsert(Tnode *pNodeToInsert, Tnode *pFrontNode, Tnode *pRearNode)
	{
		pRearNode->*ofsPrev = pNodeToInsert;
		pNodeToInsert->*ofsNext = pRearNode;

		pFrontNode->*ofsNext = pNodeToInsert;
		pNodeToInsert->*ofsPrev = pFrontNode;

		pNodeToInsert->*ofsHead = pFrontNode->*ofsHead;
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
	NodeDel_(Tnode* pFrontNode, Tnode* pRearNode)
	{
		pRearNode->*ofsPrev = pFrontNode;
		pFrontNode->*ofsNext = pRearNode;
	}

	static Tnode* 
	nDel(Tnode* pNodeToDel)
	{
		NodeDel_(pNodeToDel->*ofsPrev, pNodeToDel->*ofsNext);
		pNodeToDel->*ofsHead = pNodeToDel->*ofsPrev = pNodeToDel->*ofsNext = pNodeToDel;
		return pNodeToDel;
	}

	static int 
	nCount(Tnode* pNode)
	{
		Tnode *phead = pNode->*ofsHead;
		int n=0;
		for(Tnode *pnext=phead->*ofsNext; pnext!=phead; pnext=pnext->*ofsNext)
			n++;
		return n;
	}

};

// Old Dlope from 2007: 

template<typename Tnode, Tnode* Tnode::*ofsPrev, Tnode* Tnode::*ofsNext>
class DlOpe
{
public:
	static Tnode* 
	InitHead(Tnode* &r_pVHeadNode, Tnode* &r_pPrevNode, Tnode* &r_pNextNode)
	{
		ptrdiff_t prev_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsPrev );
		r_pVHeadNode = (Tnode*)((char*)&r_pPrevNode - prev_offset);

		ptrdiff_t next_offset = (ptrdiff_t) &( ((Tnode*)0)->*ofsNext );
		Tnode *VerifyHeadNode = (Tnode*)((char*)&r_pNextNode - next_offset);

		assert(r_pVHeadNode==VerifyHeadNode);
		//
		// If the input &r_pPrevNode and &r_pNextNode have inconsistent relative memory location,
		// this will assert fail.
		// For example:
		// If your node is defined as: 
		//	struct SNode { int num; SNode *pprev, *pnext; };
		// but your virtual head node is:
		//	struct SHead { SNode *pPrev, *bad_stuff_here, *pNext; SNode *pVirtualHead; };
		// then,
		//	DlOpe<SNode, &SNode::pprev, &SNode::pnext>::InitHead(head.pVirtualHead, head.pPrev, head.pNext);
		// will assert fail.

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
	NodeDel_(Tnode* pFrontNode, Tnode* pRearNode)
	{
		pRearNode->*ofsPrev = pFrontNode;
		pFrontNode->*ofsNext = pRearNode;
	}

	static Tnode* 
	nDel(Tnode* pNodeToDel)
	{
		NodeDel_(pNodeToDel->*ofsPrev, pNodeToDel->*ofsNext);
		pNodeToDel->*ofsPrev = pNodeToDel->*ofsNext = pNodeToDel;
		return pNodeToDel;
	}

	static int 
	nCount(Tnode* pNodeIn)
	{
		int n=0;
		for(Tnode *pnext=pNodeIn->*ofsNext; pnext!=pNodeIn; pnext=pnext->*ofsNext)
			n++;
		return n;
	}
};


/*
//////////////////////////////////////////////////////////////////////////
Work around for Dlope:
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

/*
// DlopeTest.cpp

#include <stdio.h>
#include <stdlib.h>
#include "DlOpe.h"

struct SNode
{
	int num;
	SNode *phead, *pprev, *pnext;

	SNode();
};

struct SHead
{
	SNode *pHead, *pPrev, *pNext;
};

//#define USE_OLD_2007 // test choice

#ifdef USE_OLD_2007
typedef DlOpe<SNode, &SNode::pprev, &SNode::pnext> DLOPE_SNode; // old 2007
#else
typedef DlOpeH<SNode, &SNode::phead, &SNode::pprev, &SNode::pnext> DLOPE_SNode;
#endif

SNode::SNode() : num(0)
{
	DLOPE_SNode::InitNode(this);
}

int main(int argc, char* argv[])
{
	printf("DlOpe demo.\n");
	printf("Input some numbers, separated by return. An empty input line concludes your input.\n");

	SHead vhead; // Use "head" to represent a doubly-linked list.
#ifdef USE_OLD_2007
	DLOPE_SNode::InitHead(vhead.pHead, vhead.pPrev);
#else
	DLOPE_SNode::InitHead(vhead.pHead, vhead.pPrev, vhead.pNext);
#endif
	for(;;)
	{
		char tbuf[80];
		fgets(tbuf, sizeof(tbuf), stdin);
		if(tbuf[0]=='\r' || tbuf[0]=='\n')
			break;

		SNode *pNewNode = new SNode;
		pNewNode->num = atoi(tbuf);
		DLOPE_SNode::nInsBefore(pNewNode, vhead.pHead);
		// Insert before the head means appending to tail of the list.
	}

	// print all inserted nodes
	int count = DLOPE_SNode::nCount(vhead.pHead);
	printf("Node count(not including vhead)=%d\n", count);

	SNode *pCurNode = vhead.pHead->pnext;
	int i;
	for(i=0; pCurNode!=vhead.pHead; pCurNode=pCurNode->pnext, i++)
	{
		printf("[%d]%d <node-addr:%p>\n", i, pCurNode->num, pCurNode);
	}

	// delete all node objects

	while( vhead.pHead->pnext!=vhead.pHead )
	{
		delete DLOPE_SNode::nDel(vhead.pHead->pnext);
	}

	return 0;
}
*/
