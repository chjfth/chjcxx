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

*/
template<typename T>
inline T*
Tscawlb_PickOptBuf(T *LocalBuf, int LocalBufsize, TScalableArray<T> &sca, int *pRetSize)
{
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

