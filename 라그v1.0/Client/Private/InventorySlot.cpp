#include "stdafx.h"
#include "..\Public\InventorySlot.h"
#include"GameInstance.h"
#include"Layer.h"

CInventorySlot::CInventorySlot(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CInventorySlot::CInventorySlot(const CInventorySlot & rhs)
	: CGameObject(rhs)
{
}
HRESULT CInventorySlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventorySlot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	

	m_fSizeX = 36.0f;
	m_fSizeY = 36.0f;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();


	Safe_AddRef(pGameInstance);
	if (m_tInfo.iHp == 1)
	{
		CGameObject::INFO tInfo;
		memcpy(&tInfo, &m_tInfo, sizeof(INFO));
		tInfo.pTarget = this;
		tInfo.iMp = 3;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Potion"), &tInfo);
		m_tInfo.pTarget = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Potion"))->Get_Objects().back();
		m_tInfo.bHit = true;
	}

	Safe_Release(pGameInstance);

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_tInfo.vPos.x - g_iWinSizeX * 0.5f, -m_tInfo.vPos.y + g_iWinSizeY * 0.5f, 0.f));//여기가중점
	return S_OK;
}

void CInventorySlot::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

}

void CInventorySlot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);

	
}

HRESULT CInventorySlot::Render()
{
	if (m_tInfo.pTerrain->Get_Info().bHit == false)
	{
	
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		
		if (m_tInfo.bHit)
		{
			CGameObject::INFO tInfo;
			memcpy(&tInfo, &m_tInfo, sizeof(INFO));
			tInfo.pTarget = this;
			m_tInfo.pTarget->Set_Info(tInfo);//m_tInfo.pTarget슬롯이 같고 타겟
			m_tInfo.pTarget->Set_UiMP(5);
		}
		else
		{
			if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
				return E_FAIL;
		}

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;

	}
	else
	{
	if (m_tInfo.pTarget != nullptr)
		{
			
			m_tInfo.pTarget->Set_UiMP(3);
		}
	}


	return S_OK;
}

HRESULT CInventorySlot::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Temp"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CInventorySlot::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}
HRESULT CInventorySlot::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CInventorySlot * CInventorySlot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventorySlot*	pInstance = new CInventorySlot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInventorySlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventorySlot::Clone(void * pArg)
{
	CInventorySlot*	pInstance = new CInventorySlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInventorySlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CInventorySlot::Get_World(void)
{
	return _float4x4();
}

void CInventorySlot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
