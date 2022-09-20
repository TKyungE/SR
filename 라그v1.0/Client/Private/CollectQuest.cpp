#include "stdafx.h"
#include "..\Public\CollectQuest.h"

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

	return S_OK;
}

HRESULT CCollectQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tQInfo, pArg, sizeof(QINFO_DERIVED));

	return S_OK;
}

void CCollectQuest::Tick(void)
{
}

CCollectQuest * CCollectQuest::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

CCollectQuest * CCollectQuest::Clone(void * pArg)
{
	return nullptr;
}

void CCollectQuest::Free(void)
{
}
