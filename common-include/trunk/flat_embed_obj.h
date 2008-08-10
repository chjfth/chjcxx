#ifndef __flat_embed_obj_h_20080810_
#define __flat_embed_obj_h_20080810_

#include <object_align.h>

#define FLAT_EMBED_OBJ_END_MARK 0x20080810

inline void Add_FlatEmbedObj_EndMark(void *p)
{
	int *pi = (int*)p;
	if(DEFAULT_OBJECT_ALIGN_BYTES>=4)
		*pi++ = FLAT_EMBED_OBJ_END_MARK
	if(DEFAULT_OBJECT_ALIGN_BYTES>=8)
		*pi++ = FLAT_EMBED_OBJ_END_MARK+1
	if(DEFAULT_OBJECT_ALIGN_BYTES>=16)
		*pi++ = FLAT_EMBED_OBJ_END_MARK+2
}

#endif
