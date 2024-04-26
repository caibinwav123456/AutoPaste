#include "stdafx.h"
#include "ClipWnd.h"

CClipWnd::CClipWnd(CWnd* host):m_pWndHost(host),
	m_rcScreen(0,0,0,0)
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
		::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,m_rcScreen.left,m_rcScreen.top,
			m_rcScreen.Width(),m_rcScreen.Height(),0);
		Invalidate();
	}
}
BOOL CClipWnd::DetectWindow(POINT* pt,CWnd** ppWnd,HWND* phWnd,CWnd* pWndParent)
{
	CWnd* pWnd;
	if(pWndParent!=NULL)
		pWnd=pWndParent->ChildWindowFromPoint(*pt);
	else
		pWnd=WindowFromPoint(*pt);
	if(pWnd==NULL)
		return FALSE;
	*ppWnd=pWnd;
	*phWnd=pWnd->GetSafeHwnd();
	return TRUE;
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
	if(m_WndCapture.empty())
		return;
	rc=m_WndCapture.back().rcWnd;
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
	CWnd* pWnd=NULL;
	HWND hWnd=NULL;
	while(!m_WndCapture.empty())
	{
		if(!m_WndCapture.back().rcWnd.PtInRect(point))
			m_WndCapture.pop_back();
		else
			break;
	}
	if(m_WndCapture.empty())
	{
		ReposeFrame();
		if(DetectWindow(&point,&pWnd,&hWnd))
		{
			CRect rect;
			pWnd->GetWindowRect(&rect);
			m_WndCapture.push_back(WndStat(hWnd,rect));
		}
		ReposeFrame(TRUE);
	}
	else
	{
		hWnd=m_WndCapture.back().hWnd;
		pWnd=FromHandle(hWnd);
	}
	if(!m_WndCapture.empty())
	{
		while(DetectWindow(&point,&pWnd,&hWnd,pWnd))
		{
			TRACE(_T("(%p,%p)"),hWnd,pWnd);
			if(hWnd==m_WndCapture.back().hWnd)
				break;
			CRect rect;
			pWnd->GetWindowRect(&rect);
			m_WndCapture.push_back(WndStat(hWnd,rect));
		}
	}
	Invalidate();
	TRACE(_T("Dump:\n"));
	for(int i=0;i<(int)m_WndCapture.size();i++)
		TRACE(_T("(%p)"),m_WndCapture[i].hWnd);
	CWnd::OnMouseMove(nFlags, point);
}
