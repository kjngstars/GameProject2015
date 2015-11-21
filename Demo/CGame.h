#pragma once

#include "CGraphics.h"
#include "GameConfig.h"
#include "CObject.h"
#include "DX\CDXInput.h"
#include "CLuigi\CLuigi.h"
#include "CMap.h"
#include "Camera.h"


class CGame
{
private:
	ID3DXFont* pDefaultFont = 0;

	CMap _map;
	CLuigi luigi;
	CCamera* _camera;

public:
	static std::map<int, std::string> _listPathObject;

	bool isPaused = false;

	CGame();
	~CGame();

	void GameInit();
	void initListPathObject();

	bool IsLostDevice();

	void ToggleFullScreen();
	void OnLostDevice();
	void OnResetDevice();

	void Update(float dt, CDXInput* inputDevice);
	void Render();
};
