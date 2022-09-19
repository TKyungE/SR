#include "stdafx.h"
#include "..\Public\HuntQuest1.h"

CHuntQuest1::CHuntQuest1(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CClientQuest(pGraphic_Device)
{
	ZeroMemory(&m_tQInfo, sizeof(QINFO));
}

CHuntQuest1::CHuntQuest1(const CHuntQuest1 & rhs)
	: CClientQuest(rhs)
{
}

HRESULT CHuntQuest1::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHuntQuest1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tQInfo, pArg, sizeof(QINFO));

	iCount = 0;

	return S_OK;
}

void CHuntQuest1::Tick(void)
{
	__super::Tick();

	if (iCount >= m_tQInfo.iHuntGoal)
		m_bClear = true;
}

CHuntQuest1 * CHuntQuest1::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHuntQuest1* pInstance = new CHuntQuest1(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHuntQuest1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHuntQuest1 * CHuntQuest1::Clone(void * pArg)
{
	CHuntQuest1* pInstance = new CHuntQuest1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHuntQuest1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuntQuest1::Free(void)
{
	__super::Free();
}
