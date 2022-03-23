#include <stdio.h>
#include <gadgetlib/ReposNewbox.h>

#include "share.h"

// Memo: use enum_windows_monitors.py to get your monitor layout.
// We prefer "work-area" over "monitor-area".

#define COUNT(ar) ((int)(sizeof(ar)/sizeof(ar[0])))

Rect_st gar_RectMonset1[] = // my office GBChj 4 monitors
{
	{ 0,0,        1600,870 }, // assume primary monitor
	{ -1920,0,    0,1170 },   // idx 1
	{ -320,-1080, 1600,-30 }, // idx 2
	{ 1600,-1016, 3040,1514 },// idx 3
};

struct TestCase_st
{
	// Input:
	Rect_st Parent;
	Size_st IdealNew;

	// Output expected:
	Rect_st OutRect;
	int idxMonitor; // zero-based
};

TestCase_st garCases[] = 
{
	// Parent @ origin, newbox narrower than parent. OutRect is inside parent's very center.
	{ {0,0, 300,200},   {200, 120},   {50,40, 300-50,200-40}, 0 },

	// Parent @ origin, newbox wider than parent. OutRect shift a bit left.
	{ {0,0, 300,200},   {400, 200},   {0,0, 400,200}, 0 },

	// Parent @ primary left-bottom. OutRect shift up and right.
	{ {0,800, 300,1000}, {400, 200},   {0,870-200, 400,870}, 0 },

	// New box 60%@monitor1, 40%@monitor0, OutRect@monitor1, Parent size==Newbox size.
	{ {-180,55, 120,155},  {300, 200},   {-300,5, 0,205}, 1 },

	// Parent @ monitor idx 2, newbox is tall thin(200*600), but not taller than monitor2's height.
	{ {0,-1000, 300,-800}, {200, 600},   {50,-1080, 250,-1080+600}, 2 },

	// Newbox placed at monitor 0, newbox ideal-size shrink to monitor size.
	{ {0,0, 300,200},   {1666, 999},   {0,0, 1600,870}, 0 },

	// Whole parent and whole center-newbox fall outsize all monitors, pick a nearest monitor(3).
	{ {1420,1514+10, 1620,1514+110},   {200, 100},   {1600,1514-100, 1600+200,1514}, 3 },
};
const int g_nCases = COUNT(garCases);

bool verify_ReposNewbox()
{
	ReposNewboxInput_st input = {};
	input.nMonitors = COUNT(gar_RectMonset1);
	input.arMonitorRect = gar_RectMonset1;
	
	int failcount = 0;

	int i;
	for(i=0; i<g_nCases; i++)
	{
		TestCase_st &cs = garCases[i];
		input.rectParent = cs.Parent;
		input.sizeNewboxIdeal = cs.IdealNew;
		
		Rect_st Outrect;
		Outrect.Reset();
		int idxMonitor = ggt_ReposNewbox(input, &Outrect);

		if(idxMonitor!=cs.idxMonitor || Outrect!=cs.OutRect)
		{
			failcount++;
			printf("Case %d error:\n", i);
			if(idxMonitor!=cs.idxMonitor)
			{
				printf("  idxMonitor output %d, correct is %d.\n", idxMonitor, cs.idxMonitor);
			}
			else if(Outrect!=cs.OutRect)
			{
				printf("  Rect output(%d,%d , %d,%d), correct is (%d,%d , %d,%d)\n",
					Outrect.left, Outrect.top, Outrect.right, Outrect.bottom, 
					cs.OutRect.left, cs.OutRect.top, cs.OutRect.right, cs.OutRect.bottom);
			}
		}
	}

	if(failcount==0)
		printf("ReposNewbox: All %d cases verify success.\n", COUNT(garCases));

	return failcount==0 ? true : false;
}


