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
}
ImageGrabD3D9::~ImageGrabD3D9()
{
	DeleteObject();
}
BOOL ImageGrabD3D9::InitObject(HWND hWnd)
{
	hWnd=GetDesktopWindow();
	if(hWnd==NULL||!IsWindow(hWnd))
		return FALSE;
	m_hWnd=hWnd;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.SwapEffect=D3DSWAPEFFECT_COPY;
	d3dpp.Windowed=TRUE;

	//IDirect3DDevice9* device=0;
	HRESULT hr=GetD3D()->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
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
}
BOOL ImageGrabD3D9::GrabToFile(LPCTSTR name)
{
	CHECK_AND_RETURN(FALSE);
	RECT rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);
	IDirect3DSurface9* pSurf=NULL;
	if(FAILED(m_pDevice->CreateOffscreenPlainSurface(rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top,
		D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&pSurf,NULL)))
		goto fail;
	if(FAILED(m_pDevice->GetFrontBufferData(0,pSurf)))
		goto fail;
	if(FAILED(D3DXSaveSurfaceToFile(name,D3DXIFF_PNG,pSurf,NULL,NULL)))
		goto fail;
	SAFE_RELEASE(pSurf);
	return TRUE;
fail:
	SAFE_RELEASE(pSurf);
	return FALSE;
}
ImageGrab* GetImageGrabD3D9()
{
	if(ImageGrabD3D9::GetD3D()==NULL)
		return NULL;
	ImageGrabD3D9* imageinterface=new ImageGrabD3D9;
	return imageinterface;
}
