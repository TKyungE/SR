#include "stdafx.h"
#include "..\Public\WorldHpBar.h"
#include"GameInstance.h"
#include "Layer.h"

CWorldHpBar::CWorldHpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CWorldHpBar::CWorldHpBar(const CWorldHpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWorldHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldHpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));

	m_tInfo.bDead = false;

	m_pTransformCom->Set_Scaled(_float3(1.f, 2.f, 1.f));
	m_pTransformCom2->Set_Scaled(_float3(1.f, 1.f, 1.f));

	return S_OK;
}

void CWorldHpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_tInfo.pTarget->Get_Info().iHp <= 0)
	{
		Set_Dead();
	}

	_float3 vPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	vPos.y += 0.5f;

	if (m_tInfo.iMonsterType == MON_BIGFOOT)
	{
		vPos.y += 0.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_tInfo.iMonsterType == MON_TOTEM)
	{
		vPos.y += 2.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_tInfo.iMonsterType == MON_BAPHOMET)
	{
		vPos.y += 0.2f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_tInfo.iMonsterType == MON_MINOROUS)
	{
		vPos.y += 0.3f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_tInfo.iMonsterType == MON_WRAITH)
	{
		vPos.y += 0.3f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);
	}




}
void CWorldHpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	OnBillboard();

	if (nullptr != m_pRendererCom && 0 == g_iCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWorldHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix, PlayerWorldMatrix;
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

	_float fHpBar = (m_tInfo.pTarget->Get_Info().iHp / (float)m_tInfo.pTarget->Get_Info().iMaxHp);

	m_pShaderCom->Set_RawValue("g_fHPBar", &fHpBar, sizeof(_float));
	_float fAlpha = 1.f;

	m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float));
	if (m_tInfo.iLevelIndex != LEVEL_MAZE)
	{
		_float fMin = 113.f;
		_float fMax = 116.f;
		m_pShaderCom->Set_RawValue("g_fMinRange", &fMin, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxRange", &fMax, sizeof(_float));

	}
	else
	{
		_float	fMin = 1.f;
		_float	fMax = 4.f;


		if (FAILED(m_pShaderCom->Set_RawValue("g_fMinRange", &fMin, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxRange", &fMax, sizeof(_float))))
			return E_FAIL;
	}

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(2));

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pShaderCom->Begin(7);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	return S_OK;
}

HRESULT CWorldHpBar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_WorldHpBar"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldHpBar::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	/*m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);*/
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CWorldHpBar::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CWorldHpBar::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);


	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);

	//m_pTransformCom2->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);

	m_pTransformCom2->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);

	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	m_pTransformCom->Set_Scaled(_float3(1.f, 2.f, 1.f));
}

CWorldHpBar * CWorldHpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWorldHpBar*	pInstance = new CWorldHpBar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CIconBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWorldHpBar::Clone(void * pArg)
{
	CWorldHpBar*	pInstance = new CWorldHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CIconBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CWorldHpBar::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CWorldHpBar::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
