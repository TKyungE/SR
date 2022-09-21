#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"

BEGIN(Client)

class CSkyField final : public CLevel
{
private:
	CSkyField(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CSkyField() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Portal(const _tchar* pLayerTag);
private:
	CGameObject::INFO Info;

private:
	void Create_Monster(_float fTimeDelta);
	void Open_Level(void);
public:
	static CSkyField* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
private:
	_float fSound = 0.f;
	_float fCollTime = 0.f;
	_float fRainTime = 0.f;
	bool m_IconRender = false;
	bool m_bNextLevel = false;

};

END
