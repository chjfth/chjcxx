#ifndef __RECTxy_h_20170720_
#define __RECTxy_h_20170720_


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
