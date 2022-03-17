
// AutoPasteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPaste.h"
#include "AutoPasteDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_TIMER 123
// CAboutDlg dialog used for App About

//Find path and name
inline void GetPathAndName(const CString& fullpath,CString& path,CString& name,TCHAR sep=_T('\\'))
{
	int pos=fullpath.ReverseFind(sep);
	if(pos==-1)
	{
		path=_T("");
		name=fullpath;
	}
	else
	{
		path=fullpath.Left(pos+1);
		name=fullpath.Right(fullpath.GetLength()-pos-1);
	}
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAutoPasteDlg dialog




CAutoPasteDlg::CAutoPasteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoPasteDlg::IDD, pParent),m_bCapture(TRUE),m_pWndCopy(NULL),
	m_hWndCopy(0),m_pBmpData(NULL),m_pBmpDataLen(0)
	, m_tTimePicker(2000,1,1,0,5,0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	TCHAR desktop[256];
	SHGetSpecialFolderPath(0,desktop,CSIDL_DESKTOPDIRECTORY,0);
	m_strSavePath=CString(desktop)+_T("\\backup.bmp");
}

CAutoPasteDlg::~CAutoPasteDlg()
{
	if(m_pBmpData!=NULL)
		delete[] m_pBmpData;
}

void CAutoPasteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CAPTURE, m_ButtonCapture);
	DDX_Control(pDX, IDC_DATETIMEPICKER, m_TimeInterval);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER, m_tTimePicker);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strSavePath);
}

BEGIN_MESSAGE_MAP(CAutoPasteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CAutoPasteDlg::OnBnClickedButtonCapture)
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CAutoPasteDlg::OnBnClickedButtonCopy)
	ON_WM_TIMER()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER, &CAutoPasteDlg::OnDatetimechangeDatetimepicker)
	ON_NOTIFY(NM_KILLFOCUS, IDC_DATETIMEPICKER, &CAutoPasteDlg::OnKillfocusDatetimepicker)
	ON_NOTIFY(NM_SETFOCUS, IDC_DATETIMEPICKER, &CAutoPasteDlg::OnSetfocusDatetimepicker)
	ON_BN_CLICKED(IDC_BUTTON_PATH, &CAutoPasteDlg::OnClickedButtonPath)
	ON_EN_KILLFOCUS(IDC_EDIT_PATH, &CAutoPasteDlg::OnKillfocusEditPath)
	ON_EN_SETFOCUS(IDC_EDIT_PATH, &CAutoPasteDlg::OnSetfocusEditPath)
END_MESSAGE_MAP()


// CAutoPasteDlg message handlers

BOOL CAutoPasteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetTimer(ID_TIMER,1000*60*5,NULL);

	//Init time picker
	m_TimeInterval.SetFormat(_T("mm:ss"));
	m_TimeInterval.SetTime(&m_tTimePicker);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoPasteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoPasteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoPasteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoPasteDlg::SetCaptureState(BOOL bCapture)
{
	if(bCapture)
	{
		m_ButtonCapture.SetWindowText(_T("Release"));
		SetCapture();
		m_bCapture=FALSE;
	}
	else
	{
		m_ButtonCapture.SetWindowText(_T("Capture"));
		ReleaseCapture();
		m_bCapture=TRUE;
	}
}

void CAutoPasteDlg::OnBnClickedButtonCapture()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetCaptureState(m_bCapture);
	UpdateData(FALSE);
}


void CAutoPasteDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCaptureState(FALSE);
	UpdateData(FALSE);
	CDialogEx::OnRButtonUp(nFlags, point);
}


void CAutoPasteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	do{
		if(m_bCapture)
			break;
		ClientToScreen(&point);
		CWnd* pWnd=WindowFromPoint(point);
		if(pWnd==NULL)
			break;
		HWND hwnd=pWnd->GetSafeHwnd();
		HWND hwndbutton=0;
		GetDlgItem(IDC_BUTTON_CAPTURE,&hwndbutton);
		if(hwnd==0||hwnd==hwndbutton)
		{
			SetCaptureState(FALSE);
			UpdateData(FALSE);
			break;
		}
		CRect rect;
		pWnd->GetWindowRect(&rect);
		CWnd* pDeskWnd=GetDesktopWindow();
		CWindowDC wdc(pDeskWnd);
		CPen pen;
		pen.CreatePen(PS_SOLID,10,RGB(255,0,0));
		CPen* oldpen=wdc.SelectObject(&pen);
		wdc.SelectStockObject(NULL_BRUSH);
		//rect.MoveToXY(0,0);
		wdc.Rectangle(&rect);
		wdc.SelectObject(oldpen);
		pen.DeleteObject();
		pDeskWnd->ReleaseDC(&wdc);
		m_pWndCopy=pWnd;
		m_hWndCopy=hwnd;
	}while(false);
	CDialogEx::OnLButtonDown(nFlags, point);
}

inline void CalcFullPath(const CString& pathname,CString& full,int cnt)
{
	CString path,title,name,ext;
	GetPathAndName(pathname,path,title);
	GetPathAndName(title,name,ext,_T('.'));
	name=name.Left(name.GetLength()-1);
	full.Format(_T("%s%s%d.%s"),(LPCTSTR)path,(LPCTSTR)name,cnt,(LPCTSTR)ext);
}

