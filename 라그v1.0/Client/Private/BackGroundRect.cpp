#include "stdafx.h"
#include "..\Public\BackGroundRect.h"

#include "GameInstance.h"

CBackGroundRect::CBackGroundRect(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CBackGroundRect::CBackGroundRect(const CBackGroundRect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGroundRect::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBackGroundRect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	memcpy(&m_IndexPos, pArg, sizeof(INDEXPOS));

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	
	m_pTransformCom->Set_Scaled(m_IndexPos.vScale);

	m_IndexPos.vPos.y += 0.5f * m_IndexPos.vScale.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_IndexPos.vPos);

	return S_OK;
}

void CBackGroundRect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//OnTerrain();
}

void CBackGroundRect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	OnBillBoard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (m_iCheck == 12)		//ÆÄ¸®
	{
		m_fTime += fTimeDelta;
		if (m_fTime > 1.f / 30.f)
		{
			++m_IndexPos.iIndex;
			m_fTime = 0.f;
			if (m_IndexPos.iIndex >= 12)
			{
				m_IndexPos.iIndex = 0;
			}
		}
	}


	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CBackGroundRect::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_IndexPos.iIndex)))
		return E_FAIL;
	
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	
	m_pVIBuffer->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGroundRect::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (m_IndexPos.iIndex == 12)
	{
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGroundFly"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		m_iCheck = m_IndexPos.iIndex;
		m_IndexPos.iIndex = 0;
		m_IndexPos.vPos.y += 0.5f;
	}
	else
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGroundRect"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGroundRect::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CBackGroundRect::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	return S_OK;
}

void CBackGroundRect::OnTerrain(void)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	
	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;
	
	CTransform* pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;
	
	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

void CBackGroundRect::OnBillBoard(void)
{
	_float4x4	ViewMatrix;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);


	m_pTransformCom->Set_Scaled(m_IndexPos.vScale);
}

CBackGroundRect * CBackGroundRect::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CBackGroundRect* pInstance = new CBackGroundRect(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBackGroundRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackGroundRect::Clone(void * pArg)
{
	CBackGroundRect* pInstance = new CBackGroundRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBackGroundRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CBackGroundRect::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CBackGroundRect::Free(void)
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);
}
