#include "stdafx.h"
#include "ClipWnd.h"

#define ID_TIMER_PRESS 456
const int step_time_millisec[]={100,500,100,10000};
#define num_step (sizeof(step_time_millisec)/sizeof(int))

CClipWnd::CClipWnd(CWnd* host):m_pWndHost(host),
	m_rcScreen(0,0,0,0),m_rcWndCapture(0,0,0,0)
{
	m_bShowClick=FALSE;
	m_bAutoPress=FALSE;
	m_iStep=-1;
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

BOOL CClipWnd::ParseCoordinate(LPCTSTR filename)
{
	if(!PathFileExists(filename))
		return FALSE;
	CFile file;
	if(!file.Open(filename,CFile::modeRead))
		return FALSE;
	UINT_PTR size=(UINT_PTR)file.GetLength();
	char* buf=new char[size];
	char numbuf[50];
	file.Read(buf,(UINT)size);
	m_ptClick.x=m_ptClick.y=-1;
	char *ptr=buf,*end,*strend=buf+size;
	for(end=ptr;end<strend&&*end!=',';end++);
	if(*end!=','||end-ptr>=50)
		goto fail;

	memcpy(numbuf,ptr,end-ptr);
	numbuf[end-ptr]=0;
	sscanf_s(numbuf,"%d",&m_ptClick.x);

	ptr=end+1,end=strend;
	if(end-ptr>=50)
		goto fail;

	memcpy(numbuf,ptr,end-ptr);
	numbuf[end-ptr]=0;
	sscanf_s(numbuf,"%d",&m_ptClick.y);

fail:
	delete[] buf;
	file.Close();

	if(m_ptClick.x==-1||m_ptClick.y==-1)
		return FALSE;

	return TRUE;
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
	if(!ParseCoordinate(_T("coord")))
		m_ptClick=m_rcScreen.CenterPoint();
	return 0;
}

void CClipWnd::ReposeFrame(BOOL bShow)
{
	if(!bShow)
	{
		EnableAutoPress(FALSE);
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_SHOW);
		::SetLayeredWindowAttributes(GetSafeHwnd(), RGB(0, 0, 0), 128, LWA_ALPHA);
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
BOOL CClipWnd::ComputeCaptureWnd(POINT* pt,HWND* phWnd,LPRECT lpRect)
{
	HWND hWnd=NULL,h=NULL;
	CRect rc,rect(0,0,0,0);
	while(DetectWindow(pt,&hWnd,&rc,hWnd))
	{
		h=hWnd;
		rect=rc;
	}
	if(h!=NULL)
	{
		*phWnd=h;
		*lpRect=rect;
		return TRUE;
	}
	return FALSE;
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
void CClipWnd::EnableAutoPress(BOOL bEnable)
{
	if((!m_bAutoPress)&&bEnable)
	{
		m_bAutoPress=TRUE;
		SetTimer(ID_TIMER_PRESS,1,NULL);
	}
	else if(m_bAutoPress&&(!bEnable))
	{
		m_bAutoPress=FALSE;
		KillTimer(ID_TIMER_PRESS);
		if(m_iStep>=0&&m_iStep%2==0&&m_CapStat.hWnd!=NULL)
		{
			CPoint pt=m_ptClick;
			::ScreenToClient(m_CapStat.hWnd,&pt);
			::SendMessage(m_CapStat.hWnd,WM_LBUTTONUP,0,MAKELONG(pt.x,pt.y));
		}
		m_iStep=-1;
	}
}

BEGIN_MESSAGE_MAP(CClipWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
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

	if(m_bShowClick)
	{
		CPen pen(PS_SOLID,2,RGB(255,0,0));
		CBrush brush2;
		brush2.CreateSolidBrush(RGB(0,255,0));
		CPen* oldpen=dc.SelectObject(&pen);
		CBrush* oldbrush=dc.SelectObject(&brush2);
		int radius=m_rcScreen.Height()/100;
		CPoint pt=m_ptClick;
		ScreenToClient(&pt);
		dc.Ellipse(m_ptClick.x-radius,m_ptClick.y-radius,
			m_ptClick.x+radius,m_ptClick.y+radius);
		dc.SelectObject(oldpen);
		dc.SelectObject(oldbrush);
	}
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

void CClipWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnRButtonUp(nFlags, point);
}

void CClipWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt=point;
	ClientToScreen(&pt);
	HWND hWnd;
	if(!ComputeCaptureWnd(&pt,&hWnd,&m_rcWndCapture))
		m_rcWndCapture=CRect(0,0,0,0);
	if(m_CapStat.hWnd!=NULL)
	{
		::ScreenToClient(m_CapStat.hWnd,&pt);
		::SendMessage(m_CapStat.hWnd,WM_MOUSEMOVE,nFlags&~MK_CONTROL,MAKELONG(pt.x,pt.y));
	}
	Invalidate();
	CWnd::OnMouseMove(nFlags, point);
}


void CClipWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt=point;
	ClientToScreen(&pt);
	if(m_bShowClick)
	{
		m_ptClick=pt;
		Invalidate();
	}
	else if((nFlags&MK_CONTROL))
	{
		if(!ComputeCaptureWnd(&pt,&m_CapStat.hWnd,&m_CapStat.rcWnd))
			m_CapStat.rcWnd=CRect(0,0,0,0);
		m_pWndHost->SendMessage(WM_NOTIFY_CAPTURE_STAT,(WPARAM)&m_CapStat);
		m_pWndHost->SendMessage(WM_NOTIFY_HIDE_CLIP_WND);
	}
	else if(m_CapStat.hWnd!=NULL)
	{
		::ScreenToClient(m_CapStat.hWnd,&pt);
		::SendMessage(m_CapStat.hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELONG(pt.x,pt.y));
	}
	CWnd::OnLButtonDown(nFlags, point);
}


void CClipWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt=point;
	ClientToScreen(&pt);
	if(m_bShowClick)
	{
		//Do nothing
	}
	else if(m_CapStat.hWnd!=NULL)
	{
		::ScreenToClient(m_CapStat.hWnd,&pt);
		::SendMessage(m_CapStat.hWnd,WM_LBUTTONUP,0,MAKELONG(pt.x,pt.y));
	}
	CWnd::OnLButtonUp(nFlags, point);
}


void CClipWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_SHIFT:
		m_bShowClick=!m_bShowClick;
		if(!m_bShowClick)
			EnableAutoPress(FALSE);
		break;
	case VK_SPACE:
		if(m_bShowClick)
			m_ptClick=m_rcScreen.CenterPoint();
		break;
	case VK_RETURN:
		EnableAutoPress(TRUE);
		break;
	}
	Invalidate();
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CClipWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==ID_TIMER_PRESS)
	{
		if((++m_iStep)==num_step)
			m_iStep=0;
		switch(m_iStep%2)
		{
		case 0:
			if(m_CapStat.hWnd!=NULL)
			{
				CPoint pt=m_ptClick;
				::ScreenToClient(m_CapStat.hWnd,&pt);
				::SendMessage(m_CapStat.hWnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELONG(pt.x,pt.y));
			}
			break;
		case 1:
			if(m_CapStat.hWnd!=NULL)
			{
				CPoint pt=m_ptClick;
				::ScreenToClient(m_CapStat.hWnd,&pt);
				::SendMessage(m_CapStat.hWnd,WM_LBUTTONUP,0,MAKELONG(pt.x,pt.y));
			}
			break;
		default:
			m_iStep=0;
			break;
		}
		KillTimer(ID_TIMER_PRESS);
		SetTimer(ID_TIMER_PRESS,(UINT_PTR)step_time_millisec[m_iStep],NULL);
	}
	CWnd::OnTimer(nIDEvent);
}


void CClipWnd::OnDestroy()
{
	EnableAutoPress(FALSE);
	char buf[100];
	sprintf_s(buf,100,"%d,%d",m_ptClick.x,m_ptClick.y);
	CFile file;
	if(file.Open(_T("coord"),CFile::modeCreate|CFile::modeWrite))
	{
		file.Write(buf,(UINT)strlen(buf));
		file.Close();
	}
	CWnd::OnDestroy();
}
