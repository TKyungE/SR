#include "stdafx.h"
#include "..\Public\Village_Chief.h"
#include "GameInstance.h"

CVillage_Chief::CVillage_Chief(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CVillage_Chief::CVillage_Chief(const CVillage_Chief& rhs)
	: CGameObject(rhs)
{
}

HRESULT CVillage_Chief::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVillage_Chief::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	_float3 vScale = { 1.f,1.f,1.f };
	m_pTransformCom->Set_Scaled(vScale);

	m_tInfo.vPos.y += 0.5f * vScale.y;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_tInfo.bDead = false;
	m_tInfo.fX = 0.1f;
	m_tFrame.iFrameStart = 0;
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 0.7f,0.7f,1.f };

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
	Safe_Release(pGameInstance);
	return S_OK;
}

void CVillage_Chief::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//OnTerrain();

}

void CVillage_Chief::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	OnBillboard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CVillage_Chief::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	Off_SamplerState();
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tInfo.iMp)))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBuffer->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;
	On_SamplerState();
	return S_OK;
}

HRESULT CVillage_Chief::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVillage_Chief::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CVillage_Chief::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}
HRESULT CVillage_Chief::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CVillage_Chief::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
void CVillage_Chief::OnTerrain(void)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform* pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.4f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

void CVillage_Chief::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale = { 1.f,1.f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

CVillage_Chief * CVillage_Chief::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CVillage_Chief* pInstance = new CVillage_Chief(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVillage_Chief::Clone(void * pArg)
{
	CVillage_Chief* pInstance = new CVillage_Chief(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVillage_Chief"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CVillage_Chief::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();;
}

void CVillage_Chief::Free(void)
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);

}