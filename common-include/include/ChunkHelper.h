#ifndef __ChunkHelper_h_20250411_
#define __ChunkHelper_h_20250411_

#include <assert.h>

template<typename T>
class TChunkHelper
{
public:
	TChunkHelper(T totalsize, T chunksize)
	{
		reset(totalsize, chunksize);
	}

	void reset(T totalsize, T chunksize)
	{
		m_totalsize = totalsize;
		m_chunksize = chunksize;
		m_offset = 0;  m_remain = totalsize;
	}

	T next() const
	{
		// pick min(chuck, remain)
		return m_chunksize < m_remain ? m_chunksize : m_remain;
	}
	
	T drop(T dropsize) 
	{
		assert(dropsize >= 0);
		assert(dropsize <= m_remain);

		m_offset += dropsize;
		m_remain -= dropsize;
		return m_remain;
	}

	T offset()
	{
		return m_offset;
	}


private:
	T m_totalsize;
	T m_chunksize;
	T m_remain;
	T m_offset;
};


// Helper function to deduce template parameters
template<typename T>
TChunkHelper<T> makeChunkHelper(T totalsize, T chunksize)
{
	return TChunkHelper<T>(totalsize, chunksize);
}

#endif
