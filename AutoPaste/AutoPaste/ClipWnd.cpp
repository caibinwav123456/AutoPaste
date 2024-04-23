#include "stdafx.h"
#include "ClipWnd.h"

CClipWnd::CClipWnd()
{

}

void CClipWnd::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

	delete this;
}


BOOL CClipWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.dwExStyle|=WS_EX_TOOLWINDOW;
	cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1));
	return CWnd::PreCreateWindow(cs);
}

void CClipWnd::ReposeFrame(const CRect* rect)
{
	if(rect==NULL)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_SHOW);
		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,rect->left,rect->top,
			rect->Width(),rect->Height(),0);
		Invalidate();
	}
}

BEGIN_MESSAGE_MAP(CClipWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CClipWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}


void CClipWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc,RGB(255,0,0));
}


void CClipWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect rc,rcs;
	CRgn rgn,rgns,rgnc;
	GetClientRect(&rc);
	rcs=rc;
	int defx=min(FRAME_LINE_WIDTH,rc.Width()/2);
	int defy=min(FRAME_LINE_WIDTH,rc.Height()/2);
	rcs.DeflateRect(defx,defy,defx,defy);
	rgn.CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);
	rgns.CreateRectRgn(rcs.left,rcs.top,rcs.right,rcs.bottom);
	rgnc.CreateRectRgn(0,0,0,0);
	rgnc.CombineRgn(&rgn,&rgns,RGN_DIFF);
	SetWindowRgn((HRGN)rgnc.m_hObject,TRUE);
}
