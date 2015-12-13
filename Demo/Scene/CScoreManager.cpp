#include "CScoreManager.h"
#include "../GameConfig.h"

ID3DXFont* CScoreManager::pFont0 = 0;
int CScoreManager::totalScored = 0;

static RECT formatRect = { 10,10,SCREEN_WIDTH-10,SCREEN_HEIGHT-10 };

void CScoreManager::Initialize(IDirect3DDevice9* pD3DDevice)
{
	if (CScoreManager::pFont0 != nullptr)
		CScoreManager::pFont0->Release();

	HR(D3DXCreateFont(
		pD3DDevice,
		25, 0,
		FW_BOLD, 0, false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"./Content/Scene/comic.ttf",
		&CScoreManager::pFont0));
}

void CScoreManager::Destroy()
{
	SAFE_RELEASE(CScoreManager::pFont0);
}

void CScoreManager::OnLostDevice()
{
	CScoreManager::pFont0->OnLostDevice();
}

void CScoreManager::OnResetDevice()
{
	CScoreManager::pFont0->OnResetDevice();
}

void CScoreManager::Update(float elapsedTime)
{
}

void CScoreManager::Render(ID3DXSprite* pSprite)
{
	MATRIX matTransform;
	D3DXMatrixIdentity(&matTransform);
	pSprite->SetTransform(&matTransform);
	
	CScoreManager::pFont0->DrawTextA(
		pSprite,
		std::to_string(CScoreManager::totalScored).c_str(),
		-1, &formatRect, DT_TOP | DT_RIGHT | DT_NOCLIP,
		D3DCOLOR_XRGB(255, 255, 255));
}

void CScoreManager::AddScore(int score)
{
	CScoreManager::totalScored += score;
}

void CScoreManager::Reset(ID3DXSprite* pSprite)
{
	CScoreManager::totalScored = 0;
}