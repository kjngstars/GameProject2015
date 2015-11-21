#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <string>
#include "GameConfig.h"
#include "Singleton.h"


class CGraphics : public Singleton<CGraphics>
{
public:
	CGraphics(HINSTANCE hInstance, HWND window);
	~CGraphics();

	bool InitDirect3D(int sWidth, int sHeight, bool fullScreen);
	void UpdateInput();
	D3DXIMAGE_INFO LoadTexture(const char*filename, TEXTURE &texture, D3DCOLOR trancolor = D3DCOLOR_XRGB(255, 255, 255));

	LPDIRECT3DDEVICE9 getD3dx9Device()
	{
		return _d3ddev;
	}

	void beginDraw()
	{
		_d3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	}

	void endDraw()
	{
		_d3dSprite->End();
	}

	void setTransform(MATRIX mat)
	{
		_d3dSprite->SetTransform(&mat);
	}

	void draw(TEXTURE texture, RECT* rect, VECTOR3* center, VECTOR3* pos, D3DCOLOR color)
	{
		_d3dSprite->Draw(texture, rect, center, pos, color);
	}

#pragma region Get Method
	IDirect3DDevice9* GetDevice() { return this->_d3ddev; }

	ID3DXSprite* GetSprite() { return this->_d3dSprite; }

	HWND GetWND() { return this->_window; }

	HINSTANCE GetHInst() { return this->hInstance; }

	D3DPRESENT_PARAMETERS* GetD3DPP(){ return &this->d3dpp; }
#pragma endregion

private:
	LPDIRECT3D9 _d3d{};
	LPDIRECT3DDEVICE9 _d3ddev{};
	D3DPRESENT_PARAMETERS d3dpp;
	LPD3DXSPRITE _d3dSprite{};
	HWND _window;

	HINSTANCE hInstance;
};
