#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollisionMgr final : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	CCollisionMgr();
	virtual ~CCollisionMgr() = default;

public:
	HRESULT Reserve_Container(_uint iNumObjects);
	HRESULT Add_ColiisionGroup(_uint iCollisionGroup, class CGameObject* pGameObject);
	_bool Collision(CGameObject * pGameObject, _uint iCollisionGroup, const _tchar* szColliderName, CGameObject** pTarget);
	void Release_Objects(void);

public:
	_float3 Get_Collision(void) { return m_vCollision; }

private:
	_uint m_iNumGroup = 0;
	_float3 m_vCollision = _float3(0.f, 0.f, 0.f);
	list <class CGameObject*>* m_GameObjects;
	typedef list<class CGameObject*> GAMEOBJECTS;

private:
	_bool Collision_AABB(class CGameObject* _Dest, class CGameObject* _Sour, const _tchar* szColliderName);

public:
	virtual void Free(void) override;
};

END