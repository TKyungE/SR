#include "stdafx.h"
#include "..\Public\HuntQuest1.h"

CHuntQuest::CHuntQuest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CClientQuest(pGraphic_Device)
{
	ZeroMemory(&m_tQInfo, sizeof(QINFO));
}

CHuntQuest::CHuntQuest(const CHuntQuest & rhs)
	: CClientQuest(rhs)
	, m_tQInfo(rhs.m_tQInfo)
{
}

void CHuntQuest::Increase_Count(MONSTERTYPE eType)
{
	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
	{
		if (eType == m_tQInfo.pMonType[i])
			++m_pCount[i];
	}
}

HRESULT CHuntQuest::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_tQInfo.pHuntGoal = nullptr;
	m_tQInfo.pMonType = nullptr;

	return S_OK;
}

HRESULT CHuntQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tQInfo, pArg, sizeof(QINFO_DERIVED));

	m_pCount = new _uint[m_tQInfo.iCount];

	for (_int i = 0; i < m_tQInfo.iCount; ++i)
		m_pCount[i] = 0;

	return S_OK;
}

void CHuntQuest::Tick(void)
{
	__super::Tick();

	for (_uint i = 0; i < m_tQInfo.iCount; ++i)
	{
		if (m_pCount[i] < m_tQInfo.pHuntGoal[i])
		{
			m_bClear = false;
			return;
		}
	}

	m_bClear = true;
}

CHuntQuest * CHuntQuest::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHuntQuest* pInstance = new CHuntQuest(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHuntQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHuntQuest * CHuntQuest::Clone(void * pArg)
{
	CHuntQuest* pInstance = new CHuntQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHuntQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuntQuest::Free(void)
{
	__super::Free();

	if (nullptr != m_tQInfo.pHuntGoal)
		Safe_Delete_Array(m_tQInfo.pHuntGoal);

	if (nullptr != m_tQInfo.pMonType)
		Safe_Delete_Array(m_tQInfo.pMonType);

	if (nullptr != m_pCount)
		Safe_Delete_Array(m_pCount);
}
