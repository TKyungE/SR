#include "stdafx.h"
#include "..\Public\DarkShield2.h"
#include "GameInstance.h"
#include "Layer.h"

CDarkShield2::CDarkShield2(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDarkShield2::CDarkShield2(const CDarkShield2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDarkShield2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDarkShield2::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	_float3 vScale = { 0.5f,0.5f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 0;
	m_tFrame.fFrameSpeed = 0.05f;
	m_tInfo.bDead = false;
	m_tInfo.fX = 0.5f;
	m_tInfo.iDmg = 478;
	m_tInfo.iMoney = 11;

	return S_OK;
}

void CDarkShield2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Move_Frame(fTimeDelta);
	m_fCollTime += fTimeDelta;
	m_fDeadTime += fTimeDelta;
	m_ShotColl += fTimeDelta;

	if (m_ShotColl > 5.f)
	{
		m_bShot = true;
		m_ShotColl = 0.f;
		m_fDeadTime = 0.f;
		m_iShot = 0;
	}
	if (m_fDeadTime > 0.1f && m_bShot)
	{
		Create_Ball();
		m_fDeadTime = 0.f;
	}
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 1.f);
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;
	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_MONSTERSKILL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CDarkShield2"));
		return;
	}
	Safe_Release(pInstance);
}

void CDarkShield2::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Motion_Change();
	OnBillboard();
	if (m_fCollTime > 0.1f)
	{
		CheckColl();
		m_fCollTime = 0.f;
	}
	Set_Pos();
	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);
	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_ALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CDarkShield2::Render()
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
	_float fAlpha = 0.7f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;
	//	if (m_tInfo.iLevelIndex == LEVEL_MAZE)
	//	{
	_float	fMin = 13.f;
	_float	fMax = 15.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMinRange", &fMin, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxRange", &fMax, sizeof(_float))))
		return E_FAIL;
	//	}

	TextureRender();

	m_pShaderCom->Begin(6);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	if (g_bCollider)
		m_pColliderCom->Render();

	return S_OK;
}


void CDarkShield2::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 0;
			m_tFrame.fFrameSpeed = 0.05f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CDarkShield2::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		m_tFrame.iFrameStart = m_pTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}

}

HRESULT CDarkShield2::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_tFrame.iFrameStart));
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CDarkShield2::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CDarkShield2::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}


HRESULT CDarkShield2::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_DarkShield2"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}
void CDarkShield2::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale;
	vScale = { 0.2f,0.2f,0.2f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0] * vScale.z);
}
HRESULT CDarkShield2::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CDarkShield2::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CDarkShield2 * CDarkShield2::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDarkShield2*	pInstance = new CDarkShield2(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDarkShield2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDarkShield2::Clone(void* pArg)
{
	CDarkShield2*	pInstance = new CDarkShield2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDarkShield2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CDarkShield2::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CDarkShield2::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
void CDarkShield2::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
	{
		pTarget->Set_Hp(m_tInfo.iDmg);
		pTarget->Set_Hit(m_tInfo.iDmg, *(_float3*)&pTarget->Get_World().m[3][0]);
		if (pTarget->Get_Info().iHp <= 0)
			pTarget->Set_Dead();
	}

	Safe_Release(pInstance);
}

void CDarkShield2::Set_Pos()
{
	_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	_float3 vPos;

	_float3	vRight = *(_float3*)&m_tInfo.pTarget->Get_World().m[0][0];
	_float3	vUp = *(_float3*)&m_tInfo.pTarget->Get_World().m[1][0];
	_float3	vLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];

	_float4x4	RotationMatrixZ;
	D3DXMatrixRotationAxis(&RotationMatrixZ, &vLook, D3DXToRadian(-45.f));

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrixZ);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrixZ);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrixZ);

	_float4x4	RotationMatrixY;
	D3DXMatrixRotationAxis(&RotationMatrixY, &vUp, D3DXToRadian(m_fAngle +45.f));

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrixY);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrixY);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrixY);

	vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] + (*D3DXVec3Normalize(&vRight, &vRight) * -0.8f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_fAngle += 5.f;
}
void CDarkShield2::Create_Ball()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject::INFO tInfo;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.iLevelIndex = m_tInfo.pTarget->Get_Info().iLevelIndex;
	tInfo.vTargetPos = *(_float3*)&m_tInfo.pTerrain->Get_World().m[3][0];
	tInfo.iMp = 1;
	if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_DarkBall"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_MonsterSkill"), &tInfo)))
		return;
	++m_iShot;
	if (m_iShot == 9)
		m_bShot = false;
	Safe_Release(pInstance);
}