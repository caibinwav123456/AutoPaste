
// AutoPasteDlg.h : header file
//

#pragma once


// CAutoPasteDlg dialog
class CAutoPasteDlg : public CDialogEx
{
// Construction
public:
	CAutoPasteDlg(CWnd* pParent = NULL);	// standard constructor
	~CAutoPasteDlg();

// Dialog Data
	enum { IDD = IDD_AUTOPASTE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnBnClickedButtonCopy();
	CButton m_ButtonCapture;
	BOOL m_bCapture;
	CWnd* m_pWndCopy;
	HWND m_hWndCopy;
	BYTE* m_pBmpData;
	UINT m_pBmpDataLen;
	void SetCaptureState(BOOL bCapture);
	void CopyWindow();
	void SetNewTimer();
	BOOL CheckPath();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CDateTimeCtrl m_TimeInterval;
	afx_msg void OnDatetimechangeDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	CTime m_tTimePicker;
	afx_msg void OnKillfocusDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	CString m_strSavePath;
	afx_msg void OnClickedButtonPath();
	afx_msg void OnKillfocusEditPath();
	afx_msg void OnSetfocusEditPath();
};
