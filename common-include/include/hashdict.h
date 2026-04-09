#ifndef __CHHI__hashdict_h_20260404_20260405_
#define __CHHI__hashdict_h_20260404_20260405_

#include <new>
#include <ps_TCHAR.h>
#include <commdefs.h>
#include <sdring.h>
#include <TScalableArray.h>
#include <vaDbgTs_util.h>

// Mimic Python 3.7's compact dict

////////////////////////////////////////////////////////////////////////////
namespace chjds { 
////////////////////////////////////////////////////////////////////////////
// Place API function declarations in this namespace.

#				ifndef hashdict_DEBUG
#				include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#				endif


struct LCGen // Linear Congruential Generator
{
	Uint m_mask, m_perturb, m_perturb_shift;
	Uint m_curval;
	int m_count;
public:
	LCGen(Uint mask, Uint perturb, int perturb_shift=5)
	{
		m_mask = mask; m_perturb = perturb; m_perturb_shift = perturb_shift;
		m_curval = 0;
		m_count = 0;
	}

	Uint next()
	{
#if 1
		// This is suggested by ChatGPT.
		m_curval = (m_curval * 5 + 1 + m_perturb) & m_mask;
		m_perturb >>= m_perturb_shift;
#else
		// ChatGPT says this uses entropy(perturb) only once, not good. To Verify. 
		if(m_count==0)
			m_curval = m_perturb;
		
		m_curval = (m_curval * 5 + 1) & m_mask;
#endif
		m_count++;
		return m_curval;
	}
};


template<typename TU> // TU is user's datatype(should support delete), key is always string
class hashdict
{
public:
	hashdict(const TCHAR *dbgsig=nullptr) { _ctor(dbgsig); }

	virtual ~hashdict() { _dtor(); }

	// set(): Return the object(pointer) of the TU object managed by hashdict, different than the input pointer.
	TU* set(const TCHAR *key, TU&& value)
	{
		return SetKey(key, std::move(value), true);
	}

	TU* set(const TCHAR *key, const TU& value)
	{
		TU copy = value;
		return SetKey(key, std::move(copy), true);
	}

	TU* setdefault(const TCHAR *key, TU&& defvalue)
	{
		return SetKey(key, std::move(defvalue), false);
	}

	TU* setdefault(const TCHAR *key, const TU& defvalue)
	{
		TU copy = defvalue;
		return SetKey(key, std::move(copy), false);
	}

	TU* get(const TCHAR *key);

	int keycount() { return m_slots_active; }

	bool del(const TCHAR *key);

	bool del(const TCHAR *key, TU& recv_old_value);

	class enumer // dict-key enumerator
	{
	public:
		enumer(hashdict &dict) : m_dict(dict) 
		{
			m_next_trovent = 0;
			m_is_end = false;
		}
		~enumer()
		{ 
			reset(); 
		}

		const TCHAR *next(TU **ppValue=nullptr);
		void reset(); // after reset(), user can start a new next() round.

	private:
		hashdict &m_dict;
		int m_next_trovent;
		bool m_is_end;
	};

private:
	enum 
	{
		// Hash slots constants:
		PerturbShift = 5,
		PctFullToResize = 66,	// when slots gets 66% full, increase it size
		DummyNotSeen = -1, // must be negative

		// Trove constants:
		TroveInitSize = 16,
		TroveExtendOnceMax = 20000,
		PctDummyToCompact = 50, // when dummy trovents exceeds 50% of dirties, do compact
		OneEmptyAtTroveEnd = 1, // must
		InvalidHash64 = (__int64)(-1),

		// Misc
		ShrinkWhenLessThan = 4, // todo
	};
	
	enum SlotState_et // represent a hash slot
	{ 
		SlotEmpty=0,	// initial state, never touched yet
		SlotInUse=1,	// a key "stores" here
		SlotDummy=2,	// a key was "stored" here but now deleted
	};
	
	enum TroventState_et // represent a trove-entry in the user-value trove
	{ 
		TroventEmpty=0,	// initial. Always keep one TroventEmpty at tail as "end guard".
		TroventInUse=1, // a key's value is stored here.
		TroventDummy=2,	// a key's value was here but now delete.
	};

