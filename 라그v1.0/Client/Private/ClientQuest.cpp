#include "stdafx.h"
#include "..\Public\ClientQuest.h"

CClientQuest::CClientQuest(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

CClientQuest::CClientQuest(const CClientQuest & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}


HRESULT CClientQuest::Initialize_Prototype(void)
{
	return S_OK;
}

HRESULT CClientQuest::Initialize(void * pArg)
{
	m_bClear = false;

	return S_OK;
}

void CClientQuest::Tick(void)
{
}

void CClientQuest::Free(void)
{
	Safe_Release(m_pGraphic_Device);
}
