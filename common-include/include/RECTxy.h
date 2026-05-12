#ifndef __RECTxy_h_
#define __RECTxy_h_
#define __RECTxy_h_created_ 20170720
#define __RECTxy_h_updated_ 20260512

#if defined(_WINDEF_) // from WinAPI windef.h
#define _RECT_STRUCT_DEFINED_
#endif

#include <ps_TCHAR.h>
#include <sdring.h>


#define RECTcx(r) ((r).right-(r).left)
#define RECTcy(r) ((r).bottom-(r).top)

#define RECTxcenter(r) (((r).left+(r).right)/2)
#define RECTycenter(r) (((r).top+(r).bottom)/2)

#define RECTarea(r) (RECTcx(r) * RECTcy(r))

#define RECT_IsSameSize(a, b) (RECTcx(a)==RECTcx(b) && RECTcy(a)==RECTcy(b))

#define RECT_AnySide_A_shorter_than_B(a, b) \
	( RECTcx(a)<RECTcx(b) || RECTcy(a)<RECTcy(b) )

#define RECT_AnySide_A_longer_than_B(a, b) \
	( RECTcx(a)>RECTcx(b) || RECTcy(a)>RECTcy(b) )


#ifdef _RECT_STRUCT_DEFINED_

inline bool operator==(const RECT& r1, const RECT& r2)
{
	if (r1.left == r2.left && r1.top == r2.top && r1.right == r2.right && r1.bottom == r2.bottom)
		return true;
	else
		return false;
}

inline bool operator!=(const RECT& r1, const RECT& r2)
{
	return !operator==(r1, r2);
}

#endif // _RECT_STRUCT_DEFINED_=yes

TCHAR* RECTtext(const RECT &r, TCHAR textbuf[], int buflen);

template<size_t eles>
inline TCHAR* RECTtext(const RECT &r, TCHAR(&textbuf)[eles])
{
	return RECTtext(r, textbuf, eles);
}

Sdring RECTtext(const RECT &r);


// The Rect_st struct here is described in .left .top .right .bottom .
// The .right and .bottom value is not inclusive -- same as Windows API RECT.
// So, rect.right-rect.left is the width, rect.bottom-rect.top is the height.

inline int rectxy_abs(int i){ return i>=0 ? i : -i; } // alias of abs()

enum IsRB_et // is right/bottom point
{
	RB_No = 0,
	RB_Yes = 1
};

inline bool InRectRange2(int n, const int *pi, IsRB_et isrb=RB_No)
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

struct Rect_st // same as Windows API RECT (right & bottom off-by-one)
{
	int left;
	int top;
	int right;
	int bottom;

	Rect_st(){ left=top=right=bottom=0; }     // old comment: would cause Visual C++ C2552 error.

	Rect_st(int l, int t, int r, int b) 
	{
		left = l; top = t; right = r; bottom = b;
	}

	void Reset(){ left=top=right=bottom=0; }

	bool operator==(const Rect_st &ri)
	{
		return (left==ri.left && top==ri.top && right==ri.right && bottom==ri.bottom) ? true:false;
	}
	bool operator!=(const Rect_st &ri)
	{
		return !(*this==ri);
	}

	inline Pace_st PaceToRect(const Rect_st &rBig) const;

#ifdef _RECT_STRUCT_DEFINED_
	operator const RECT&() {
		return *(RECT*)this;
	}
	Rect_st(const RECT& r) {
		*this = *(const RECT*)&r;
	}
	Rect_st& operator= (const RECT& r) {
		*this = *(const Rect_st*)&r;
		return *this;
	}
#endif
};


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


#if defined(RECTxy_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_RECTxy) // [IMPL]

#include <_MINMAX_.h>
#include <snTprintf.h>

#ifndef RECTxy_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


TCHAR* RECTtext(const RECT &r, TCHAR textbuf[], int buflen)
{
	// buflen suggestion: 80
	snTprintf(textbuf, buflen, _T("LT(%d,%d)RB(%d,%d)[%d*%d]"),
		r.left, r.top, r.right, r.bottom, (r.right - r.left), (r.bottom - r.top));
	return textbuf;
}

Sdring RECTtext(const RECT &r)
{
	Sdring sd(80);
	RECTtext(r, sd.getbuf(), sd.rawlen());
	return sd;
}


