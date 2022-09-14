#include "stdafx.h"
#include "..\Public\HuntQuest1.h"
#include "GameInstance.h"
#include "Layer.h"

CHuntQuest1::CHuntQuest1(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CQuest(pGraphic_Device)
{
	ZeroMemory(&m_tQInfo, sizeof(QINFO));
}

CHuntQuest1::CHuntQuest1(const CHuntQuest1 & rhs)
	: CQuest(rhs)
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

	//m_bActive = true;

	memcpy(&m_tQInfo, pArg, sizeof(QINFO));

	iCount = 0;

	return S_OK;
}

void CHuntQuest1::Tick(void)
{
	__super::Tick();

	if (iCount != m_tQInfo.iHuntGoal)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		if (nullptr == pInstance)
			return;

		Safe_AddRef(pInstance);

		CLayer* pMonsterLayer = pInstance->Find_Layer(m_tQInfo.eLevelIndex, TEXT("Layer_Monster"));

		for (auto& iter : pMonsterLayer->Get_Objects())
		{
			if ((m_tQInfo.eMonType == (MONSTERTYPE)iter->Get_Info().iMonsterType) && true == iter->Get_Info().bDead)
				++iCount;
		}

		Safe_Release(pInstance);
	}
	else
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
