#include "stdafx.h"

#include <TScalableArray.h>

typedef TScalableArray<TCHAR> TSA_tchar;
typedef TSA_tchar::ReCode_et Terr;

typedef TScalableArray<int> TSA_int;

template<typename T>
bool tsa_imatch(const TScalableArray<T> &sa, int ele, int sto, int reallocs)
{
	TScalableArray<T>::Internal_st ins = sa.GetInternal();
	if( ins.ele==ele && ins.sto==sto && (ins.reallocs==reallocs || reallocs==-1) )
	{
		return true;
	}
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

template<typename T>
bool tsa_compare_eles(const TScalableArray<T> &sa, const T expect[], int expects)
{
	int eles = sa.CurrentEles();
	if(eles!=expects)
	{
		printf("tsa_compare_eles ERR: eles expect=%d, actual=%d\n", expects, eles);
		return false;
	}

	int i;
	for(i=0; i<eles; i++)
	{
		if(sa[i]!=expect[i])
		{
			printf("tsa_compare_eles ERR: eles[%d] mismatch.\n", i);
			return false;
		}
	}
	return true;
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
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_tchar sa(3, 1, _0_, 0);
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 0) );

	sa.SetEleQuan(2);
	EXPECT_TRUE( tsa_imatch(sa, 2, 2, 1) );

	sa.SetEleQuan(1);
	EXPECT_TRUE( tsa_imatch(sa, 1, _2_, 1) );

	sa.SetEleQuan(0);
	EXPECT_TRUE( tsa_imatch(sa, 0, _2_, 1) );

	sa.Cleanup();
	EXPECT_TRUE( tsa_imatch(sa, 0, 0, 2) );
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

TEST(tc_TScalableArray, InsertBefore)
{
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_int sa(10, 1, 1, 0);

	// prepare [0,1,2]
	sa.SetEleQuan(3);
	sa[0] = 0, sa[1] = 1, sa[2] = 2;

	int result1[] = { 3,4, 0,1,2 };
	int ahead[2] = {3,4};
	sa.InsertBefore(0, ahead, 2);
	EXPECT_TRUE( tsa_compare_eles(sa, result1, 5) );

	const int int5 = 5;
	int result2[] = { 3,4, 5, 0,1,2 };
	sa.InsertBefore(2, int5);
	EXPECT_TRUE( tsa_compare_eles(sa, result2, 6) );

	// insert at a invalid location:
	TSA_int::ReCode_et err = sa.InsertBefore(7, 444);
	EXPECT_EQ(TSA_int::E_InvalidParam, err);
	
	err = sa.InsertBefore(-1, 444);
	EXPECT_EQ(TSA_int::E_InvalidParam, err);

	// Append at tail:
	int atail[2] = {6,7};
	int result3[] = { 3,4, 5, 0,1,2, 6,7 };
	sa.AppendTail(atail, 2);
	EXPECT_TRUE( tsa_compare_eles(sa, result3, 8) );

	// Append too many: No partial append
	int errtail[] = {-1, -2 -3};
	err = sa.AppendTail(errtail, 3); // would be 11 eles
	EXPECT_EQ(TSA_int::E_Full, err);
	EXPECT_TRUE( tsa_compare_eles(sa, result3, 8) ); // element count no change

	// Append too many in the middle:
	err = sa.InsertBefore(1, errtail, 3);
	EXPECT_EQ(TSA_int::E_Full, err);
	EXPECT_TRUE( tsa_compare_eles(sa, result3, 8) ); // element count no change

	// Append too many at head:
	err = sa.InsertBefore(0, errtail, 3);
	EXPECT_EQ(TSA_int::E_Full, err);
	EXPECT_TRUE( tsa_compare_eles(sa, result3, 8) ); // element count no change
}

TEST(tc_TScalableArray, DeleteEles)
{
	// memo: TScalableArray(MaxEle, IncSize, DecSize, DecThres);
	TSA_int a(8, 2, 4, 1);

	int a8[8] = {0,1,2,3,4,5,6,7};

	a.SetEleQuan(8);
	int *pints = a.GetElePtr();

	memcpy(pints, a8, sizeof(a8));
	EXPECT_TRUE( tsa_compare_eles(a, a8, 8) );

	a.DeleteEles(0, 2);
	EXPECT_TRUE( tsa_compare_eles(a, a8+2, 6) );
	EXPECT_TRUE( tsa_imatch(a, 6, 8, 1) );

	int a4[4] = { 2,3,4,7 };
	a.DeleteEles(3, 2);
	EXPECT_TRUE( tsa_compare_eles(a, a4, 4) );
	EXPECT_TRUE( tsa_imatch(a, 4, 8, 1) );

	int a3[3] = {2,4,7};
	a.DeleteEle(1); // delete ele idx=1
	EXPECT_TRUE( tsa_compare_eles(a, a3, 3) );
	EXPECT_TRUE( tsa_imatch(a, 3, 4, _2_) );

	// Delete at exact tail is allowed:
	TSA_int::ReCode_et err = a.DeleteEle(3);
	EXPECT_EQ(TSA_int::E_Success, err);

	// Delete beyond tail is invalid:
	err = a.DeleteEle(4);
	EXPECT_EQ(TSA_int::E_InvalidParam, err);

	// Delete extra count is allowed.
	int a1[1] = {2};
	err = a.DeleteEles(1, 3);
	EXPECT_EQ(TSA_int::E_Success, err);
	EXPECT_TRUE( tsa_compare_eles(a, a1, 1) );
}
