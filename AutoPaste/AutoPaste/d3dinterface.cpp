#include <windows.h>
#include "d3dinterface.h"
#include <d3d9.h>
#include <d3dx9.h>
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define CHECK_AND_RETURN(ret) \
	if(m_hWnd==NULL||!IsWindow(m_hWnd)||m_pDevice==NULL) \
	{ \
		DeleteObject(); \
		return ret; \
	}
struct D3DInterface
{
	IDirect3D9* s_pD3D;
	D3DInterface()
	{
		s_pD3D=Direct3DCreate9(D3D_SDK_VERSION);
	}
	~D3DInterface()
	{
		SAFE_RELEASE(s_pD3D);
	}
};
class ImageGrabD3D9:public ImageGrab
{
	friend ImageGrab* GetImageGrabD3D9();
public:
	ImageGrabD3D9();
	virtual ~ImageGrabD3D9();
	virtual BOOL InitObject(HWND hWnd);
	virtual void DeleteObject();
	virtual BOOL GrabToFile(LPCTSTR name);
protected:
	HWND m_hWnd;
private:
	IDirect3DDevice9* m_pDevice;
	RECT m_rcScreen;
	static IDirect3D9* GetD3D();
};
IDirect3D9* ImageGrabD3D9::GetD3D()
{
	static D3DInterface s_D3D;
	return s_D3D.s_pD3D;
}
ImageGrabD3D9::ImageGrabD3D9():ImageGrab()
{
	m_hWnd=NULL;
	m_pDevice=NULL;
	ZeroMemory(&m_rcScreen,sizeof(RECT));
}
ImageGrabD3D9::~ImageGrabD3D9()
{
	DeleteObject();
}
BOOL ImageGrabD3D9::InitObject(HWND hWnd)
{
	if(hWnd==NULL||!IsWindow(hWnd))
		return FALSE;
	m_hWnd=hWnd;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.SwapEffect=D3DSWAPEFFECT_COPY;
	d3dpp.Windowed=TRUE;
	HWND hScreen=GetDesktopWindow();
	if(hScreen==NULL)
	{
		m_hWnd=NULL;
		return FALSE;
	}
	GetWindowRect(hScreen, &m_rcScreen);
	HRESULT hr=GetD3D()->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hScreen,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDevice);
	if(FAILED(hr))
	{
		DeleteObject();
		return FALSE;
	}
	return TRUE;
}
void ImageGrabD3D9::DeleteObject()
{
	SAFE_RELEASE(m_pDevice);
	m_hWnd=NULL;
	ZeroMemory(&m_rcScreen,sizeof(RECT));
}
BOOL ImageGrabD3D9::GrabToFile(LPCTSTR name)
{
	CHECK_AND_RETURN(FALSE);
	RECT rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);
	if(rcWnd.left<m_rcScreen.left)rcWnd.left=m_rcScreen.left;
	if(rcWnd.top<m_rcScreen.top)rcWnd.top=m_rcScreen.top;
	if(rcWnd.right>m_rcScreen.right)rcWnd.right=m_rcScreen.right;
	if(rcWnd.bottom>m_rcScreen.bottom)rcWnd.bottom=m_rcScreen.bottom;
	if(rcWnd.left>=rcWnd.right||rcWnd.top>=rcWnd.bottom)
		return FALSE;
	rcWnd.left-=m_rcScreen.left,rcWnd.right-=m_rcScreen.left,
		rcWnd.top-=m_rcScreen.top,rcWnd.bottom-=m_rcScreen.top;
	INT_PTR subw=rcWnd.right-rcWnd.left,subh=rcWnd.bottom-rcWnd.top;
	RECT rcWndOrg={0,0,(int)subw,(int)subh};
	IDirect3DSurface9 *pSurf=NULL,*pSubSurf=NULL;
	D3DLOCKED_RECT d3drect,d3dsubrect;
	HRESULT hr=0;
	ZeroMemory(&d3drect,sizeof(D3DLOCKED_RECT));
	ZeroMemory(&d3dsubrect,sizeof(D3DLOCKED_RECT));
	if(FAILED(hr=m_pDevice->CreateOffscreenPlainSurface(m_rcScreen.right-m_rcScreen.left,m_rcScreen.bottom-m_rcScreen.top,
		D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&pSurf,NULL)))
		goto end;
	if(FAILED(hr=m_pDevice->CreateOffscreenPlainSurface((int)subw,(int)subh,
		D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&pSubSurf,NULL)))
		goto end;
	if(FAILED(hr=m_pDevice->GetFrontBufferData(0,pSurf)))
		goto end;
	if(FAILED(hr=pSurf->LockRect(&d3drect,&rcWnd,D3DLOCK_READONLY)))
		goto end;
	if(FAILED(hr=pSubSurf->LockRect(&d3dsubrect,&rcWndOrg,0)))
	{
		pSurf->UnlockRect();
		goto end;
	}
	for(INT_PTR i=0;i<subh;i++)
		memcpy((char*)d3dsubrect.pBits+(INT_PTR)d3dsubrect.Pitch*i,
			(char*)d3drect.pBits+(INT_PTR)d3drect.Pitch*i,4*subw);
	pSurf->UnlockRect();
	pSubSurf->UnlockRect();
	if(FAILED(hr=D3DXSaveSurfaceToFile(name,D3DXIFF_PNG,pSubSurf,NULL,NULL)))
		goto end;
end:
	SAFE_RELEASE(pSurf);
	SAFE_RELEASE(pSubSurf);
	return SUCCEEDED(hr);
}
ImageGrab* GetImageGrabD3D9()
{
	if(ImageGrabD3D9::GetD3D()==NULL)
		return NULL;
	ImageGrabD3D9* imageinterface=new ImageGrabD3D9;
	return imageinterface;
}
