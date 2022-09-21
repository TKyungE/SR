#include "stdafx.h"
#include "..\Public\CollectQuest.h"
#include "GameInstance.h"
#include "Layer.h"

CCollectQuest::CCollectQuest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CClientQuest(pGraphic_Device)
{
	ZeroMemory(&m_tQInfo, sizeof(QINFO_DERIVED));
}

CCollectQuest::CCollectQuest(const CCollectQuest & rhs)
	: CClientQuest(rhs)
	, m_tQInfo(rhs.m_tQInfo)
{
}

HRESULT CCollectQuest::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_tQInfo.pCollectGoal = nullptr;
	m_tQInfo.pItemType = nullptr;

	return S_OK;
}

HRESULT CCollectQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tQInfo, pArg, sizeof(QINFO_DERIVED));

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return E_FAIL;

	Safe_AddRef(pInstance);

	m_pStatInfo = (CStatInfo*)pInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();

	m_pCount = new _uint[m_tQInfo.iCount];

	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
		m_pCount[i] = 0;

	m_pClear = new _bool[m_tQInfo.iCount];

	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
		m_pClear[i] = false;

	Safe_Release(pInstance);

	return S_OK;
}

void CCollectQuest::Tick(void)
{
	__super::Tick();

	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
	{
		for (_uint j = 0; j < 24; ++j)
		{
			if (m_tQInfo.pItemType[i] == m_pStatInfo->Get_Item(j).eItemNum)
			{
				m_pCount[i] = m_pStatInfo->Get_Item(j).iCount;
				if ((_int)m_tQInfo.pCollectGoal[i] <= m_pStatInfo->Get_Item(j).iCount)
					m_pClear[i] = true;
			}
		}
	}

	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
	{
		if (!m_pClear[i])
			return;
	}

	m_bClear = true;
}

CCollectQuest * CCollectQuest::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollectQuest* pInstance = new CCollectQuest(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollectQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCollectQuest * CCollectQuest::Clone(void * pArg)
{
	CCollectQuest* pInstance = new CCollectQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollectQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollectQuest::Free(void)
{
	__super::Free();

	if (nullptr != m_tQInfo.pCollectGoal)
		Safe_Delete_Array(m_tQInfo.pCollectGoal);

	if (nullptr != m_tQInfo.pItemType)
		Safe_Delete_Array(m_tQInfo.pItemType);

	if (nullptr != m_pCount)
		Safe_Delete_Array(m_pCount);

	if (nullptr != m_pClear)
		Safe_Delete_Array(m_pClear);
}
