#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"

BEGIN(Client)

class CLevel_MidBoss final : public CLevel
{
private:
	CLevel_MidBoss(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_MidBoss() = default;

public:
	virtual HRESULT Initialize(void);
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
	_float3 Get_CollisionPos(CGameObject* pDest, CGameObject* pSour);
	void Open_Level(void);

private:
	void LoadData(void);

	typedef struct tagIndexPos
	{
		_float3 BackGroundPos;
		_float3 vScale;
		_uint iIndex;
		_uint iTurn;
	}INDEXPOS;

	vector<INDEXPOS> m_vecTree;
	vector<INDEXPOS>	m_vecIndex;
	vector<INDEXPOS>	m_vecHouse;
	vector<INDEXPOS>	m_vecHouse2;
	vector<INDEXPOS>	m_vecPortal;
	vector<INDEXPOS>	m_vecNPC;
	vector<INDEXPOS>	m_vecWall;

	_float3	m_vPlayerPos;
	_float3 m_vBackPos;
	vector<_float3> m_vMonsterPos1;

private:
	void Create_Rain(_float fTimeDelta);

public:
	static CLevel_MidBoss* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
private:
	_float fSound = 0.f;
	_float fCollTime = 0.f;
	_float fRainTime = 0.f;
	bool m_IconRender = false;
	bool m_bNextLevel = false;
};

END

