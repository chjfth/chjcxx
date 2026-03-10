#ifndef __Cxx_delete_this_helper_h_20260310_
#define __Cxx_delete_this_helper_h_20260310_

class Cxx_delete_this_base
{
public:
	Cxx_delete_this_base()
	{
		m_recurse_count = 0;
		m_mark_deletes = 0;
	}

	virtual ~Cxx_delete_this_base() 
	{
	}

private:
	int m_recurse_count;
	int m_mark_deletes;

	friend class Cxx_delete_this_helper;
};


class Cxx_delete_this_helper
{
public:
	Cxx_delete_this_helper(Cxx_delete_this_base *pCxxBase)
	{
		m_pbase = pCxxBase;
		m_pbase->m_recurse_count++;
	}

	~Cxx_delete_this_helper()
	{
		m_pbase->m_recurse_count--;

		if(m_pbase->m_recurse_count==0 && m_pbase->m_mark_deletes>0)
		{
			delete m_pbase;
		}
	}

	int MarkDelete()
	{
		m_pbase->m_mark_deletes++;
		return m_pbase->m_mark_deletes;
	}

private:
	Cxx_delete_this_base *m_pbase;
};

#endif
