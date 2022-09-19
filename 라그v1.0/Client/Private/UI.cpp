#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 300.0f;
	m_fSizeY = 180.0f;
	m_fX = 150.f;
	m_fY = 90.f;
	m_rcLv = { 225,145,290,165 };
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	tInfo.bDead = false;
	tInfo.pTarget = m_tInfo.pTarget;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerHp"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerMp"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerExp"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_QuickSlot"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
		return E_FAIL;
	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (CKeyMgr::Get_Instance()->Key_Down('L'))
	{
		switch (m_bStatus)
		{
		case true:
			m_bStatus = false;
			break;
		case false:
			CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			CGameObject::INFO tInfo;

			tInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tInfo.bDead = false;
			tInfo.pTarget = m_tInfo.pTarget;
			tInfo.pTerrain = this;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Status"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
				return;
			dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(true);
			Safe_Release(pGameInstance);
			m_bStatus = true;
			break;
		}
	}
		if (CKeyMgr::Get_Instance()->Key_Down('I'))
		{
			switch (m_bInven)
			{
			case true:
				m_bInven = false;
				break;
			case false:
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				CGameObject::INFO tInfo;

				tInfo.iLevelIndex = m_tInfo.iLevelIndex;
				tInfo.bDead = false;
				tInfo.pTarget = m_tInfo.pTarget;
				tInfo.pTerrain = this;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Inven"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
					return;
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(true);
				Safe_Release(pGameInstance);
				m_bInven = true;
				break;
			}
		}
		if (CKeyMgr::Get_Instance()->Key_Down('K'))
		{
			switch (m_bSkill)
			{
			case true:
				m_bSkill = false;
				break;
			case false:
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				CGameObject::INFO tInfo;

				tInfo.iLevelIndex = m_tInfo.iLevelIndex;
				tInfo.bDead = false;
				tInfo.pTarget = m_tInfo.pTarget;
				tInfo.pTerrain = this;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkillSlot"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
					return;
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(true);
				Safe_Release(pGameInstance);
				m_bSkill = true;
				break;
			}
		}
		if (CKeyMgr::Get_Instance()->Key_Down('E'))
		{
			switch (m_bEquip)
			{
			case true:
				m_bEquip = false;
				break;
			case false:
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				CGameObject::INFO tInfo;

				tInfo.iLevelIndex = m_tInfo.iLevelIndex;
				tInfo.bDead = false;
				tInfo.pTarget = m_tInfo.pTarget;
				tInfo.pTerrain = this;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Equip"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
					return;
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(true);
				Safe_Release(pGameInstance);
				m_bEquip = true;
				break;
			}
		}
}

void CUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	wstring szLv = L"";
	szLv = to_wstring(m_tInfo.pTarget->Get_Info().iLv);
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Get_Font()->DrawText(nullptr, szLv.c_str(), (_int)szLv.length() , &m_rcLv, DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));

	Safe_Release(pGameInstance);

	if (FAILED(Release_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI::SetUp_Components()
{
/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;	

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CUI::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CUI * CUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI*	pInstance = new CUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI::Clone(void* pArg)
{
	CUI*	pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CUI::Get_World(void)
{
	return _float4x4();
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
