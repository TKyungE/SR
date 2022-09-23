#include "stdafx.h"
#include "..\Public\ThunderTarget.h"
#include "GameInstance.h"
#include "Layer.h"
#include "SoundMgr.h"

CThunderTarget::CThunderTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CThunderTarget::CThunderTarget(const CThunderTarget & rhs)
	: CGameObject(rhs)
{
}

HRESULT CThunderTarget::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThunderTarget::Initialize(void* pArg)
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
	m_fDeadTime = 0.f;
	return S_OK;
}

void CThunderTarget::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fDeadTime += fTimeDelta;
	m_fSkillCool += fTimeDelta;
	Move_Frame(fTimeDelta);
}

void CThunderTarget::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Motion_Change();
	Set_TargetPos();
	if (3.f < m_fDeadTime)
		Set_Dead();


	if (m_fSkillCool > 0.3f)
	{
		_float MinDist = 100000.f;
		_float HelpDistance = 0.f;
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		if (pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_Monster")) != nullptr && pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_SkyBoss")) != nullptr)
		{
			for (auto& iter : pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_Monster"))->Get_Objects())
			{
				_float3 Target = *(_float3*)&iter->Get_World().m[3][0];
				if (iter->Get_Info().iHp <= 0)
					continue;
				HelpDistance = D3DXVec3Length(&(Target - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
				if (HelpDistance <= 0.f)
					continue;
				if (MinDist > HelpDistance)
				{
					MinDist = HelpDistance;
					tInfo.pTarget = iter;
				}
			}
			_float3 vTarget = *(_float3*)&pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_SkyBoss"))->Get_Objects().front()->Get_World().m[3][0];
			
			HelpDistance = D3DXVec3Length(&(vTarget - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			if (MinDist > HelpDistance)
			{
				MinDist = HelpDistance;
				tInfo.pTarget = pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_SkyBoss"))->Get_Objects().front();
			}
		}

		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		tInfo.iLevelIndex = LEVEL_SKY;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkyThunder"), LEVEL_SKY, TEXT("Layer_Skill"), &tInfo)))
			return;
		CSoundMgr::Get_Instance()->PlayEffect(L"SkyThunder.wav", fSOUND -0.05f);
		Safe_Release(pGameInstance);
		m_fSkillCool = 0.f;
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CThunderTarget::Render()
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


void CThunderTarget::Motion_Change()
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

void CThunderTarget::Move_Frame(_float fTimeDelta)
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

HRESULT CThunderTarget::TextureRender()
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

HRESULT CThunderTarget::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CThunderTarget::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CThunderTarget::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossSkillTarget"), (CComponent**)&m_pTextureCom)))
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

HRESULT CThunderTarget::SetUp_RenderState()
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

HRESULT CThunderTarget::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

CThunderTarget * CThunderTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CThunderTarget*	pInstance = new CThunderTarget(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CThunderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CThunderTarget::Clone(void* pArg)
{
	CThunderTarget*	pInstance = new CThunderTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CThunderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CThunderTarget::Get_World(void)
{
	return _float4x4();
}

void CThunderTarget::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
void CThunderTarget::Set_vPos()
{
	m_tInfo.bDead = false;
	m_pTransformCom->Set_Scaled({ 2.f,2.f,1.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


}
void CThunderTarget::Set_TargetPos()
{
	_float3 vScale = { 1.f,1.f,1.f };
	_float3 vTargetPos, vTargetLook, vTargetUp,vTargetRight, vPos, vRight;
	vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];
	vTargetLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[2][0];
	vTargetUp = *(_float3*)&m_tInfo.pTarget->Get_World().m[1][0];
	vTargetRight = *(_float3*)&m_tInfo.pTarget->Get_World().m[0][0];
	D3DXVec3Normalize(&vTargetPos, &vTargetPos);
	D3DXVec3Normalize(&vTargetLook, &vTargetLook);
	D3DXVec3Normalize(&vTargetUp, &vTargetUp);
	D3DXVec3Normalize(&vTargetRight, &vTargetRight);
	vTargetPos *= -0.01f;
	vPos += vTargetPos;
	if (m_tInfo.iMp == 1)
	{
		vPos += vTargetUp * 0.6f;
		vPos += vTargetRight * 0.4f;
	}
	else
	{
		vPos += vTargetUp * 0.6f;
		vPos -= vTargetRight * 0.4f;
	}
	D3DXVec3Cross(&vRight, &vTargetUp, &vTargetLook);


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vTargetUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vTargetLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}