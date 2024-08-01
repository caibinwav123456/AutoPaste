#pragma once
#include "struct.h"
class CClipWnd : public CWnd
{
public:
	CClipWnd(CWnd* host);
	void ReposeFrame(BOOL bShow=FALSE);

public:
	BOOL ComputeCaptureWnd(POINT* pt,HWND* phWnd,LPRECT lpRect);
	BOOL DetectWindow(POINT* pt,HWND* phWnd,LPRECT lpRect,HWND hWndParent=NULL);
	BOOL IsOccludedByFrame(POINT* pt);
	void EnableAutoPress(BOOL bEnable);
	BOOL ParseCoordinate(LPCTSTR filename);

private:
	CWnd* m_pWndHost;
	CRect m_rcScreen;
	CRect m_rcWndCapture;
	CaptureStat m_CapStat;
	CPoint m_ptClick;
	BOOL m_bShowClick;
	BOOL m_bAutoPress;
	int m_iStep;

private:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
