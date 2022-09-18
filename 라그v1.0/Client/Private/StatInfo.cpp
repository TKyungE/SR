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
	ZeroMemory(&m_tMaxStat, sizeof(STAT));
	ZeroMemory(&m_MouseItem, sizeof(ITEM));

	for (int i = 0; i < 24; ++i)
	{
		ZeroMemory(&m_tItem[i], sizeof(ITEM));
		m_tItem[i].iSlotNum = i;
		m_tItem[i].eItemNum = EITEM_END;
	}
	for (int i = 0; i < 10; ++i)
		m_pQuickSlot[i] = { EITEM_END ,0,0};
	for (int i = 0; i < 7; ++i)
		m_SkillSlot[i] = { EITEM_END ,i,0 };
	for (int i = 0; i < 10; ++i)
		m_EquipSlot[i] = { EITEM_END ,i,0 };
	
	m_SkillSlot[0].eItemNum = SKILL_THUNDER;
	m_SkillSlot[1].eItemNum = SKILL_TORNADO;
	m_SkillSlot[2].eItemNum = SKILL_FIREBALL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	
	return S_OK;
}

void CStatInfo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Equip();

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

void CStatInfo::Check_Equip()
{
	_int iStr = 0;
	_int iDex = 0;
	_int iInt = 0;
	_int iLuk = 0;

	for (int i = 0; i < 10; ++i)
	{
		if (m_EquipSlot[i].eItemNum != EITEM_END)
		{
			switch (m_EquipSlot[i].eItemNum)
			{
			case CStatInfo::EARRING:
				iLuk += 10;
				break;
			case CStatInfo::TIARA:
				iDex += 5;
				iInt += 5;
				break;
			case CStatInfo::PANDANT:
				iInt += 5;
				break;
			case CStatInfo::BOBY:
				iDex += 10;
				break;
			case CStatInfo::STAFF:
				iStr += 10;
				break;
			case CStatInfo::ORB:
				iStr += 5;
				iInt += 5;
				break;
			case CStatInfo::ROBE:
				iLuk += 5;
				iInt += 5;
				break;
			case CStatInfo::SHOES:
				iDex += 5;
				iLuk += 5;
				break;
			case CStatInfo::BRACELET:
				iStr += 5;
				iDex += 5;
				break;
			case CStatInfo::RING:
				iStr += 5;
				iLuk += 5;
				break;
			default:
				break;
			}
		}
	}
	

	m_iEquipStr = iStr;
	m_iEquipDex = iDex;
	m_iEquipInt = iInt;
	m_iEquipLuk = iLuk;

	m_tMaxStat.iSTR = m_tStat.iSTR + m_iEquipStr;
	m_tMaxStat.iDEX = m_tStat.iDEX + m_iEquipDex;
	m_tMaxStat.iINT = m_tStat.iINT + m_iEquipInt;
	m_tMaxStat.iLUK = m_tStat.iLUK + m_iEquipLuk;
}


