#ifndef __LiveUicXString_h_ 
#define __LiveUicXString_h_ 
#define __LiveUicXString_h_created_ 20260517
#define __LiveUicXString_h_updated_ 20260518


#include "LiveUic.h"
//	using namespace liveuic;

#include <DataXString.h>


template<typename T_LiveUic, typename TU, typename FORMAT=XStringFormatDefault>
class LiveUicXString : public T_LiveUic, public DataXString<TU, FORMAT>
{
public:
	using baseU = T_LiveUic;
	using baseX = DataXString<TU, FORMAT>;

public: // override LiveUic's virtual

	virtual void Reset() cxx11_override
	{
		baseU::Reset(); // LiveUic internal changed
		
		TU idata = baseU::GetXData();
		baseX::SetValue(idata);
	}

	virtual void DataToUic() cxx11_override
	{
		baseU::DataToUic();

		TU idata = baseU::GetXData();
		baseX::SetValue(idata);
	}

	virtual void DataFromUic() cxx11_override
	{
		baseU::DataFromUic();

		TU idata = baseU::GetXData();
		baseX::SetValue(idata);
	}


public: // override DataXString's virtual

	virtual void SetValueByString(const TCHAR *valsz, bool is_mark_dirty=true) cxx11_override
	{
		baseX::SetValueByString(valsz, is_mark_dirty);
		baseU::SetXData(baseX::m_val);
	}

	virtual void SetValueToDefault() cxx11_override 
	{
		// This override is from DataXString side. 
		// To eliminate 'default' ambiguity, we use 'default' from LiveUic side instead.

		this->Reset();
	}
};

#endif
