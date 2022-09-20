#include "stdafx.h"
#include "..\Public\UseSkill.h"
#include "GameInstance.h"

CUseSkill::CUseSkill(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUseSkill::CUseSkill(const CUseSkill & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUseSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUseSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_tInfo.vPos.y += 0.01f;
	_float3 vScale = { 2.f,2.f,1.f };
	_float3 vRight = { 1.f,0.f,0.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Turn(vRight, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 0;
	m_tFrame.fFrameSpeed = 0.1f;
	m_tInfo.bDead = false;

	return S_OK;
}

void CUseSkill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	LockPos();
	Move_Frame(fTimeDelta);
	
	
}

void CUseSkill::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	Motion_Change();

	//_float4x4		ViewMatrix;

	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	//D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	////m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CUseSkill::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;


	//���¿� ���� ���ε��ϴ� �Լ�
	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();
	return S_OK;
}


void CUseSkill::Motion_Change()
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

void CUseSkill::Move_Frame(_float fTimeDelta)
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

HRESULT CUseSkill::TextureRender()
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

HRESULT CUseSkill::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CUseSkill::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

void CUseSkill::LockPos()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	pVIBuffer_Terrain->Picking(pTransform_Terrain->Get_WorldMatrix(), &m_tInfo.vPos);

	m_tInfo.vPos.y += 0.01f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	Safe_Release(pGameInstance);
}


HRESULT CUseSkill::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC ,TEXT("Prototype_Component_Texture_UseSkill"), (CComponent**)&m_pTextureCom)))
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

HRESULT CUseSkill::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);



	return S_OK;
}

HRESULT CUseSkill::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

CUseSkill * CUseSkill::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUseSkill*	pInstance = new CUseSkill(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUseSkill::Clone(void* pArg)
{
	CUseSkill*	pInstance = new CUseSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CUseSkill::Get_World(void)
{
	return _float4x4();
}

void CUseSkill::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
