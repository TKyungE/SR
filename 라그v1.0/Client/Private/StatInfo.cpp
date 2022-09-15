#include "stdafx.h"
#include "..\Public\StatInfo.h"
#include "GameInstance.h"
#include "Layer.h"
#include "KeyMgr.h"

CStatInfo::CStatInfo(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CStatInfo::CStatInfo(const CStatInfo& rhs)
	: CGameObject(rhs)
{
}

HRESULT CStatInfo::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatInfo::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&m_tStat, sizeof(STAT));
	for (int i = 0; i < 24; ++i)
	{
		ZeroMemory(&m_tItem[i], sizeof(ITEM));
		m_tItem[i].iSlotNum = i;
		m_tItem[i].eItemNum = EITEM_END;
	}
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	


	return S_OK;
}

void CStatInfo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_tInfo.bDead = false;
}

void CStatInfo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CStatInfo::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

CStatInfo * CStatInfo::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CStatInfo* pInstance = new CStatInfo(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CStats"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatInfo::Clone(void * pArg)
{
	CStatInfo* pInstance = new CStatInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CStats"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

_float4x4 CStatInfo::Get_World(void)
{
	return _float4x4();
}

void CStatInfo::Free(void)
{
	__super::Free();

}


