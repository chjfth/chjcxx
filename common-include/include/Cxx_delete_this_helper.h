#ifndef __Cxx_delete_this_helper_h_20260310_20260315_
#define __Cxx_delete_this_helper_h_20260310_20260315_

// Used in: CxxWindowSubclass

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
	Cxx_delete_this_helper(Cxx_delete_this_base *pCxxBase, bool is_inplace=false)
	{
		m_pbase = pCxxBase;
		m_pbase->m_recurse_count++;

		m_is_inplace = is_inplace;
	}

	~Cxx_delete_this_helper()
	{
		m_pbase->m_recurse_count--;

		if(m_pbase->m_recurse_count==0 && m_pbase->m_mark_deletes>0)
		{
			if(!m_is_inplace)
				delete m_pbase;
			else
				m_pbase->~Cxx_delete_this_base();
		}
	}

	int MarkDelete()
	{
		m_pbase->m_mark_deletes++;
		return m_pbase->m_mark_deletes;
	}

private:
	Cxx_delete_this_base *m_pbase;
	bool m_is_inplace;
};

#endif
