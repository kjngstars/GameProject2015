#include "CLuigi.h"
#include "LuigiConstant.h"
#include "CSPoint.h"

void CLuigi::Render_Normal(ID3DXSprite* pSprite)
{
	if (this->invincibleTime > 0.0f)
	{
		HR(pSprite->End());
		HR(this->pEffect->SetFloat("hue", this->hue));
		UINT numPasses = 0;
		this->pEffect->Begin(&numPasses, 0);

		for (UINT i = 0;i < numPasses;i++)
		{
			HR(pSprite->Begin(D3DXSPRITE_ALPHABLEND));
			HR(this->pEffect->BeginPass(i));

			HR(pSprite->Draw(
				this->_texture, &this->sourceRect,
				0, 0, this->color));
			HR(this->pEffect->EndPass());
			HR(pSprite->End());
		}

		HR(this->pEffect->End());

		HR(pSprite->Begin(D3DXSPRITE_ALPHABLEND));
	}
	else
		HR(pSprite->Draw(
			this->_texture, &this->sourceRect,
			0, 0, this->color));
}

void CLuigi::Render_GrowToBig(ID3DXSprite* pSprite)
{
	HR(pSprite->Draw(
		this->_texture, &this->sourceRect,
		0, 0, D3DCOLOR_XRGB(255, 255, 255)));
}

void CLuigi::Render_GrowToFire(ID3DXSprite* pSprite)
{
	HR(pSprite->Draw(
		this->_texture, &this->sourceRect,
		0, 0, D3DCOLOR_XRGB(255, 255, 255)));
}

void CLuigi::Render_ShrinkToSmall(ID3DXSprite* pSprite)
{
	HR(pSprite->Draw(
		this->_texture, &this->sourceRect,
		0, 0, this->color));
}

void CLuigi::Render_Die(ID3DXSprite* pSprite)
{
	HR(pSprite->Draw(
		this->_texture, &this->sourceRect,
		0, 0, this->color));
}

void CLuigi::Render(ID3DXSprite* pSprite, CCamera* const pCamera)
{
	MATRIX matTransform;

	this->_anchorPoint = D3DXVECTOR2(this->GetWSrcRect() / 2.0f,
		this->GetHSrcRect() / 2.0f);

	D3DXVECTOR2 position_temp = pCamera->getPointTransform(
		this->_position.x, this->_position.y);

	D3DXMatrixTransformation2D(
		&matTransform,
		&this->_anchorPoint, 0, &this->_scale,
		&this->_anchorPoint, (float)this->_rotate,
		&D3DXVECTOR2(position_temp.x - this->_anchorPoint.x,
			position_temp.y - this->GetHSrcRect()));
	pSprite->SetTransform(&matTransform);

	switch (this->state)
	{
	case LuigiState::Normal:
	{
		this->Render_Normal(pSprite);
	}
	break;

	case LuigiState::GrowToBig:
	{
		this->Render_GrowToBig(pSprite);
	}
	break;

	case LuigiState::GrowToFire:
	{
		this->Render_GrowToFire(pSprite);
	}
	break;

	case LuigiState::Shink:
	{
		this->Render_ShrinkToSmall(pSprite);
	}
	break;

	case LuigiState::Die:
	{
		this->Render_Die(pSprite);
	}
	break;
	}

	CSPointManager::Render(pSprite, pCamera);
}