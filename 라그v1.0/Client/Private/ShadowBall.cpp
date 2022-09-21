#include "stdafx.h"
#include "..\Public\ShadowBall.h"
#include "GameInstance.h"

CShadowBall::CShadowBall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CShadowBall::CShadowBall(const CShadowBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShadowBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShadowBall::Initialize(void* pArg)
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


	_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vLook = m_tInfo.vTargetPos - vPosition;

	return S_OK;
}

void CShadowBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Move_Frame(fTimeDelta);
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 7.0f)
		Set_Dead();

	_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition += *D3DXVec3Normalize(&vLook, &vLook) * 5.f * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.7f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_MONSTERSKILL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CShadowBall"));
		return;
	}

	Safe_Release(pInstance);
}

void CShadowBall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Motion_Change();
	OnBillboard();
	CheckColl();

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

HRESULT CShadowBall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;


	//상태에 따라 바인드하는 함수
	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();
	if (g_bCollider)
		m_pColliderCom->Render();
	return S_OK;
}


void CShadowBall::Motion_Change()
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

void CShadowBall::Move_Frame(_float fTimeDelta)
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

HRESULT CShadowBall::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CShadowBall::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CShadowBall::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}


HRESULT CShadowBall::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShadowBall"), (CComponent**)&m_pTextureCom)))
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


	return S_OK;
}
void CShadowBall::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScale;
	vScale = { 0.5f,0.5f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]* vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
HRESULT CShadowBall::SetUp_RenderState()
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

HRESULT CShadowBall::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CShadowBall * CShadowBall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShadowBall*	pInstance = new CShadowBall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShadowBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShadowBall::Clone(void* pArg)
{
	CShadowBall*	pInstance = new CShadowBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShadowBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CShadowBall::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CShadowBall::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
void CShadowBall::CheckColl()
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
		Set_Dead();
	}

	Safe_Release(pInstance);
}
