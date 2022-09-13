#include "stdafx.h"
#include "..\Public\Wing.h"
#include "GameInstance.h"
#include "Player.h"
CWing::CWing(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWing::CWing(const CWing & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	
	m_pTransformCom->Set_Scaled(m_tInfo.vPos);
	m_pTransformCom2->Set_Scaled(m_tInfo.vPos);

	m_OriginvRight = *(_float3*)&m_tInfo.pTarget->Get_World().m[0][0];
	m_OriginvLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 0;
	m_tFrame.fFrameSpeed = 0.1f;
	m_tInfo.bDead = false;
	m_bTurn = false;
	m_fAngle = D3DXToRadian(135.f);
	return S_OK;
}

void CWing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CWing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Get_Fly() == false)
	{
		Set_Dead();
		return;
	}
	Set_TargetPos();
	WingTurn(fTimeDelta);
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale = { 0.5f,0.5f,1.f };
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CWing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();
	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;
	m_pVIBufferCom2->Render();
	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();
	return S_OK;
}


void CWing::Motion_Change()
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

void CWing::Move_Frame(_float fTimeDelta)
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

void CWing::Set_TargetPos()
{
	if (m_tInfo.pTarget->Get_Info().iHp <= 0 || m_tInfo.pTarget->Get_Info().bDead)
	{
		Set_Dead();
	}
	_float3 vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	if (dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Get_Front() == false)
		vPos.z -= 0.1f;
	else
		vPos.z += 0.1f;
	vPos.y += 0.2f;
//	vPos.x -= 0.2f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
//	vPos.x += 0.4f;
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);

}

void CWing::WingTurn(_float fTimeDelta)
{
	m_TurnTime += fTimeDelta;
	if (m_TurnTime > 0.5f)
	{
		if (!m_bTurn)
		{
			m_bTurn = true;
			vfTurn = -1.f;
		}
		else
		{
			m_bTurn = false;
			vfTurn = 1.f;
		}
		m_TurnTime = 0.f;
	}


	if (dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Get_Front() == true)
	{
		if (m_bFront)
		{
			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_OriginvRight);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_OriginvLook);
			m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, m_OriginvRight);
			m_pTransformCom2->Set_State(CTransform::STATE_LOOK, m_OriginvLook);
			m_bFront = false;
			m_pTransformCom->Set_Scaled(m_tInfo.vPos);
			m_pTransformCom2->Set_Scaled(m_tInfo.vPos);
		}

		_float3	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_float3	vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		_float3	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 vUpTurn = { 0.f,1.f,0.f };
		_float4x4	RotationMatrix;
		D3DXMatrixRotationAxis(&RotationMatrix, &vUpTurn, -m_fAngle * (fTimeDelta * vfTurn));

		D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
		D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
		D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
		////////////////////////////////////////////////////
		_float3	vRight2 = m_pTransformCom2->Get_State(CTransform::STATE_RIGHT);
		_float3	vUp2 = m_pTransformCom2->Get_State(CTransform::STATE_UP);
		_float3	vLook2 = m_pTransformCom2->Get_State(CTransform::STATE_LOOK);
		_float3 vUpTurn2 = { 0.f,-1.f,0.f };
		_float4x4	RotationMatrix2;
		D3DXMatrixRotationAxis(&RotationMatrix2, &vUpTurn2, -m_fAngle * (fTimeDelta * vfTurn));

		D3DXVec3TransformNormal(&vRight2, &vRight2, &RotationMatrix2);
		D3DXVec3TransformNormal(&vUp2, &vUp2, &RotationMatrix2);
		D3DXVec3TransformNormal(&vLook2, &vLook2, &RotationMatrix2);

		m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, vRight2);
		m_pTransformCom2->Set_State(CTransform::STATE_UP, vUp2);
		m_pTransformCom2->Set_State(CTransform::STATE_LOOK, vLook2);
	}

	else
	{
		if (!m_bFront)
		{
			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_OriginvRight);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_OriginvLook);
			m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, m_OriginvRight);
			m_pTransformCom2->Set_State(CTransform::STATE_LOOK, m_OriginvLook);
			m_bFront = true;
			m_pTransformCom->Set_Scaled(m_tInfo.vPos);
			m_pTransformCom2->Set_Scaled(m_tInfo.vPos);
		}
		_float3	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		_float3	vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		_float3	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 vUpTurn = { 0.f,-1.f,0.f };
		_float4x4	RotationMatrix;
		D3DXMatrixRotationAxis(&RotationMatrix, &vUpTurn, m_fAngle * (fTimeDelta * vfTurn));

		D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
		D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
		D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
		////////////////////////////////////////////////////
		_float3	vRight2 = m_pTransformCom2->Get_State(CTransform::STATE_RIGHT);
		_float3	vUp2 = m_pTransformCom2->Get_State(CTransform::STATE_UP);
		_float3	vLook2 = m_pTransformCom2->Get_State(CTransform::STATE_LOOK);
		_float3 vUpTurn2 = { 0.f,1.f,0.f };
		_float4x4	RotationMatrix2;
		D3DXMatrixRotationAxis(&RotationMatrix2, &vUpTurn2, m_fAngle * (fTimeDelta * vfTurn));

		D3DXVec3TransformNormal(&vRight2, &vRight2, &RotationMatrix2);
		D3DXVec3TransformNormal(&vUp2, &vUp2, &RotationMatrix2);
		D3DXVec3TransformNormal(&vLook2, &vLook2, &RotationMatrix2);

		m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, vRight2);
		m_pTransformCom2->Set_State(CTransform::STATE_UP, vUp2);
		m_pTransformCom2->Set_State(CTransform::STATE_LOOK, vLook2);







	}




}


HRESULT CWing::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CWing::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}


HRESULT CWing::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wing"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectLeft"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 0.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(150.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWing::SetUp_RenderState()
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

HRESULT CWing::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CWing * CWing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWing*	pInstance = new CWing(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWing::Clone(void* pArg)
{
	CWing*	pInstance = new CWing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CWing::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CWing::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
