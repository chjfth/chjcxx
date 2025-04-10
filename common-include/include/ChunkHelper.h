#ifndef __ChunkHelper_h_20250410_
#define __ChunkHelper_h_20250410_

#include <assert.h>

class CChunkHelper
{
public:
	CChunkHelper(int totalsize, int chunksize) :
		m_totalsize(totalsize), m_chunksize(chunksize), m_remain(totalsize)
	{}

	int next() 
	{
		// pick min(chuck, remain)
		return m_chunksize < m_remain ? m_chunksize : m_remain;
	}
	
	int drop(int dropsize) 
	{
		assert(dropsize >= 0);
		assert(dropsize <= m_remain);

		m_remain -= dropsize;
		return m_remain;
	}


private:
	int m_totalsize;
	int m_chunksize;
	int m_remain;
};


#endif
