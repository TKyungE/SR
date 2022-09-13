#include "..\Public\Layer.h"


CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& iter = m_GameObjects.begin();
		iter != m_GameObjects.end();)
	{
		(*iter)->Tick(fTimeDelta);
		if ((*iter)->Get_Info().bDead)
		{
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}

}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

CGameObject * CLayer::Find_GameObject(_uint iIndex)
{
	auto& iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}

