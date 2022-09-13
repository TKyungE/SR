#include "stdafx.h"
#include "..\Public\PlayerInfo.h"
#include "GameInstance.h"
#include "Layer.h"
#include "KeyMgr.h"

CPlayerInfo::CPlayerInfo(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CPlayerInfo::CPlayerInfo(const CPlayerInfo& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerInfo::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_tInfo.pTarget = this;
	memcpy(pArg, &m_tInfo, sizeof(INFO));
	

	return S_OK;
}

void CPlayerInfo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_tInfo.bDead = false;
}

void CPlayerInfo::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CPlayerInfo::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

CPlayerInfo * CPlayerInfo::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CPlayerInfo* pInstance = new CPlayerInfo(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerInfo::Clone(void * pArg)
{
	CPlayerInfo* pInstance = new CPlayerInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}


	return pInstance;
}

_float4x4 CPlayerInfo::Get_World(void)
{
	return _float4x4();
}

void CPlayerInfo::Free(void)
{
	__super::Free();

}





