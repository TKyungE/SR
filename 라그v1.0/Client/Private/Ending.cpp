#include "stdafx.h"
#include "..\Public\Ending.h"

#include "GameInstance.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CEnding::CEnding(const CEnding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnding::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CEnding::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(200.f, 200.f, 200.f));
	m_pTransformCom2->Set_Scaled(_float3(200.f, 200.f, 200.f));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, _float3(1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, _float3(0.f, 0.f, 1.f));

	m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, -0.5f, -0.5f));

	m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, _float3(1.f, 0.f, 0.f));
	m_pTransformCom2->Set_State(CTransform::STATE_UP, _float3(0.f, 1.f, 0.f));
	m_pTransformCom2->Set_State(CTransform::STATE_LOOK, _float3(0.f, 0.f, 1.f));

	m_pTransformCom2->Turn(_float3(1.f, 0.f, 0.f), 1.f);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.z -= 0.8f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);


	return S_OK;
}

void CEnding::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.z += fTimeDelta * 0.1f;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	vPos = m_pTransformCom2->Get_State(CTransform::STATE_POSITION);
	vPos.z += fTimeDelta * 0.1f;

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);

}

void CEnding::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CEnding::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
	
	m_pShaderCom->Begin(0);

	m_pVIBuffer->Render();

	m_pShaderCom->End();

	WorldMatrix = m_pTransformCom2->Get_WorldMatrix();

	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(1));

	m_pShaderCom->Begin(0);

	m_pVIBuffer2->Render();

	m_pShaderCom->End();
	return S_OK;
}

HRESULT CEnding::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer2)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ending"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CEnding * CEnding::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CEnding* pInstance = new CEnding(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBackGroundRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnding::Clone(void * pArg)
{
	CEnding* pInstance = new CEnding(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBackGroundRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CEnding::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CEnding::Free(void)
{
	__super::Free();
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBuffer2);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);
}
