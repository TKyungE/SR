#include "stdafx.h"
#include "..\Public\WorldHpBar.h"
#include"GameInstance.h"


CWorldHpBar::CWorldHpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CWorldHpBar::CWorldHpBar(const CWorldHpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWorldHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldHpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));

	m_tInfo.bDead = false;
	


	return S_OK;
}

void CWorldHpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_tInfo.pTarget->Get_Info().iHp <= 0)
	{
		Set_Dead();
	}
	
	_float3 vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	vPos.y += m_tInfo.vPos.y;
	vPos.x -= 0.5f;
	m_pTransformCom2->Set_Scaled({ 1.f, 0.1f, 1.f });
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	m_fSizeX = m_tInfo.pTarget->Get_Info().iHp /(float)m_tInfo.pTarget->Get_Info().iMaxHp;

	m_pTransformCom->Set_Scaled({m_fSizeX, 0.1f, 1.f });
	_float fX = (1.f - m_fSizeX) / 2.f;
	vPos.z -= 0.001f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	


}
void CWorldHpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	OnBillboard();
	if (nullptr != m_pRendererCom && 0 == g_iCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CWorldHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(2)))
		return E_FAIL;
	m_pVIBufferCom2->Render();
	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CWorldHpBar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_WorldHpBar"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldHpBar::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CWorldHpBar::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CWorldHpBar::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	
	_float3 vScale = { 1.f,0.1f,1.f };
	m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom2->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom2->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
	vScale = { m_fSizeX,0.1f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CWorldHpBar * CWorldHpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWorldHpBar*	pInstance = new CWorldHpBar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CIconBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWorldHpBar::Clone(void * pArg)
{
	CWorldHpBar*	pInstance = new CWorldHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CIconBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CWorldHpBar::Get_World(void)
{
	return _float4x4();
}

void CWorldHpBar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
