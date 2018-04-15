#include "stdafx.h"

#include <TScalableArray.h>

typedef TScalableArray<TCHAR> TSA_tchar;
typedef TSA_tchar::ReCode_et Terr;

template<typename T>
bool tsa_imatch(const TScalableArray<T> &sa, int ele, int sto, int reallocs)
{
	TScalableArray<T>::Internal_st ins = sa.GetInternal();
	if( ins.ele==ele && ins.sto==sto && ins.reallocs==reallocs )
		return true;
	else
	{
		if(ins.ele!=ele)
			printf("tsa_imatch ERR: .ele expect=%d, actual=%d\n", ele, ins.ele);
		if(ins.sto!=sto)
			printf("tsa_imatch ERR: .sto expect=%d, actual=%d\n", sto, ins.sto);
		if(ins.reallocs!=reallocs)
			printf("tsa_imatch ERR: .reallocs expect=%d, actual=%d\n", reallocs, ins.reallocs);
		return false;
	}
}

TEST(tc_TScalableArray, Incsize1_Decsize1_Thres0) // tc: gtest test-case
{
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_tchar sa(3, 1, 1, 0);
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 0) );

	sa.SetEleQuan(1);
	EXPECT_TRUE( tsa_imatch(sa, 1, 1, 1) );

	sa.SetEleQuan(2);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 2) );

	sa.SetEleQuan(3);
	EXPECT_TRUE( tsa_imatch(sa, 3, 3, 3) );

	sa.SetEleQuan(2);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 4) );

	sa.SetEleQuan(3);
	EXPECT_TRUE( tsa_imatch(sa, 3, 3, 5) );

	Terr terr = sa.SetEleQuan(4);
	EXPECT_EQ(TSA_tchar::E_Full, terr);
	EXPECT_TRUE( tsa_imatch(sa, 3, 3, 5) );

	terr = sa.SetEleQuan(2);
	EXPECT_EQ(TSA_tchar::E_Success, terr);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 6) );

	// SetEleQuan keeps same value, reallocs should not change
	sa.SetEleQuan(2);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 6) );

	sa.SetEleQuan(0);
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 7 ) );
}

TEST(tc_TScalableArray, Incsize1_Decsize1_Thres1)
{
	const int DECTHRES1 = 1;
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_tchar sa(3, 1, 1, DECTHRES1);
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 0) );

	sa.SetEleQuan(3);
	EXPECT_TRUE( tsa_imatch(sa, 3, 3, 1) );

	sa.SetEleQuan(2); // no shrink happens
	EXPECT_TRUE( tsa_imatch(sa, 2, _3_, _1_) ); 

	sa.SetEleQuan(1); // shrink to 1 ele
	EXPECT_TRUE( tsa_imatch(sa, 1, _2_, _2_) ); 

	sa.SetEleQuan(0); // shrink to 0 ele, and still keep sto==1
	EXPECT_TRUE( tsa_imatch(sa, 0, _1_, _3_) ); 

	// so further append one ele will not cause reallocation
	sa.SetEleQuan(1);
	EXPECT_TRUE( tsa_imatch(sa, 1, 1, 3) ); 

	// Cleanup does nothing when there is still eles
	sa.Cleanup(); 
	EXPECT_TRUE( tsa_imatch(sa, 1, 1, 3) ); 

	sa.SetEleQuan(0); // shrink to 0 ele, and still keep sto==1
	EXPECT_TRUE( tsa_imatch(sa, 0, _1_, _3_) ); 

	sa.Cleanup(); // a Cleanup now clears all vacant storage
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 4) ); 
}

TEST(tc_TScalableArray, DecSize0)
{
	// TODO:
}

TEST(tc_TScalableArray, Incsize2)
{
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_tchar sa(5, 2, 2, 0);
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 0) );

	sa.SetEleQuan(1);
	EXPECT_TRUE( tsa_imatch(sa, 1, _2_, 1) );

	sa.SetEleQuan(2);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 1) );

	sa.SetEleQuan(3);
	EXPECT_TRUE( tsa_imatch(sa, 3, 4, 2) );

	sa.SetEleQuan(4);
	EXPECT_TRUE( tsa_imatch(sa, 4, 4, 2) );

	sa.SetEleQuan(5); // storage is not 6, bcz it will not exceed maxele
	EXPECT_TRUE( tsa_imatch(sa, 5, _5_, 3) );

	sa.SetEleQuan(4);
	EXPECT_TRUE( tsa_imatch(sa, 4, 4, 4) );
}
