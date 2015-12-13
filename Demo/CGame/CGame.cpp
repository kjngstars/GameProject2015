#include "../CGame.h"

bool CGame::isPaused = false;
bool CGame::end = false;
std::map<int, std::string> CGame::_listPathObject;

CGame::CGame()
{
}

CGame::~CGame()
{
	SAFE_RELEASE(this->pDefaultFont);
}

void CGame::GameInit()
{
#pragma region pDefaultFont
	if (this->pDefaultFont != nullptr)
		this->pDefaultFont->Release();

	HR(D3DXCreateFont(
		CGraphics::GetInstancePtr()->GetDevice(),
		20, 0,
		FW_NORMAL, 0, false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Times New Roman",
		&this->pDefaultFont));
#pragma endregion
	this->initListPathObject();

	this->mainMenu.Initialize(CGraphics::GetInstancePtr()->GetDevice());
	this->gameplay.Initialize(CGraphics::GetInstancePtr()->GetDevice());

	this->currentScene = &this->mainMenu;
}

void CGame::initListPathObject()
{
	int count = 1;
	std::string path;

	//ground 1
	path = "gameobject/ground1/";
	for (int i = 0; i < 41; ++i)
	{
		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//ground 2
	path = "gameobject/ground2/";
	for (int i = 0; i < 27; ++i)
	{
		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//flip ground
	path = "gameobject/flip/";
	for (int i = 0; i < 8; ++i)
	{
		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//grass
	path = "gameobject/grass/";
	for (int i = 0; i < 7; ++i)
	{

		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//ground 3
	path = "gameobject/ground3/";
	for (int i = 0; i < 14; ++i)
	{
		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//pipes
	path = "gameobject/pipes/";
	for (int i = 0; i < 14; i++)
	{
		_listPathObject.insert(std::make_pair(count, path + std::to_string(i) + ".png"));
		count++;
	}

	//enemy object
}

bool CGame::IsLostDevice()
{
	HRESULT hr = CGraphics::GetInstancePtr()->GetDevice()->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	else if (hr == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(0, "[CGame::IsLostDevice] Internal Driver Error", 0, 0);
		PostQuitMessage(0);
		return true;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		this->OnLostDevice();

		HR(CGraphics::GetInstancePtr()->GetDevice()->Reset(
			CGraphics::GetInstancePtr()->GetD3DPP()));

		this->OnResetDevice();

		return false;
	}
	else
		return false;
}

void CGame::ToggleFullScreen()
{
	if (CGraphics::GetInstancePtr()->GetD3DPP()->Windowed)
	{
		CGraphics::GetInstancePtr()->GetD3DPP()->Windowed = false;

		SetWindowLongPtr(
			CGraphics::GetInstancePtr()->GetWND()
			, GWL_STYLE, WS_POPUP);

		SetWindowPos(
			CGraphics::GetInstancePtr()->GetWND(), HWND_TOP,
			0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else
	{
		CGraphics::GetInstancePtr()->GetD3DPP()->Windowed = true;

		SetWindowLongPtr(CGraphics::GetInstancePtr()->GetWND(), GWL_STYLE,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);

		SetWindowPos(
			CGraphics::GetInstancePtr()->GetWND(), HWND_TOP,
			100, 100,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SWP_NOZORDER | SWP_SHOWWINDOW);

		this->OnLostDevice();

		HR(CGraphics::GetInstancePtr()->GetDevice()->Reset(
			CGraphics::GetInstancePtr()->GetD3DPP()));

		this->OnResetDevice();
	}
}

void CGame::OnLostDevice()
{
	HR(CGraphics::GetInstancePtr()->GetSprite()->OnLostDevice());
	HR(this->pDefaultFont->OnLostDevice());
	this->gameplay.OnLostDevice();
}

void CGame::OnResetDevice()
{
	HR(CGraphics::GetInstancePtr()->GetSprite()->OnResetDevice());
	HR(this->pDefaultFont->OnResetDevice());
	this->gameplay.OnResetDevice();
}

void CGame::Update(float dt, CDXInput* const inputDevice)
{
	switch (this->currentScene->Update(dt, inputDevice))
	{
	case SceneResult::None:
		break;

	case SceneResult::MainMenu:
		this->currentScene = &this->mainMenu;
		break;

	case SceneResult::Gameplay:
		this->currentScene = &this->gameplay;
		break;

	case SceneResult::Exit:
		CGame::end = true;
		break;
	}

	this->fps = std::roundf(1000.0f / dt);
}

void CGame::Render()
{
	CGraphics::GetInstancePtr()->getD3dx9Device()->Clear(0, NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	BEGIN_SCENE();
	BEGIN_DRAW();

	this->currentScene->Render(CGraphics::GetInstancePtr()->GetSprite());

#pragma region text
	MATRIX matTransform;
	D3DXMatrixIdentity(&matTransform);
	CGraphics::GetInstancePtr()->GetSprite()->SetTransform(&matTransform);

	RECT formatRect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	std::string str =
		"FPS: " + std::to_string(this->fps) + "\n" +
		"Mouse: " + std::to_string(this->mousePosition.x) + " " +
		std::to_string(this->mousePosition.y) + "\n";

	this->pDefaultFont->DrawText(
		CGraphics::GetInstancePtr()->GetSprite(),
		str.c_str(), -1, &formatRect, DT_NOCLIP,
		D3DCOLOR_XRGB(0, 0, 0));
#pragma endregion

	END_DRAW();
	END_SCENE();
	CGraphics::GetInstancePtr()->getD3dx9Device()->Present(nullptr, nullptr, nullptr, nullptr);
}