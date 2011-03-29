#ifndef __ListnodeOpEase_h_20060410_
#define __ListnodeOpEase_h_20060410_

#include <stddef.h> // for definition of NULL
#include <assert.h>


class LnOpe // LnOpe means List-node Operation easier
{
public:

//////////////////////////////////////////////////////////////////////////
// Singly-linked list >>>>
//////////////////////////////////////////////////////////////////////////

	template<typename T>
	static T* slnInit(T *pNode, T* T::*ofsNext)
	{
		pNode->*ofsNext = NULL;

		return pNode;
	}

	template<typename T>
	static T* slnInsertHead(T *pNewHeadNode, T *pOldHeadNode, T* T::*ofsNext)
	{
		if(pNewHeadNode==NULL) 
			return NULL;

		pNewHeadNode->*ofsNext = pOldHeadNode;

		return pNewHeadNode;
	}

	template<typename T>
	static T* slnRemoveHead(T *pOldHeadNode, T* T::*ofsNext)
	{
		if(pOldHeadNode==NULL) 
			return NULL;

		pOldHeadNode->*ofsNext = pOldHeadNode->*ofsNext->*ofsNext;

		return pOldHeadNode->*ofsNext; //return the new head-node
	}

	template<typename T>
	static void slnInsertAfter(T *pLinkedNode, T *pNodeToInsert, T* T::*ofsNext)
	{
		if(pLinkedNode)
		{
			pNodeToInsert->*ofsNext = pLinkedNode->*ofsNext;
			pLinkedNode->*ofsNext= pNodeToInsert;
		}
		else
		{
			pNodeToInsert->*ofsNext = NULL;
		}
	}

	template<typename T>
	static T* slnDeleteRear(T* pFrontNode, T* T::*ofsNext)
	{
		// Return the node unlinked from the list

		if(pFrontNode==NULL || pFrontNode->*ofsNext==NULL) 
			return NULL;

		pFrontNode->*ofsNext->*ofsNext = NULL;

		return pFrontNode->*ofsNext;
	}

//////////////////////////////////////////////////////////////////////////
// Doubly-linked list >>>>
//////////////////////////////////////////////////////////////////////////

	template<typename T>
	static T* dlnInit(T *pNode, T* T::*ofsPrev, T* T::*ofsNext)
	{
		pNode->*ofsPrev = pNode->*ofsNext = NULL;

		return pNode;
	}

	template<typename T>
	static void dlnInsert(T *pFrontNode, T *pNodeToInsert, T *pRearNode, 
		T* T::*ofsPrev, T* T::*ofsNext)
	{
		pNodeToInsert->*ofsPrev = pFrontNode;
		pNodeToInsert->*ofsNext = pRearNode;
		
		if(pFrontNode) 
			pFrontNode->*ofsNext = pNodeToInsert;
		
		if(pRearNode) 
			pRearNode->*ofsPrev = pNodeToInsert;
	}

	template<typename T>
	static T* dlnInsertBefore(T* pNodeToInsert, T* pRearNode,
		T* T::* ofsPrev, T* T::* ofsNext)
	{
		dlnInsert(pRearNode ? pRearNode->*ofsPrev : NULL, pNodeToInsert, pRearNode,
			ofsPrev, ofsNext);
		return pNodeToInsert; // return the newly inserted node
	}

	template<typename T>
	static T* dlnInsertAfter(T *pFrontNode, T* pNodeToInsert, 
		T* T::*ofsPrev, T* T::*ofsNext)
	{
		dlnInsert(pFrontNode, pNodeToInsert, pFrontNode ? pFrontNode->*ofsNext : NULL,
			ofsPrev, ofsNext);
		return pNodeToInsert; // return the newly inserted node
	}

	template<typename T>
	static T* dlnUnlink(T *pNodeToUnlink, T* T::*ofsPrev, T* T::*ofsNext)
	{
		T* pNodeFront = pNodeToUnlink->*ofsPrev;
		T* pNodeRear = pNodeToUnlink->*ofsNext;
		
		if(pNodeFront) 
			pNodeFront->*ofsNext = pNodeToUnlink->*ofsNext;
		if(pNodeRear) 
			pNodeRear->*ofsPrev = pNodeToUnlink->*ofsPrev;
		
		dlnInit(pNodeToUnlink, ofsPrev, ofsNext);

		return pNodeToUnlink;
	}

//////////////////////////////////////////////////////////////////////////
// Doubly-cyclic-linked list >>>>
//////////////////////////////////////////////////////////////////////////

	template<typename T>
	static T* dclnInit(T *pNode, T* T::*ofsPrev, T* T::*ofsNext)
	{
		pNode->*ofsPrev = pNode->*ofsNext = pNode; // both points to himself

		return pNode;
	}

	template<typename T>
	static void dclnInsert(T *pFrontNode, T *pNodeToInsert, T *pRearNode, 
		T* T::*ofsPrev, T* T::*ofsNext)
	{
		assert(pFrontNode && pNodeToInsert && pRearNode);

		pNodeToInsert->*ofsPrev = pFrontNode;
		pNodeToInsert->*ofsNext = pRearNode;
		
		pFrontNode->*ofsNext = pNodeToInsert;
		pRearNode->*ofsPrev = pNodeToInsert;
	}

	template<typename T>
	static T* dclnInsertBefore(T* pNodeToInsert, T* pRearNode,
		T* T::* ofsPrev, T* T::* ofsNext)
	{
		assert(pNodeToInsert && pRearNode);
		dclnInsert(pRearNode->*ofsPrev, pNodeToInsert, pRearNode,
			ofsPrev, ofsNext);
		return pNodeToInsert; // return the newly inserted node
	}

	template<typename T>
	static T* dclnInsertAfter(T *pFrontNode, T* pNodeToInsert, 
		T* T::*ofsPrev, T* T::*ofsNext)
	{
		assert(pFrontNode && pNodeToInsert);
		dclnInsert(pFrontNode, pNodeToInsert, pFrontNode->*ofsNext,
			ofsPrev, ofsNext);
		return pNodeToInsert; // return the newly inserted node
	}

	template<typename T>
	static T* dclnUnlink(T *pNodeToUnlink, T* T::*ofsPrev, T* T::*ofsNext)
	{
		assert(pNodeToUnlink);

		T* pNodeFront = pNodeToUnlink->*ofsPrev;
		T* pNodeRear = pNodeToUnlink->*ofsNext;
		
		pNodeFront->*ofsNext = pNodeToUnlink->*ofsNext;
		pNodeRear->*ofsPrev = pNodeToUnlink->*ofsPrev;
		
		dclnInit(pNodeToUnlink, ofsPrev, ofsNext);
		
		return pNodeToUnlink;
	}

};

#endif // One-time inclusion test