void CAutoPasteDlg::CopyWindow()
{
	if(m_hWndCopy==0)
		return;
	m_pWndCopy=FromHandle(m_hWndCopy);
	CWindowDC dc(m_pWndCopy);
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	CRect rectWndCopy;
	m_pWndCopy->GetWindowRect(&rectWndCopy);
	rectWndCopy.MoveToXY(0,0);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,rectWndCopy.Width(),rectWndCopy.Height());
	CBitmap* oldbmp=mdc.SelectObject(&bmp);
	mdc.BitBlt(0,0,rectWndCopy.Width(),rectWndCopy.Height(),&dc,0,0,SRCCOPY);
	int infoheader_rgbquad_size=sizeof(BITMAPINFOHEADER)+3*sizeof(RGBQUAD);
	BITMAPINFO* info=(BITMAPINFO*)new char[infoheader_rgbquad_size];
	memset(info,0,infoheader_rgbquad_size);
	info->bmiHeader.biSize=sizeof(BITMAPINFO);
	GetDIBits(mdc.m_hDC,(HBITMAP)bmp.m_hObject,0,rectWndCopy.Height(),NULL,info,DIB_RGB_COLORS);
	BITMAPFILEHEADER fh;
	memset(&fh,0,sizeof(fh));
	fh.bfType=0x4d42;
	fh.bfSize=sizeof(fh)+infoheader_rgbquad_size;
	fh.bfOffBits=fh.bfSize;
	fh.bfSize+=info->bmiHeader.biSizeImage;
	BYTE* pixel=new BYTE[fh.bfSize];
	int ret=GetDIBits(mdc.m_hDC,(HBITMAP)bmp.m_hObject,0,rectWndCopy.Height(),pixel+fh.bfOffBits,info,DIB_RGB_COLORS);
	memcpy(pixel,&fh,sizeof(fh));
	memcpy(pixel+sizeof(fh),info,infoheader_rgbquad_size);
	UINT filesize=fh.bfSize;
	BOOL changed=FALSE;
	BYTE* tmp=m_pBmpData;
	if(m_pBmpData==NULL||filesize!=m_pBmpDataLen||memcmp(pixel,m_pBmpData,filesize)!=0)
		changed=TRUE;
	m_pBmpDataLen=filesize;
	m_pBmpData=pixel;
	if(tmp!=NULL)
		delete[] tmp;
	delete[] (char*)info;
	mdc.SelectObject(oldbmp);
	mdc.DeleteDC();
	bmp.DeleteObject();
	if(!changed)
		return;
	static int cnt=1;
	CString full;
	if(CheckPath())
	{
		CalcFullPath(m_strSavePath,full,cnt);
		CFile file;
		while(!file.Open(full,CFile::modeCreate|CFile::modeWrite))
			Sleep(10*1000);
		file.Write(m_pBmpData,m_pBmpDataLen);
		file.Close();
		for(int i=cnt-100;i<cnt-10;i++)
		{
			CalcFullPath(m_strSavePath,full,i);
			if(PathFileExists(full))
				DeleteFile(full);
		}
		cnt++;
	}
}

void CAutoPasteDlg::OnBnClickedButtonCopy()
{
	// TODO: Add your control notification handler code here
	CopyWindow();
}

void CAutoPasteDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);
	SetCaptureState(FALSE);
	UpdateData(FALSE);
	// TODO: Add your message handler code here
}


void CAutoPasteDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==ID_TIMER)
		CopyWindow();
	CDialogEx::OnTimer(nIDEvent);
}


void CAutoPasteDlg::OnDatetimechangeDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_tTimePicker.GetMinute()==0&&m_tTimePicker.GetSecond()<10)
	{
		m_tTimePicker=CTime(2000,1,1,0,0,10);
		UpdateData(FALSE);
	}
	*pResult = 0;
}


void CAutoPasteDlg::OnKillfocusDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	SetNewTimer();
	*pResult = 0;
}


void CAutoPasteDlg::OnSetfocusDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	SetNewTimer();
	*pResult = 0;
}

void CAutoPasteDlg::SetNewTimer()
{
	UpdateData(FALSE);
	KillTimer(ID_TIMER);
	CTimeSpan span=m_tTimePicker-CTime(2000,1,1,0,0,0);
	int s=span.GetSeconds(),m=span.GetMinutes();
	SetTimer(ID_TIMER,1000*(span.GetMinutes()*60+span.GetSeconds()),NULL);
}

void CAutoPasteDlg::OnClickedButtonPath()
{
	// TODO: 在此添加控件通知处理程序代码
	const int buflen=2048;
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("Bitmap Files|*.bmp||"),this);
	TCHAR* strFileName=new TCHAR[buflen];
	TCHAR* strFileTitle=new TCHAR[buflen];
	CString dir,name;
	GetPathAndName(m_strSavePath,dir,name);
	_tcscpy(strFileName,m_strSavePath);
	_tcscpy(strFileTitle,name);
	dlg.m_ofn.lpstrFile=strFileName;
	dlg.m_ofn.lpstrFileTitle=strFileTitle;
	dlg.m_ofn.nMaxFile=buflen;
	dlg.m_ofn.nMaxFileTitle=buflen;
	if(dlg.DoModal()==IDOK)
	{
		m_strSavePath=dlg.GetPathName();
		CheckPath();
	}
	delete[] strFileName;
	delete[] strFileTitle;
}


void CAutoPasteDlg::OnKillfocusEditPath()
{
	// TODO: 在此添加控件通知处理程序代码
	CheckPath();
}


void CAutoPasteDlg::OnSetfocusEditPath()
{
	// TODO: 在此添加控件通知处理程序代码
	CheckPath();
}

BOOL CAutoPasteDlg::CheckPath()
{
	UpdateData(FALSE);
	CString dir,name;
	GetPathAndName(m_strSavePath,dir,name);
	if(!PathFileExists(dir))
	{
		MessageBox(_T("Path not exist!"));
		return FALSE;
	}
	return TRUE;
}