bool 
Point_st::InRect(const Rect_st &r, IsRB_et isrb) const
{
	// isrb: Is this Point a [RECT:right/bottom] point?

	bool xInRange = InRectRange2(x, &r.left, isrb);
	bool yInRange = InRectRange2(y, &r.top, isrb);
	return (xInRange && yInRange) ? true : false;
}

Pace_st 
Point_st::PaceToRect(const Rect_st &r, IsRB_et isrb) const
{
	// isrb: Is this Point a [RECT:right/bottom] point?
	//
	// Return: 
	// If ret.x== 30, it means this pt should walk toward right 30 pixels to reach the Rect's left border.
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
Rect_st::PaceToRect(const Rect_st &rTarget) const
{
	// Idea similar to Point_st:PaceToRect().
	// Calculate how much x & y to move so that this-Rect falls into rTarget.
	// 
	// Outgrown case: 
	// If this-Rect is wider than rTarget, this-Rect.x will be aligned to rTarget.x .
	// (same idea when this-Rect is taller)

	Pace_st paceRet = {0,0};

	// memo: A is this-rect's left-top corner; B is this-rect's right-bottom corner.
	Pace_st paceA = ((Point_st*)(&this->left))->PaceToRect(rTarget, RB_No);
	Pace_st paceB = ((Point_st*)(&this->right))->PaceToRect(rTarget, RB_Yes);

	// For each pace.x/pace.y, pick the one with larger abs-value.
	// We pick the "larger" value, bcz it is the pace to move this Rect_st wholly inside rTarget.
	paceRet.x = rectxy_abs(paceA.x)>rectxy_abs(paceB.x) ? paceA.x : paceB.x;
	paceRet.y = rectxy_abs(paceA.y)>rectxy_abs(paceB.y) ? paceA.y : paceB.y;

	// Check outgrown case:
	if(this->left + paceRet.x < rTarget.left)
		paceRet.x = rTarget.left - this->left;
	if(this->top  + paceRet.y < rTarget.top)
		paceRet.y = rTarget.top - this->top;

	return paceRet;

/*  Test cases:
	Rect_st rTarget= {100, 100, 200, 200};
	Pace_st pace;
	pace = Rect_st{110, 110, 200, 200}.PaceToRect(rTarget);   // ans: (0, 0)
	pace = Rect_st{110, 110, 220, 220}.PaceToRect(rTarget);   // ans: (-10, -10)
	pace = Rect_st{ 80, 190, 220, 230}.PaceToRect(rTarget);   // ans: (-20, -30)
*/
}


bool getLinesegOverlap(const int seg1[2], const int seg2[2], int ovlp[2])
{
	// Semantic:
	// Lineseg-end is off-by-one style. {1, 3} has length of 2.
	// If .end_ is smaller than .start, we consider the line length zero.

	int seg1_start = seg1[0];
	int seg1_end_ = _MAX_(seg1[0], seg1[1]);
	int seg2_start = seg2[0];
	int seg2_end_ = _MAX_(seg2[0], seg2[1]);

	int ovlp_start = seg1_start>seg2_start ? seg1_start : seg2_start;
	int ovlp_end_  = seg1_end_ <seg2_end_  ? seg1_end_  : seg2_end_;

	if (ovlp_start <= ovlp_end_) {
		ovlp[0] = ovlp_start;
		ovlp[1] = ovlp_end_;
		return true;
	}
	else {
		ovlp[0] = ovlp[1] = 0;
		return false;
	}
}

bool getRectOverlap(const Rect_st& rect1, const Rect_st& rect2, Rect_st *pOvlp)
{
	// Similar to WinAPI IntersectRect().
	// If input Rect's .left is larger than .right, its width is considered zero.
	
	pOvlp->left = pOvlp->right = pOvlp->top = pOvlp->bottom = 0;

	int xseg1[2] = {rect1.left, rect1.right};
	int xseg2[2] = {rect2.left, rect2.right};

	int xovlp[2], yovlp[2];
	
	bool isok = getLinesegOverlap(xseg1, xseg2, xovlp);
	if(!isok)
		return false;

	int yseg1[2] = { rect1.top, rect1.bottom };
	int yseg2[2] = { rect2.top, rect2.bottom };

	isok = getLinesegOverlap(yseg1, yseg2, yovlp);
	if(!isok)
		return false;

	pOvlp->left = xovlp[0]; pOvlp->right = xovlp[1];
	pOvlp->top = yovlp[0]; pOvlp->bottom = yovlp[1];
	return true;
}





#ifndef RECTxy_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
