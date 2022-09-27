#include "stdafx.h"
#include "..\Public\Portal.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Layer.h"

CPortal::CPortal(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CPortal::CPortal(const CPortal& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPortal::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortal::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_Scaled(m_tInfo.vScale);
	if (m_tInfo.iNextLevel == LEVEL_CHOBOFIELD)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 1);
	}
	m_tInfo.vPos.y += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_tInfo.bDead = false;
	m_tInfo.fX = 0.1f;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 23;
	m_tFrame.fFrameSpeed = 0.05f;
	m_bLevel = false;

	return S_OK;
}

void CPortal::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	OnTerrain();
	Move_Frame(fTimeDelta);

	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_PORTAL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CPortal"));
		return;
	}

	Safe_Release(pInstance);
}

void CPortal::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//OnBillboard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;
	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_ALPHABLEND, this);
	}

	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
		m_bLevel = true;

	Safe_Release(pInstance);
}

HRESULT CPortal::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix, PlayerWorldMatrix;
	_float4			vCamPosition;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	CLayer* pLayer = pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Player"));

	list<class CGameObject*> GameObject = pLayer->Get_Objects();

	PlayerWorldMatrix = GameObject.front()->Get_World();

	memcpy(&vCamPosition, &PlayerWorldMatrix.m[3][0], sizeof(_float4));
	Safe_Release(pInstance);
	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;
	_float fAlpha = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;
	//	if (m_tInfo.iLevelIndex == LEVEL_MAZE)
	//	{
	_float	fMin = 130.f;
	_float	fMax = 150.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMinRange", &fMin, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxRange", &fMax, sizeof(_float))))
		return E_FAIL;
	//	}

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_tFrame.iFrameStart));

	m_pShaderCom->Begin(6);

	m_pVIBuffer->Render();

	m_pShaderCom->End();

	if (g_bCollider)
		m_pColliderCom->Render();
	
	return S_OK;
}

HRESULT CPortal::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Portal"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPortal::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CPortal::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}
HRESULT CPortal::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CPortal::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
void CPortal::OnTerrain(void)
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

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.8f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

void CPortal::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale = { 2.f,2.f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
void CPortal::Move_Frame(_float fTimeDelta)
{
	m_tFrame.iFrameStart = m_pTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);


}
CPortal * CPortal::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CPortal* pInstance = new CPortal(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPortal::Clone(void * pArg)
{
	CPortal* pInstance = new CPortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CPortal::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();;
}

void CPortal::Free(void)
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}