#include "..\Public\Quest.h"

CQuest::CQuest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

CQuest::CQuest(const CQuest & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CQuest::Initialize_Prototype(void)
{
	m_bActive = false;
	m_bClear = false;
	m_iIndex = 0;

	return S_OK;
}

HRESULT CQuest::Initialize(void * pArg)
{
	return S_OK;
}

void CQuest::Tick(void)
{
}

void CQuest::Free(void)
{
	Safe_Release(m_pGraphic_Device);
}