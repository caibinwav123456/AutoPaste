#pragma once
#define FRAME_LINE_WIDTH 5
#define WM_NOTIFY_HIDE_CLIP_WND (WM_USER+100)
#define WM_NOTIFY_CAPTURE_STAT  (WM_USER+200)

struct CaptureStat
{
	HWND hWnd;
	CRect rcWnd;
	CRect rcCapture;
	CaptureStat():hWnd(NULL){}
};
