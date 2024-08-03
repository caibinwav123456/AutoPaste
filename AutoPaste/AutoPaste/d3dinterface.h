#pragma once
class ImageGrab
{
public:
	ImageGrab(){};
	virtual ~ImageGrab(){};
	virtual BOOL InitObject(HWND hWnd)=0;
	virtual void DeleteObject()=0;
	virtual BOOL GrabToFile(LPCTSTR name)=0;
	void Release(){delete this;}
};
ImageGrab* GetImageGrabD3D9();
