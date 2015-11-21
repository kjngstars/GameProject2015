#include "CGraphics.h"

CGraphics::CGraphics(HINSTANCE hInstance, HWND window)
{
	this->hInstance = hInstance;
	this->_window = window;

	//init direct stuff
	InitDirect3D(SCREEN_WIDTH, SCREEN_HEIGHT, false);

	//init sprite object
	HRESULT result = D3DXCreateSprite(_d3ddev, &_d3dSprite);
	if (FAILED(result))
	{
		PostQuitMessage(0);
	}

}

CGraphics::~CGraphics()
{
	SAFE_RELEASE(_d3d);
	SAFE_RELEASE(_d3ddev);
	SAFE_RELEASE(_d3dSprite);
}

bool CGraphics::InitDirect3D(int sWidth, int sHeight, bool fullScreen)
{
	//initialize Direct3D
	_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!_d3d)
	{
		MessageBox(0, "[CGraphics::InitDirect3D] Direct3DCreate9 FAILED", 0, 0);
		return false;
	}

#pragma region devBehaviorFlags
	D3DCAPS9 caps;
	HR(this->_d3d->GetDeviceCaps(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps));

	DWORD devBehaviorFlags = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
		devBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		devBehaviorFlags |= D3DCREATE_PUREDEVICE;
#pragma endregion

	//set Direct3D presentation parameters
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow = this->_window;
	d3dpp.Windowed = (!fullScreen);
	d3dpp.SwapEffect =D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;

	//create Direct3D device
	_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->_window,
		devBehaviorFlags, &d3dpp, &_d3ddev);
	if (!_d3ddev)
	{
		MessageBox(0, "[CGraphics::InitDirect3D] CreateDevice FAILED", 0, 0);
		return false;
	}

	return true;
}

D3DXIMAGE_INFO CGraphics::LoadTexture(const char * filename, TEXTURE & texture, D3DCOLOR trancolor)
{
	std::string errorMsg;
	HRESULT result;

	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	result = D3DXGetImageInfoFromFile(filename, &info);
	if (result != D3D_OK)
	{
		errorMsg = "Can't get image info " + std::string{ filename };
		MessageBox(_window, errorMsg.c_str(), "Error", 0);
	}

	//create the new texture by loading a bitmap image file
	result = D3DXCreateTextureFromFileEx(
		_d3ddev,                //Direct3D device object
		filename,      //bitmap filename
		info.Width,            //bitmap image width
		info.Height,           //bitmap image height
		1,                     //mip-map levels (1 for no chain)
		D3DPOOL_DEFAULT,       //the type of surface (standard)
		D3DFMT_UNKNOWN,        //surface format (default)
		D3DPOOL_DEFAULT,       //memory class for the texture
		D3DX_DEFAULT,          //image filter
		D3DX_DEFAULT,          //mip filter
		trancolor,            //color key for transparency
		&info,                 //bitmap file info (from loaded file)
		NULL,                  //color palette
		&texture); ;           //destination texture

	if (result != D3D_OK)
	{
		errorMsg = "Can't create texture " + std::string{ filename };
		MessageBox(_window, errorMsg.c_str(), "Error", 0);
	}

	return info;
}