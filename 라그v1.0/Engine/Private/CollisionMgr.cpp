#include "..\Public\CollisionMgr.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

HRESULT CCollisionMgr::Reserve_Container(_uint iNumObjects)
{
	m_iNumGroup = iNumObjects;

	m_GameObjects = new GAMEOBJECTS[iNumObjects];

	return S_OK;
}

HRESULT CCollisionMgr::Add_ColiisionGroup(_uint iCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects[iCollisionGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

_bool CCollisionMgr::Collision(CGameObject * pGameObject, _uint iCollisionGroup, const _tchar* szColliderName, CGameObject** pTarget)
{
	if (nullptr == pGameObject)
	{
		ERR_MSG(TEXT("Failed to Collision : pGameObject"));
		return false;
	}

	for (auto& iter : m_GameObjects[iCollisionGroup])
	{
		if (Collision_AABB(pGameObject, iter, szColliderName))
		{
			*pTarget = iter;
			return true;
		}
	}

	return false;
}

void CCollisionMgr::Release_Objects(void)
{
	if (nullptr != m_GameObjects)
	{
		for (_uint i = 0; i < m_iNumGroup; ++i)
		{
			for (auto& iter : m_GameObjects[i])
				Safe_Release(iter);

			m_GameObjects[i].clear();
		}
	}
}

_bool CCollisionMgr::Collision_AABB(class CGameObject* _Dest, class CGameObject* _Sour, const _tchar* szColliderName)
{
	if (_Dest == _Sour)
		return false;

	CCollider* DestCollider = (CCollider*)_Dest->Find_Component(TEXT("Com_Collider"));
	if (nullptr == DestCollider)
	{
		ERR_MSG(TEXT("Failed to Check AABB : Dest"));
		return false;
	}

	_float3 DestMin = DestCollider->Find_MinPoint();
	_float3 DestMax = DestCollider->Find_MaxPoint();

	CCollider* SourCollider = (CCollider*)_Sour->Find_Component(szColliderName);
	if (nullptr == SourCollider)
	{
		ERR_MSG(TEXT("Failed to Check AABB : Sour"));
		return false;
	}

	ZeroMemory(&m_vCollision, sizeof(_float3));

	_float3 SourMin = SourCollider->Find_MinPoint();
	_float3 SourMax = SourCollider->Find_MaxPoint();

	if (_Dest->Get_World().m[3][0] < _Sour->Get_World().m[3][0])
	{
		if (DestMax.x <= SourMin.x)
			return false;
		else
		{
			m_vCollision.x = DestMax.x - SourMin.x;

			if (_Dest->Get_World().m[3][2] < _Sour->Get_World().m[3][2])
			{
				if (DestMax.z <= SourMin.z)
					return false;

				else
				{
					m_vCollision.z = DestMax.z - SourMin.z;

					if (_Dest->Get_World().m[3][1] < _Sour->Get_World().m[3][1])
					{
						if (DestMax.y <= SourMin.y)
							return false;

						else
							m_vCollision.y = DestMax.y - SourMin.y;
					}
					else
					{
						if (DestMin.y >= SourMax.y)
							return false;

						else
							m_vCollision.y = DestMin.y - SourMax.y;
					}
				}
			}
			else
			{
				if (DestMin.z >= SourMax.z)
					return false;

				else
				{
					m_vCollision.z = DestMin.z - SourMax.z;

					if (_Dest->Get_World().m[3][1] < _Sour->Get_World().m[3][1])
					{
						if (DestMax.y <= SourMin.y)
							return false;

						else
							m_vCollision.y = DestMax.y - SourMin.y;
					}
					else
					{
						if (DestMin.y >= SourMax.y)
							return false;

						else
							m_vCollision.y = DestMin.y - SourMax.y;
					}
				}
			}
		}
	}
	else
	{
		if (DestMin.x >= SourMax.x)
			return false;
		else
		{
			m_vCollision.x = DestMin.x - SourMax.x;

			if (_Dest->Get_World().m[3][2] < _Sour->Get_World().m[3][2])
			{
				if (DestMax.z <= SourMin.z)
					return false;

				else
				{
					m_vCollision.z = DestMax.z - SourMin.z;

					if (_Dest->Get_World().m[3][1] < _Sour->Get_World().m[3][1])
					{
						if (DestMax.y <= SourMin.y)
							return false;

						else
							m_vCollision.y = DestMax.y - SourMin.y;
					}
					else
					{
						if (DestMin.y >= SourMax.y)
							return false;

						else
							m_vCollision.y = DestMin.y - SourMax.y;
					}
				}

			}
			else
			{
				if (DestMin.z >= SourMax.z)
					return false;

				else
				{
					m_vCollision.z = DestMin.z - SourMax.z;

					if (_Dest->Get_World().m[3][1] < _Sour->Get_World().m[3][1])
					{
						if (DestMax.y <= SourMin.y)
							return false;

						else
							m_vCollision.y = DestMax.y - SourMin.y;
					}
					else
					{
						if (DestMin.y >= SourMax.y)
							return false;

						else
							m_vCollision.y = DestMin.y - SourMax.y;
					}
				}
			}
		}
	}

	return true;
}

void CCollisionMgr::Free(void)
{
	for (_uint i = 0; i < m_iNumGroup; ++i)
	{
		for (auto& iter : m_GameObjects[i])
			Safe_Release(iter);

		m_GameObjects[i].clear();
	}

	Safe_Delete_Array(m_GameObjects);
}
