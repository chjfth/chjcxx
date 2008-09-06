#ifndef __TScaWithLocalBuf_h_20080827_
#define __TScaWithLocalBuf_h_20080827_

#include <TScalableArray.h>

//! Pick an optimized buffer between a local buffer or an TScalableArray maintained buffer.
/*! 

 @param[in] LocalBuf
	Local buffer is usually a buffer(array) allocated on a function's stack.

 @param[in] LocalBufsize
	Size of the local buffer, in T elements.

 @sca[in]
	An TScalableArray object who can provide scalable buffer.

 @pRetSize[out]
	This output tells the size of buffer pointed to by the returned pointer.

 @return
  This function compares two buffers provided by LocalBuf and sca, returning the
  larger one's pointer to the caller. 

  NOTE: "Buffer size" are all described in T elements.

*/
template<typename T>
inline T*
Tscawlb_PickOptBuf(T *LocalBuf, int LocalBufsize, TScalableArray<T> &sca, int *pRetSize=0)
{
	// Hmm, I hope I can declare `sca' const, since this function does not change sca at all.
	// But I can't, because GetElePtr() does not return a const pointer.

	int scaSize = sca.CurrentEles();

	if(LocalBufsize>=scaSize)
	{
		if(pRetSize)
			*pRetSize = LocalBufsize;
		return LocalBuf;
	}
	else 
	{
		if(pRetSize)
			*pRetSize = scaSize;
		return sca.GetElePtr(0);
	}
}

//! Request an optimized buffer between a local buffer and an TScalableArray maintained buffer.
/*! If the local buffer size is enough, local buffer pointer is returned, otherwise the
 TScalableArray's buffer is increased to make a buffer of requested size.

 @param[in] LocalBuf
	Local buffer is usually a buffer(array) allocated on a function's stack.

 @param[in] LocalBufsize
	Size of the local buffer, in T elements.

 @sca[in]
	An TScalableArray object who can provide scalable buffer.

 @pReize[in/out] (requested and returned size)
	On input, tells the buffer size request; on output, tells the available buffer size
	by the returned pointer. This may be larger than the requested size.
	This output tells the size of buffer pointed to by the returned pointer.

 @isShrinkSca[in]
	If requested size is larger than LocalBuf but smaller than sca, whether call 
	sca.SetEleQuan() so that sca's internal buffer may be shrinked. 


 @return
  This function compares two buffers provided by LocalBuf and sca, returning the
  larger one's pointer to the caller.

  NOTE: "Buffer size" are all described in T elements.

*/
template<typename T>
inline T*
Tscawlb_ReqOptBuf(T *LocalBuf, int LocalBufsize, TScalableArray<T> &sca, int *pReSize,
	bool isShrinkSca=false)
{
	int scaSize = sca.CurrentEles();

	if(LocalBufsize>=*pReSize)
	{
		*pReSize = LocalBufsize;
		return LocalBuf;
	}
	else if(scaSize>=*pReSize)
	{
		if(isShrinkSca)
		{
			sca.SetEleQuan(UP_ROUND(*pReSize, sca.GetIncSize()), NO);
			*pReSize = sca.CurrentEles();
		}
		else
			*pReSize = scaSize;

		return sca.GetElePtr(0);
	}
	else // must increase sca buffer
	{
		sca.SetEleQuan(UP_ROUND(*pReSize, sca.GetIncSize()), NO);
		*pReSize = sca.CurrentEles();
		return sca.GetElePtr(0);
	}
}


//! Extend or shrink a scalable-array to new size.
/*! 

  Note: This function always change size to the `IncSize' boundary, so that all 
  TScalableArray-allocated buffer can be immediately used by the user.

  @param[in] ReqSize
	Requested scalable array size in elements. This tells the minimum of user request; 
	the actual size re-allocated to sca can be larger to meet the `IncSize' boundary.

  @return
	If success, return pointer to the new array; if fail(no memory to extend the size), return NULL.
*/
template<typename T>
inline T*
Tscawlb_ReqNewSize(TScalableArray<T> &sca, int ReqSize)
{
	TScalableArray<T>::ReCode_t err = 
		sca.SetEleQuan(UP_ROUND(ReqSize, sca.GetIncSize()), NO);

	if(err)
		return NULL;
	else
		return sca;
}



#endif // __TScaWithLocalBuf_h_20080827_

