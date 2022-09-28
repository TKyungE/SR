#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"

BEGIN(Client)

class CLevel_End final : public CLevel
{
private:
	CLevel_End(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_End() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
private:
	CGameObject::INFO Info;

public:
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);

public:
	static CLevel_End* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END