	struct slot_st
	{
		SlotState_et state;
		int idx_trove;  // index into trove_st array
	};

	struct trove_entry_st // user data trove
	{
		TroventState_et state;
		Uint64 hashfull;
		Sdring key;
		TU uvalue;

		trove_entry_st& operator=(trove_entry_st&& old) // move-assign
		{
			if (this != &old) 
			{
				this->state    = old.state;
				this->hashfull = old.hashfull;
				this->key      = std::move(old.key);
				this->uvalue   = std::move(old.uvalue);
			}
			return *this;
		}
	};

	typedef unsigned int Uint32;

private:
	void _ctor(const TCHAR *dbgsig);
	void _dtor();

	int RequestTroveEntry(); // Return idx of the "new" trove, may cause trove compact

	trove_entry_st* IsKeyFoundAtSlot(int idxSlot, Uint64 in_keyhash, const TCHAR *in_key);

	TU* SetKey(const TCHAR *key, TU&& value, bool is_overwrite);

	bool in_del(const TCHAR *key, bool is_recv_old, TU& recv_old_value);

	int slots_non_empty() { return m_slots_active+m_slots_dummy; }
	bool CheckToIncreaseSlots();
	void CheckToCompactTrove();

	Uint64 cal_hashfull(const TCHAR *str)
	{
		if(!(str && str[0]))
			return 0;

		Uint64 hash = 0;
		for(int i=0; str[i]; i++)
			hash = hash * 31 + str[i];

		return hash;
	}

public: // debugging purpose
	void SetDbgParams(int resize_pct) {
		m_resize_pct = resize_pct;
	}
	bool IsResizeByPct() { return m_resize_pct>0; }
	bool RebuildSlotsFromTrove(int new_dict_width);
	bool CompactTrove();

private:
	int m_dict_width;	   // start from 3, bits of current hash table
	Uint m_hashmask;       // = m_slots_total-1
	int m_slots_capacity;	// = pow(2, m_dict_width)

	int m_slots_active;		// < m_slots_total
	int m_slots_dummy;		// used then deleted slot count

	union 
	{
		int m_resize_pct;	// > 0 , normal behavior, 66 means: when 66% full, do resize
		int m_hungry_slots;	// >=0 , for debugging, resize when if empty drops to (-m_hungry_slot)
	};

	TScalableArray<slot_st> msa_slots; // the hash-slots

	TScalableArray<trove_entry_st> msa_trove;

	int m_trove_capacity;
	int m_trove_dirties;	// how many trovents touched, max=m_trove_capacity-1, can > m_slots_active.
							// CompactTrove() will shrink it(=remove dummies).
	int m_trove_dummies;

	int m_enuming_sessions;	// if >0, someone is enumerating this dict, that locks the dict.

