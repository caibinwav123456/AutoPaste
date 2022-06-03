
// AutoPasteDlg.h : header file
//

#pragma once

struct CBmpData
{
	BYTE* m_pBmpData;
	UINT m_pBmpDataLen;
	CBmpData(BYTE* pBmpData, UINT pBmpDataLen):m_pBmpData(pBmpData), m_pBmpDataLen(pBmpDataLen){}
	~CBmpData()
	{
		if(m_pBmpData!=NULL)
			delete[] m_pBmpData;
	}
};
bool operator==(const CBmpData& a, const CBmpData& b);
bool operator!=(const CBmpData& a, const CBmpData& b);

struct CBmpArray
{
	CBmpData** m_pArrBmp;
	UINT m_nBmp;
	UINT m_iStart;
	UINT m_iEnd;
	struct Iterator
	{
		CBmpArray* m_pHost;
		UINT index;
		Iterator(CBmpArray* pHost);
		void operator++();
		void operator--();
		void operator++(int);
		void operator--(int);
		CBmpData*& operator*();
		CBmpData** operator->();
		void SetStart();
		void SetEnd();
		bool Start();
		bool End();
	};
	CBmpArray(UINT cnt);
	~CBmpArray();
	bool Full();
	bool Empty();
	CBmpData*& PushBack();
	CBmpData*& PopFront();
};

// CAutoPasteDlg dialog
class CAutoPasteDlg : public CDialogEx
{
// Construction
public:
	CAutoPasteDlg(CWnd* pParent = NULL);	// standard constructor

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
#if 0
	BYTE* m_pBmpData;
	UINT m_pBmpDataLen;
#endif
	CBmpArray m_arrBmp;
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
