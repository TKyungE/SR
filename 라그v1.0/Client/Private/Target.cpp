#include "stdafx.h"
#include "..\Public\Target.h"
#include "GameInstance.h"
#include "Layer.h"

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTarget::CTarget(const CTarget & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTarget::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTarget::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Set_vPos();

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 0;
	m_tFrame.fFrameSpeed = 0.1f;
	m_tInfo.bDead = false;
	
	return S_OK;
}

void CTarget::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	Move_Frame(fTimeDelta);

	Set_TargetPos();
}

void CTarget::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Motion_Change();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTarget::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;


	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();
	return S_OK;
}


void CTarget::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 0;
			m_tFrame.fFrameSpeed = 0.1f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CTarget::Move_Frame(_float fTimeDelta)
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

HRESULT CTarget::TextureRender()
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

HRESULT CTarget::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CTarget::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CTarget::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Target"), (CComponent**)&m_pTextureCom)))
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

HRESULT CTarget::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);



	return S_OK;
}

HRESULT CTarget::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTarget*	pInstance = new CTarget(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTarget::Clone(void* pArg)
{
	CTarget*	pInstance = new CTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CTarget::Get_World(void)
{
	return _float4x4();
}

void CTarget::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
void CTarget::Set_vPos()
{
	m_tInfo.bDead = false;
	m_pTransformCom->Set_Scaled({ 1.f,1.f,1.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


}
void CTarget::Set_TargetPos()
{
	_float3 vScale = { 0.5f,0.5f,1.f };
	_float3 vTargetPos, vTargetLook, vTargetUp, vTargetRight, vPos, vRight;
	vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];
	vTargetLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];
	vTargetUp = *(_float3*)&m_tInfo.pTarget->Get_World().m[1][0];
	vTargetRight = *(_float3*)&m_tInfo.pTarget->Get_World().m[0][0];
	D3DXVec3Normalize(&vTargetPos, &vTargetPos);
	D3DXVec3Normalize(&vTargetLook, &vTargetLook);
	D3DXVec3Normalize(&vTargetUp, &vTargetUp);
	D3DXVec3Normalize(&vTargetRight, &vTargetRight);
	vTargetPos *= 2.f;
	vPos += vTargetPos;
	//vPos += -vTargetUp * 1.5f;
	D3DXVec3Cross(&vRight, &vTargetUp, &vTargetLook);


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vTargetUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTargetLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}