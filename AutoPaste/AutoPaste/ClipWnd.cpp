#include "stdafx.h"
#include "ClipWnd.h"

CClipWnd::CClipWnd(CWnd* host):m_pWndHost(host),
	m_rcScreen(0,0,0,0),m_rcWndCapture(0,0,0,0)
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
	cs.dwExStyle|=(WS_EX_TOOLWINDOW|WS_EX_LAYERED);
	cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1));
	return CWnd::PreCreateWindow(cs);
}

int CClipWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi))
		return -1;
	m_rcScreen=mi.rcMonitor;
	return 0;
}

void CClipWnd::ReposeFrame(BOOL bShow)
{
	if(!bShow)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_SHOW);
		::SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), 128, LWA_ALPHA);
#if 1
		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,m_rcScreen.left,m_rcScreen.top,
			m_rcScreen.Width(),m_rcScreen.Height(),0);
#else
		CRect rc(100,100,200,200);
		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,rc.left,rc.top,
			rc.Width(),rc.Height(),0);
#endif
		Invalidate();
	}
}
BOOL CClipWnd::DetectWindow(POINT* pt,HWND* phWnd,LPRECT lpRect,HWND hWndParent)
{
	for(HWND hWnd=::GetTopWindow(hWndParent);hWnd!=NULL;hWnd=::GetWindow(hWnd,GW_HWNDNEXT))
	{
		//hWnd=::FindWindowEx(hWndParent,hWnd,NULL,NULL);
		if(hWnd==m_hWnd||!::IsWindowVisible(hWnd))
			continue;
		CRect rect;
		::GetWindowRect(hWnd,&rect);
		if(rect.PtInRect(*pt))
		{
			*phWnd=hWnd;
			*lpRect=rect;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CClipWnd::IsOccludedByFrame(POINT* pt)
{
	CPoint point=*pt;
	ScreenToClient(&point);
	CRgn rgn;
	rgn.CreateRectRgn(0,0,0,0);
	GetWindowRgn(rgn);
	return rgn.PtInRegion(point);
}

BEGIN_MESSAGE_MAP(CClipWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
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
	dc.FillSolidRect(&rc,RGB(0,0,0));
	if(m_rcWndCapture==CRect(0,0,0,0))
		return;
	rc=m_rcWndCapture;
	CPoint pt(0,0);
	ScreenToClient(&pt);
	rc.OffsetRect(pt);
	CRect rcs,rcin;
	rcin=rcs=rc;
	int defx=min(FRAME_LINE_WIDTH,rcs.Width()/2);
	int defy=min(FRAME_LINE_WIDTH,rcs.Height()/2);
	rcin.DeflateRect(defx,defy,defx,defy);

	CRgn rgns,rgnin,rgnc;
	rgns.CreateRectRgn(rcs.left,rcs.top,rcs.right,rcs.bottom);
	rgnin.CreateRectRgn(rcin.left,rcin.top,rcin.right,rcin.bottom);
	rgnc.CreateRectRgn(0,0,0,0);
	rgnc.CombineRgn(&rgns,&rgnin,RGN_DIFF);

	CBrush brush(RGB(255,0,0));
	dc.FillRgn(&rgnc,&brush);
}


void CClipWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
#if 0
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
#endif
}

HWND CreateFullscreenWindow(HWND hwnd)
{
	HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi))
		return NULL;
	return CreateWindow(_T("static"), _T("something interesting might go here"),
		WS_POPUP|WS_VISIBLE,
		mi.rcMonitor.left,
		mi.rcMonitor.top,
		mi.rcMonitor.right - mi.rcMonitor.left,
		mi.rcMonitor.bottom - mi.rcMonitor.top,
		hwnd, NULL, NULL, 0);
}


void CClipWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_pWndHost->SendMessage(WM_NOTIFY_HIDE_CLIP_WND);
	CWnd::OnRButtonDown(nFlags, point);
}


void CClipWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ClientToScreen(&point);
	HWND hWnd=NULL;
	CRect rc;
	m_rcWndCapture=CRect(0,0,0,0);
	while(DetectWindow(&point,&hWnd,&rc,hWnd))
	{
		m_rcWndCapture=rc;
	}
	Invalidate();
	CWnd::OnMouseMove(nFlags, point);
}
