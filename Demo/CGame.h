#pragma once

#include "Scene\CMainMenu.h"
#include "Scene\CGameplay.h"

class CGame
{
private:
	POINT mousePosition;
	ID3DXFont* pDefaultFont = 0;

	CScene* currentScene = 0;
	
	CMainMenu mainMenu;
	CGameplay gameplay;

	int fps = 0;

public:
	static bool isPaused;
	static bool end;
	static std::map<int, std::string> _listPathObject;

	CGame();
	~CGame();

	void GameInit();
	void initListPathObject();

	bool IsLostDevice();

	void ToggleFullScreen();
	void OnLostDevice();
	void OnResetDevice();

	void Update(float dt, CDXInput* const inputDevice);
	void Render();
};
