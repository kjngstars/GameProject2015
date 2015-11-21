#pragma once

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS 60

#define APP_TITLE "Game"

//macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

//safe delete
#define SAFE_RELEASE(ptr) { if (ptr) { (ptr)->Release(); (ptr) = nullptr; } }

#define TEXTURE LPDIRECT3DTEXTURE9
#define VECTOR2 D3DXVECTOR2
#define VECTOR3 D3DXVECTOR3
#define MATRIX	D3DXMATRIX

//create texture function
#define CREATE_TEXTURE(fileName,texture) CGraphics::GetInstancePtr()->LoadTexture(fileName,texture)
#define CREATE_TEXTURE_TRAN(fileName,texture,color) CGraphics::GetInstancePtr()->LoadTexture(fileName,texture,color)

#define ccp(x,y) D3DXVECTOR2(x,y)

#define PI 3.1415926535

#define TO_RADIANS(d) d*(PI/180.0f)

#define SET_TRANSFORM(mat) CGraphics::GetInstancePtr()->setTransform(mat)
#define DRAW(texture,rect,center,pos,color) CGraphics::GetInstancePtr()->draw(texture,rect,center,pos,color)

//scene
#define BEGIN_SCENE() CGraphics::GetInstancePtr()->getD3dx9Device()->BeginScene()
#define END_SCENE() CGraphics::GetInstancePtr()->getD3dx9Device()->EndScene()
#define BEGIN_DRAW() CGraphics::GetInstancePtr()->beginDraw()
#define END_DRAW() CGraphics::GetInstancePtr()->endDraw()

#pragma region 0

#include <DxErr.h>

#define HR(x)											\
{														\
	HRESULT hr = x;										\
	if(FAILED(hr))										\
	{													\
		DXTrace(__FILE__, __LINE__, hr, #x, TRUE);	\
	}													\
}

#pragma