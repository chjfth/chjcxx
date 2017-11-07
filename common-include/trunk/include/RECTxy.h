#ifndef __RECTxy_h_20170720_
#define __RECTxy_h_20170720_

// The Rect struct here is described in .left .top .right .bottom .
// The .right and .bottom value is not inclusive -- same as Windows API RECT.
// So, rect.right-rect.left is the width, rect.bottom-rect.top is the height.

#define RECTxy_Abs(i) ( (i)>=0 ? (i) : 0-(i) )

enum IsRB_et // is right/bottom point
{
	RB_No = 0,
	RB_Yes = 1
};

inline bool InRange2(int n, const int *pi, IsRB_et isrb=RB_No)
{
	// If pi points to RECT.left, it will check whether n in range [RECT.left, RECT.right) .
	// If pi points to RECT.top, it will check whether n in range [RECT.top, RECT.bottom) .
	if(!isrb)
		return (n>=pi[0] && n<pi[2]) ? true : false;
	else
		return (n>pi[0] && n<=pi[2]) ? true : false;
}

struct Rect_st;

struct Pace_st { int x, y; };

struct Point_st
{
	int x, y;

	inline bool InRect(const Rect_st &r, IsRB_et isrb=RB_No) const;

	inline Pace_st PaceToRect(const Rect_st &r, IsRB_et isrb=RB_No) const;
};

struct Size_st
{
	int cx, cy;
};

struct Rect_st // same as Windows API RECT
{
	int left;
	int top;
	int right;
	int bottom;

	//	Rect_st(){ left=top=right=bottom=0; } // would cause Visual C++ C2552 error.

	void Reset(){ left=top=right=bottom=0; }

	bool operator==(const Rect_st &ri)
	{
		return (left==ri.left && top==ri.top && right==ri.right && bottom==ri.bottom) ? true:false;
	}
	bool operator!=(const Rect_st &ri)
	{
		return !(*this==ri);
	}

	inline Pace_st PaceToRect(const Rect_st &rBig);
};

bool 
Point_st::InRect(const Rect_st &r, IsRB_et isrb) const
{
	// isrb: Is this Point a [RECT:right/bottom] point?

	bool xInRange = InRange2(x, &r.left, isrb);
	bool yInRange = InRange2(y, &r.top, isrb);
	return (xInRange && yInRange) ? true : false;
}

Pace_st 
Point_st::PaceToRect(const Rect_st &r, IsRB_et isrb) const
{
	// isrb: Is this Point a [RECT:right/bottom] point?
	//
	// Return: 
	// If ret.x==30, it means this pt should walk toward right 30 pixels to reach the Rect's left border.
	// If ret.x==-20, it means this pt should walk toward left 20 pixels to reach the Rect's right border.
	// If this pt falls inside the rect(per axis), the pace is 0.

	Pace_st pace = {0,0};

	if(!isrb)
	{
		if(x<r.left)
			pace.x = r.left - x;
		else if(x>=r.right)
			pace.x = r.right - x -1; // -1 bcz right border not inclusive

		if(y<r.top)
			pace.y = r.top - y;
		else if(y>=r.bottom)
			pace.y = r.bottom - y -1; // -1 bcz bottom not inclusive
	}
	else
	{
		if(x<=r.left)
			pace.x = r.left - x +1;
		else if(x>r.right)
			pace.x = r.right - x;

		if(y<=r.top)
			pace.y = r.top - y +1;
		else if(y>r.bottom)
			pace.y = r.bottom - y;
	}

	return pace;
}

Pace_st 
Rect_st::PaceToRect(const Rect_st &rBig)
{
	// Idea similar to Point_st:PaceToRect().
	// this Rect must be smaller than or equal to rBig, in both x&y direction.

	Pace_st paceRet = {0,0};

	// memo: A is this-rect's left-top corner; B is this-rect's right-bottom corner.
	Pace_st paceA = ((Point_st*)(&this->left))->PaceToRect(rBig, RB_No);
	Pace_st paceB = ((Point_st*)(&this->right))->PaceToRect(rBig, RB_Yes);

	// For each pace.x/pace.y, pick the one with larger abs-value.
	// We pick the "larger" value, bcz it is the pace to move this Rect_st wholly inside rBig.
	paceRet.x = RECTxy_Abs(paceA.x)>RECTxy_Abs(paceB.x) ? paceA.x : paceB.x;
	paceRet.y = RECTxy_Abs(paceA.y)>RECTxy_Abs(paceB.y) ? paceA.y : paceB.y;

	return paceRet;
}



#define RECTcx(r) ((r).right-(r).left)
#define RECTcy(r) ((r).bottom-(r).top)

#define RECTxcenter(r) (((r).left+(r).right)/2)
#define RECTycenter(r) (((r).top+(r).bottom)/2)

#define RECT_IsSameSize(a, b) (RECTcx(a)==RECTcx(b) && RECTcy(a)==RECTcy(b))

#define RECT_AnySide_A_shorter_than_B(a, b) \
	( RECTcx(a)<RECTcx(b) || RECTcy(a)<RECTcy(b) )

#define RECT_AnySide_A_longer_than_B(a, b) \
	( RECTcx(a)>RECTcx(b) || RECTcy(a)>RECTcy(b) )


#endif
