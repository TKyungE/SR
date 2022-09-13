#pragma once

#include "GameObject.h"

/* 객체들을 모아 놓는다. */
BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);

public:
	typedef list<class CGameObject*>	GAMEOBJECTS;
public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	GAMEOBJECTS Get_Objects() { return m_GameObjects; }

	class CGameObject* Find_GameObject(_uint iIndex);
private:
	list<class CGameObject*>			m_GameObjects;
public:
	static CLayer* Create();
	virtual void Free() override;
};

END