	Sdring m_dbgsig;
	const TCHAR* dbgsig() { return m_dbgsig ? m_dbgsig.c_str() : _T(""); }
};


template<typename TU> 
void hashdict<TU>::_ctor(const TCHAR *in_dbgsig)
{
	m_dbgsig = in_dbgsig;
	vaDBG3(_T("{%s}hashdict() ctor. this@<%p>."), dbgsig(), this);

	m_dict_width = 3;
	m_slots_capacity = int_pow(2, m_dict_width);
	m_hashmask = m_slots_capacity -1 ;
	m_slots_active = 0;
	m_slots_dummy = 0;

	m_resize_pct = PctFullToResize;

	msa_slots.SetTrait(0x7FFFffff, 1, 1, 0);
	msa_slots.SetEleQuan(m_slots_capacity, true);

	msa_trove.SetTrait(0x7FFFffff, 1, 1, 0);
	m_trove_capacity = m_trove_dirties = m_trove_dummies = 0;

	m_enuming_sessions = 0;
}

template<typename TU> 
void hashdict<TU>::_dtor()
{
	vaDBG3(_T("{%s}hashdict() dtor. this@<%p>. Remnant keys: %d"), 
		dbgsig(), this,	m_slots_active);
	
	m_dbgsig = nullptr;

	if(m_trove_capacity>0)
	{
		assert(msa_trove[m_trove_capacity-1].state==TroventEmpty
			&& msa_trove[m_trove_capacity-1].hashfull==InvalidHash64);
	}

	for(int i=0; i<m_trove_dirties; i++)
	{
		trove_entry_st &trovent = msa_trove[i];
		if(trovent.state==TroventDummy)
			continue;

		assert(trovent.state==TroventInUse);

		vaDBG3(_T("  [idxTrove#%d]Freeing key \"%s\", value@<%p>"), 
			i, trovent.key.c_str(), &trovent.uvalue);

		trovent.key.~sdring();
		trovent.uvalue.~TU();
	}
}

template<typename TU> 
TU* hashdict<TU>::SetKey(const TCHAR *in_key, TU&& in_value, bool is_overwrite)
{
	if(m_enuming_sessions>0)
	{
		vaDBG1(_T("{%s}hashdict::SetKey() NOT allowed when m_enuming_sessions(%d)>0"), dbgsig(),
			m_enuming_sessions);
		return nullptr;
	}

	vaDBG3_DbgEnterExit;

	assert(in_key && in_key[0]);
	if(! (in_key && in_key[0]) )
		return nullptr;

	bool succ = CheckToIncreaseSlots(); // resize(increase) slots if needed.
	if(!succ)
	{
		vaDBG0(_T("{%s}hashdict::SetKey() failed due to no-mem!"), dbgsig());
		return nullptr;
	}

	Uint64 in_hashfull = cal_hashfull(in_key);
	Uint32 in_hashtail = (Uint32)in_hashfull & m_hashmask;

	assert((int)in_hashtail < m_slots_capacity);

	vaDBG3(_T("{%s}hashdict::SetKey(\"%s\", @<%p>, overwrite=%s); keyhash=0x%llX(0x%0*X)"), dbgsig(),
		in_key,     // %s
		&in_value,  // @%p
		is_overwrite ? _T("yes") : _T("no"), 
		in_hashfull, (m_dict_width+1)/4, in_hashtail  // keyhash
		);

	// Probe LCG for matching key-slot.

	LCGen lcg(m_hashmask, (Uint32)in_hashfull, PerturbShift);

	int firstSlot = lcg.next();
	int idxSlot = firstSlot;

	int idxFirstDummy = DummyNotSeen;

	int probes = 0; // debug
	for(;; probes++)
	{
		const slot_st &slot = msa_slots[idxSlot];

		if(slot.state==SlotEmpty)
		{
			vaDBG3(_T("{%s}  probe#%d, land at idxSlot=%d, SlotEmpty."), dbgsig(),
				probes, idxSlot);

			// Determine: Use this slot, or, reuse the first-seen dummy slot.

			slot_st *pUseSlot = &msa_slots[idxSlot];
			if(idxFirstDummy!=DummyNotSeen)
			{
				vaDBG3(_T("{%s}  [optimize1] Use first-seen dummy idxSlot=%d"), dbgsig(), idxFirstDummy);
				pUseSlot = &msa_slots[idxFirstDummy];
				m_slots_dummy--;
			}

			int idxTrove = RequestTroveEntry();  // occupy a new trove entry.
			assert(idxTrove>=0);

			trove_entry_st &trovent = msa_trove[idxTrove];
			trovent.hashfull = in_hashfull;
			new(&trovent.key) Sdring(in_key);
			new(&trovent.uvalue) TU( std::move(in_value) ); // placement new() for the in-dict uvalue

			pUseSlot->state = SlotInUse;
			pUseSlot->idx_trove = idxTrove;
			m_slots_active++;

			vaDBG3(_T("{%s}hashdict::SetKey() new key added: idxSlot=%d, idxTrove=%d, value@<%p>"), dbgsig(),
				pUseSlot-msa_slots.GetElePtr(0), idxTrove, &trovent.uvalue);

			return &trovent.uvalue;
		}
		else if(slot.state==SlotInUse)
		{
			vaDBG3(_T("{%s}  probe#%d, land at idxSlot=%d, SlotInUse."), dbgsig(), probes, idxSlot);

			trove_entry_st *pte = IsKeyFoundAtSlot(idxSlot, in_hashfull, in_key);
			if(pte) // found key-match
			{
				assert(pte == &msa_trove[ msa_slots[idxSlot].idx_trove ]);
				trove_entry_st &trovent = *pte;

				if(is_overwrite)
				{
					vaDBG3(_T("{%s}hashdict::SetKey() overwrite old value: idxSlot=%d, idxTrove=%d ; value@<%p>"), dbgsig(),
						idxSlot, pte-msa_trove.GetElePtr(0), &trovent.uvalue);

					trovent.uvalue.~TU(); // call dtor on old value
					new(&trovent.uvalue) TU( std::move(in_value) ); // re-construct new value in-place

					return &trovent.uvalue;
				}
				else
				{
					vaDBG3(_T("{%s}hashdict::SetKey() found and keep old value: idxSlot=%d, idxTrove=%d, value=@<%p>"), dbgsig(),
						idxSlot, pte-msa_trove.GetElePtr(0), &trovent.uvalue);

					return &trovent.uvalue;
				}
			}
			else // not key-match yet 
			{
				// fall down
			}
		}
		else if(slot.state==SlotDummy)
		{
			vaDBG3(_T("{%s}  probe#%d, land at idxSlot=%d, SlotDummy."), dbgsig(), probes, idxSlot);

			if(idxFirstDummy==DummyNotSeen)
				idxFirstDummy = idxSlot;
		}
		else
			assert(0);

		idxSlot = lcg.next();

		if(probes>(64/PerturbShift+2) && idxSlot==firstSlot)
		{
			// idxSlot wraps around, can SlotDummy flooding cause this?
			vaDBG3(_T("{%s}BUG! hashdict::SetKey() see all slots exhausted. Devguy forgot to extend msa_slots."), dbgsig(), firstSlot);
			assert(0);
			return nullptr;
		}
	}

	return nullptr;
}

template<typename TU> 
TU* hashdict<TU>::get(const TCHAR *in_key)
{
	assert(in_key && in_key[0]);
	if(! (in_key && in_key[0]) )
		return nullptr;

	Uint64 in_hashfull = cal_hashfull(in_key);
	Uint32 in_hashtail = (Uint32)in_hashfull & m_hashmask;

	assert((int)in_hashtail < m_slots_capacity);

	// Probe LCG for matching key-slot.

	LCGen lcg(m_hashmask, (Uint32)in_hashfull, PerturbShift);

	int firstSlot = lcg.next();
	int idxSlot = firstSlot;

	int probes = 0; // debug
	for(;; probes++)
	{
		slot_st &slot = msa_slots[idxSlot];

		if(slot.state==SlotEmpty)
		{
			vaDBG3(_T("{%s}hashdict:get(\"%s\") NOT found after %d probes"), dbgsig(), 
				in_key, probes+1);
			return nullptr;
		}
		else if(slot.state==SlotInUse)
		{
			trove_entry_st *pte = IsKeyFoundAtSlot(idxSlot, in_hashfull, in_key);
			if(pte)
			{
				vaDBG3(_T("{%s}hashdict:get(\"%s\") found after %d probes, value@<%p>"), dbgsig(), 
					in_key, probes+1, &pte->uvalue);
				return &pte->uvalue;
			}
			else; // fall down
			
		}
		else if(slot.state==SlotDummy)
		{
			// do nothing, fall down
		}
		else
			assert(0);

		idxSlot = lcg.next();

		if(probes>(64/PerturbShift+2) && idxSlot==firstSlot)
		{
			// idxSlot wraps around, can SlotDummy flooding cause this?
			vaDBG2(_T("{%s}Weird! hashdict::get() see idxSlot(%d) wraps around."), dbgsig(), firstSlot);
			return nullptr; // not found
		}
	}
}


template<typename TU> 
void hashdict<TU>::CheckToCompactTrove()
{
	int dummypct = m_trove_dummies*100/m_trove_dirties;
	if(dummypct >= PctDummyToCompact)
	{
		vaDBG3(_T("{%s}  Triggering trove compact, bcz dummy pct reaches %d%% (%d/%d=%d)"), dbgsig(), 
			PctDummyToCompact,  m_trove_dummies, m_trove_dirties, dummypct);
		CompactTrove();
	}
}

template<typename TU> 
bool hashdict<TU>::del(const TCHAR *in_key)
{
	vaDBG3_DbgEnterExit;

	TU tempobj;
	bool succ = in_del(in_key, false, tempobj);
	if(!succ)
		return false;

	CheckToCompactTrove();
	return true;
}

template<typename TU> 
bool hashdict<TU>::del(const TCHAR *in_key, TU& recv_old_value)
{
	vaDBG3_DbgEnterExit;

	bool succ = in_del(in_key, true, recv_old_value);
	if(!succ)
		return false;

	CheckToCompactTrove();
	return true;
}

template<typename TU> 
bool hashdict<TU>::in_del(const TCHAR *in_key, bool is_recv_old, TU& recv_old_value)
{
	// Return: If in_key is found and deleted.

	if(m_enuming_sessions>0)
	{
		vaDBG1(_T("{%s}hashdict::in_del() NOT allowed when m_enuming_sessions(%d)>0"), dbgsig(), m_enuming_sessions);
		return false;
	}

	Uint64 in_hashfull = cal_hashfull(in_key);
	Uint32 in_hashtail = (Uint32)in_hashfull & m_hashmask;

	assert((int)in_hashtail < m_slots_capacity);

	vaDBG3(_T("{%s}hashdict::in_del(\"%s\"); keyhash=0x%llX(0x%0*X)"), dbgsig(), 
		in_key,     // %s
		in_hashfull, (m_dict_width+1)/4, in_hashtail  // keyhash
		);

	// Probe LCG for matching key-slot.

	LCGen lcg(m_hashmask, (Uint32)in_hashfull, PerturbShift);

	int probes = 0; // debug
	for(;; probes++)
	{
		int idxSlot = lcg.next();
		slot_st &slot = msa_slots[idxSlot];

		if(slot.state==SlotEmpty)
		{
			vaDBG3(_T("{%s}  key not found after %d probes."), dbgsig(), probes+1);
			return false; // key not found
		}
		else if(slot.state==SlotInUse)
		{
			trove_entry_st *pte = IsKeyFoundAtSlot(idxSlot, in_hashfull, in_key);
			if(pte)
			{
				assert(pte == &msa_trove[ msa_slots[idxSlot].idx_trove ]);
				trove_entry_st &trovent = *pte;

				vaDBG3(_T("{%s}  key found after %d probes, idxSlot=%d, idxTrove=%d, value@<%p>"), dbgsig(), 
					probes+1, idxSlot, pte-msa_trove.GetElePtr(0), &trovent.uvalue);
				
				if(is_recv_old)
				{
					recv_old_value = std::move(trovent.uvalue);
				}
				trovent.uvalue.~TU();

				trovent.state = TroventDummy;
				trovent.hashfull = InvalidHash64;
				trovent.key = nullptr;
				
				m_trove_dummies++;

				slot.state = SlotDummy;
				m_slots_dummy++;
				m_slots_active--;

				return true;
			}
		}
		else
		{
			assert(slot.state==SlotDummy);
		}
	}
}

template<typename TU> 
typename hashdict<TU>::trove_entry_st* 
hashdict<TU>::IsKeyFoundAtSlot(int idxSlot, Uint64 in_keyhash, const TCHAR *in_key)
{
	slot_st &slot = msa_slots[idxSlot];

	assert(slot.state==SlotInUse);

	trove_entry_st &trovent = msa_trove[slot.idx_trove];

	assert(trovent.state==TroventInUse);

	// Need to first confirm with hash match.

	if(trovent.hashfull!=in_keyhash)
		return nullptr;

	// Second confirm with exact key-string match

	if(trovent.key==in_key)
		return &trovent;
	else
		return nullptr;
}


template<typename TU> 
int hashdict<TU>::RequestTroveEntry() // request a new trove entry to use
{
	assert(m_trove_dirties <= m_trove_capacity);

	if(m_trove_dirties>0 && msa_trove[m_trove_dirties-1].state==TroventDummy)
	{	// a bit optimize, re-used tail dummy entry
		vaDBG3(_T("{%s}  [optimize2] Reuse tail dummy trovent at idxTrove=%d"), dbgsig(), m_trove_dirties-1);
		m_trove_dirties--;
		m_trove_dummies--;
	}

	if( m_trove_capacity==0 ||
		m_trove_dirties == m_trove_capacity-OneEmptyAtTroveEnd )
	{
		if(m_trove_capacity>0)
		{
			assert(msa_trove[m_trove_capacity-1].state==TroventEmpty
				&& msa_trove[m_trove_capacity-1].hashfull==InvalidHash64);
		}

		// need to extend trove, 
		// default is to double the size, but one-time-increase limits to TroveExtendOnceMax

		int inc_size = _MID_(TroveInitSize, m_trove_capacity, TroveExtendOnceMax); 
		
		int new_size = m_trove_capacity + inc_size;

		msa_trove.SetEleQuan(new_size, true); // true: new space clears to zero.
		m_trove_capacity = new_size;

		// Mark ending trovent as TroventEmpty guard.
		trove_entry_st &trovend = msa_trove[m_trove_capacity-1];
		trovend.state = TroventEmpty;
		trovend.hashfull = InvalidHash64; // debug purpose
	}

	const int idx_new_trovent = m_trove_dirties++;
	msa_trove[idx_new_trovent].state = TroventInUse;
//	new TU(&msa_trove[idx_new_trovent].uvalue);  // do it outside

	return idx_new_trovent;
}

template<typename TU> 
bool hashdict<TU>::CheckToIncreaseSlots()
{
	int thres_slots = -1;
	if(IsResizeByPct())
	{
		thres_slots = m_slots_capacity * m_resize_pct/100;
	}
	else
	{
		assert(m_hungry_slots<=0);
		thres_slots = m_slots_capacity + m_hungry_slots;
	}

	if(slots_non_empty() < thres_slots)
		return true;

	// Now increase the slots
	bool succ = RebuildSlotsFromTrove(m_dict_width+1);

	return succ ? true : false;
}

template<typename TU> 
bool hashdict<TU>::RebuildSlotsFromTrove(int new_dict_width)
{
	assert(new_dict_width>=m_dict_width);

	// Increase slots capacity if requested.

	if(new_dict_width > m_dict_width)
	{
		vaDBG3(_T("{%s}  hashdict: Increasing slots from %d(%d bits) to %d(%d bits)"), dbgsig(), 
			int_pow(2, m_dict_width), m_dict_width,  int_pow(2, m_dict_width+1), m_dict_width+1);

		m_dict_width = new_dict_width;
		m_slots_capacity = int_pow(2, m_dict_width);
		m_hashmask = m_slots_capacity -1 ;

		auto err = msa_slots.SetEleQuan(m_slots_capacity);
		if(err)
		{
			vaDBG0(_T("{%s}  [ERROR] msa_slots.SetEleQuan(%d) fail, no mem!"), dbgsig(), m_slots_capacity);
			return false;
		}
	}

	msa_slots.ZeroEles();

	// Rebuild the slots' content from trove.

	for(int i=0; i<m_trove_dirties; i++)
	{
		trove_entry_st &trovent = msa_trove[i];
		if(trovent.state!=TroventInUse)
		{
			assert(trovent.state==TroventDummy);
			continue;
		}

		Uint32 hashtail = (Uint32)trovent.hashfull & m_hashmask;

		LCGen lcg(m_hashmask, (Uint32)trovent.hashfull, PerturbShift);

		int probes = 0;
		for(;; probes++)
		{
			int idxSlot = lcg.next();
			slot_st &slot = msa_slots[idxSlot];

			if(slot.state==SlotEmpty)
			{
				slot.state = SlotInUse;
				slot.idx_trove = i;
				break;
			}

			assert(slot.state==SlotInUse);
			assert(probes<=m_slots_capacity);
		}
	} // for each trove-entry

	return true;
}

template<typename TU> 
bool hashdict<TU>::CompactTrove()
{
	vaDBG3(_T("{%s}hashdict::CompactTrove() capacity/dirty/dummy was %d/%d/%d, will compat to dirties=%d"), dbgsig(), 
		m_trove_capacity, m_trove_dirties, m_trove_dummies, m_trove_dirties-m_trove_dummies);

	// Purge all dummy trove-entries, then rebuild slots from the fresh trove.
	// This function runs unconditionally. When to call it is determined by caller.

	assert(m_trove_dirties < m_trove_capacity);
	assert(m_trove_dummies <= m_trove_dirties);
	assert(msa_trove[m_trove_capacity-1].state==TroventEmpty
		&& msa_trove[m_trove_capacity-1].hashfull==InvalidHash64);

	int isrc=0, idst=0;

	// Skip starting InUse entries, to avoid needless trovent moving.
	while(msa_trove[isrc].state == TroventInUse)
		isrc++, idst++;

	while(msa_trove[isrc].state != TroventEmpty)
	{
		while(msa_trove[isrc].state == TroventDummy)
			isrc++;

		if(msa_trove[isrc].state == TroventEmpty)
			break;

		assert(msa_trove[isrc].state == TroventInUse);

		while(msa_trove[isrc].state == TroventInUse)
		{
			msa_trove[idst] = std::move(msa_trove[isrc]);
			
			msa_trove[isrc].state = TroventEmpty;
			msa_trove[isrc].uvalue.~TU();
			
			idst++, isrc++;
		}
	}

	m_trove_dirties -= m_trove_dummies;
	m_trove_dummies = 0;

	bool succ = RebuildSlotsFromTrove(m_dict_width);
	return succ;
}

////\\\\////\\\\////\\\\////\\\\////\\\\////\\\\////\\\\

template<typename TU> 
const TCHAR* hashdict<TU>::enumer::next(TU **ppValue)
{
	SETTLE_OUTPUT_PTR(TU*, ppValue, nullptr)

	if(m_is_end)
		return nullptr;

	if(m_next_trovent == 0)
		m_dict.m_enuming_sessions++;

	for(; m_next_trovent<m_dict.m_trove_dirties; m_next_trovent++)
	{
		hashdict::trove_entry_st &trovent = m_dict.msa_trove[m_next_trovent];
		if(trovent.state == TroventInUse)
		{
			*ppValue = &trovent.uvalue;

			m_next_trovent++;

			return trovent.key.c_str();
		}
	}

	// End of trove, we return NULL to signify enum end.

	*ppValue = nullptr;

	m_is_end = true;
	m_dict.m_enuming_sessions--;

	return nullptr;
}

template<typename TU> 
void hashdict<TU>::enumer::reset()
{
	if(m_is_end)
	{
		m_is_end = false;
	}
	else
	{
		if(m_next_trovent>0)
			m_dict.m_enuming_sessions--;
	}

	m_next_trovent = 0;
}


#				ifndef hashdict_DEBUG
#				include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#				endif


////////////////////////////////////////////////////////////////////////////
} // namespace chjds
////////////////////////////////////////////////////////////////////////////


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++


#if defined(hashdict_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_hashdict) // [IMPL]


// >>> Include headers required by this lib's implementation

#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
#include <snTprintf.h>

// <<< Include headers required by this lib's implementation




#ifndef hashdict_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macro, from now on
#endif


////////////////////////////////////////////////////////////////////////////
namespace chjds {
////////////////////////////////////////////////////////////////////////////
// Place API function Implementation in this namespace.


// int getversion()
// {
// 	return 1;
// }


////////////////////////////////////////////////////////////////////////////
} // namespace chjds
////////////////////////////////////////////////////////////////////////////



#ifndef hashdict_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macro
#endif


#endif // [IMPL]


#endif // include once guard
