#include "stdafx.h"
#include "..\Public\Fade.h"
#include "GameInstance.h"

CFade::CFade(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFade::CFade(const CFade & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFade::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFade::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_tInfo.bDead = false;
	m_tInfo.bHit = false;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (m_tInfo.iMp == 0)
		m_bCheck = false;
	
	else
		m_bCheck = true;
	

	m_fSizeX = (_float)g_iWinSizeX * 2.f;
	m_fSizeY = (_float)g_iWinSizeY * 2.f;
	m_fX = ((_float)g_iWinSizeX - ((_float)g_iWinSizeX * 2.f)) * 0.5f;
	m_fY = (_float)g_iWinSizeY* 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	if (!m_bCheck)
		m_fAlpha = 0.f;
	else
		m_fAlpha = 1.f;

	return S_OK;
}

void CFade::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	RECT	rcRect;

	SetRect(&rcRect, _int(m_fX - m_fSizeX * 0.5f), _int(m_fY - m_fSizeY * 0.5f), _int(m_fX + m_fSizeX * 0.5f), _int(m_fY + m_fSizeY * 0.5f));
}

void CFade::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!m_bCheck && m_tInfo.iMaxMp != 1)
	{
		m_fAlpha += fTimeDelta * 1.2f;

		if (m_fAlpha >= 1.f)
		{
			m_bCheck = true;
			m_tInfo.iMaxMp = 1;
		}
	}
	else if (m_bCheck && m_tInfo.iMaxMp != 1)
	{
		m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= -1.f)
		{
			m_bCheck = false;
			m_tInfo.iMaxMp = 1;
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MOUSE, this);
}

HRESULT CFade::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	D3DXMatrixIdentity(&ViewMatrix);
	
	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix,&WorldMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));

	m_pShaderCom->Begin(10);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	return S_OK;
}

HRESULT CFade::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Alpha"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom)))
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


CFade * CFade::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFade*	pInstance = new CFade(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayerMp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFade::Clone(void* pArg)
{
	CFade*	pInstance = new CFade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayerMp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CFade::Get_World(void)
{
	return _float4x4();
}

void CFade::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
