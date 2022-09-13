#include "stdafx.h"
#include "..\Public\SkillSlot.h"
#include"GameInstance.h"
#include"Layer.h"

CSkillSlot::CSkillSlot(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CSkillSlot::CSkillSlot(const CSkillSlot & rhs)
	: CGameObject(rhs)
{
}
HRESULT CSkillSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillSlot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_fSizeX = 36.0f;
	m_fSizeY = 36.0f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();


	Safe_AddRef(pGameInstance);
	if (m_tInfo.iHp == 1)
	{
		m_tInfo.iMp = 5;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Potion"), &m_tInfo);
		m_tInfo.pTarget = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Potion"))->Get_Objects().back();
	}

	Safe_Release(pGameInstance);
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_tInfo.vPos.x - g_iWinSizeX * 0.5f, -m_tInfo.vPos.y + g_iWinSizeY * 0.5f, 0.f));//여기가중점
	return S_OK;
}

void CSkillSlot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}
void CSkillSlot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CSkillSlot::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
	if (m_tInfo.pTarget == nullptr)
	{
		if(FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
	}	
	else//타겟을 가지고있다면 그 타겟의 렌더를 띄어야한다
	{
		m_tInfo.pTarget->Set_Info(m_tInfo);//m_tInfo.pTarget슬롯이 같고 타겟
		m_tInfo.pTarget->Set_UiMP(5);
	}
	
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkillSlot::SetUp_Components()
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

HRESULT CSkillSlot::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return S_OK;
}
HRESULT CSkillSlot::Release_RenderState()
{
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

CSkillSlot * CSkillSlot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkillSlot*	pInstance = new CSkillSlot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillSlot::Clone(void * pArg)
{
	CSkillSlot*	pInstance = new CSkillSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CSkillSlot::Get_World(void)
{
	return _float4x4();
}

void CSkillSlot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);

}
