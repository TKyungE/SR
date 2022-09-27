#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"

BEGIN(Client)

class CLevel_Maze final : public CLevel
{
private:
	CLevel_Maze(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Maze() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
private:
	CGameObject::INFO Info;
private:		// 파일입출력 라인
	void LoadData();
	void LoadData2();

	typedef struct tagIndexPos		// 백그라운드 정보 읽기
	{
		_float3  BackGroundPos;
		_float3 vScale;
		_uint	iIndex;
		_uint iTrun;
	}INDEXPOS;

	vector<INDEXPOS>	m_vecTree;
	vector<INDEXPOS>	m_vecIndex;
	vector<INDEXPOS>	m_vecHouse;
	vector<INDEXPOS>	m_vecHouse2;
	vector<INDEXPOS>	m_vecPortal;
	vector<INDEXPOS>	m_vecNPC;
	vector<INDEXPOS>	m_vecWall;

	vector<INDEXPOS>	m_vecTree1;
	vector<INDEXPOS>	m_vecIndex1;
	vector<INDEXPOS>	m_vecHouse1;
	vector<INDEXPOS>	m_vecHouse21;
	vector<INDEXPOS>	m_vecPortal1;
	vector<INDEXPOS>	m_vecNPC1;
	vector<INDEXPOS>	m_vecWall1;

	_float3	m_vPlayerPos1;
	_float3 m_vBackPos1;

	_float3	m_vPlayerPos;
	_float3 m_vBackPos;
	vector<_float3> m_vMonsterPos1;
	vector<_float3> m_vMonsterPos2;
	_bool	m_bCheck = false;
private:
	bool m_bNextLevel = false;
	_bool m_bFade = false;
	_bool m_bPortal = false;
public:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);

	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Portal(const _tchar* pLayerTag);

private:
	void Open_Level(void);

public:
	static CLevel_Maze* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END