#pragma once
class CClipWnd : public CWnd
{
public:
	CClipWnd(CWnd* host);
	void ReposeFrame(BOOL bShow=FALSE);

public:
	BOOL DetectWindow(POINT* pt,HWND* phWnd,LPRECT lpRect,HWND hWndParent=NULL);
	BOOL IsOccludedByFrame(POINT* pt);

private:
	CWnd* m_pWndHost;
	CRect m_rcScreen;
	CRect m_rcWndCapture;

private:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
