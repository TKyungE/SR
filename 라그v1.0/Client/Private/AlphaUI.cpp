#include "stdafx.h"
#include "..\Public\AlphaUI.h"
#include "GameInstance.h"

CAlphaUI::CAlphaUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CAlphaUI::CAlphaUI(const CAlphaUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAlphaUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlphaUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	m_tInfo.bDead = false;
	m_tInfo.bHit = false;
	m_tInfo.iMp = 0;		// 이걸로 계속 중첩으로 맞았는지 검사할 거임
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fTime = 0.1f;

	m_fSizeX = (_float)g_iWinSizeX * 1.2f;
	m_fSizeY = (_float)g_iWinSizeY * 1.2f;
	m_fX = (g_iWinSizeX - (g_iWinSizeX * 1.2f)) * 0.5f;
	m_fY = (_float)g_iWinSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

void CAlphaUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	RECT	rcRect;

	SetRect(&rcRect, _int(m_fX - m_fSizeX * 0.5f), _int(m_fY - m_fSizeY * 0.5f), _int(m_fX + m_fSizeX * 0.5f), _int(m_fY + m_fSizeY * 0.5f));
}

void CAlphaUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_fTimeDelta += fTimeDelta;

	if (m_tInfo.iMp == 1)
	{
		m_fTime = 0.001f;
		m_fAlpha = 0.f;
		m_bCheck = false;

		m_tInfo.iMp = 0;
	}



	if (m_fTimeDelta > m_fTime && !m_bCheck && m_tInfo.bHit)
	{
		m_fAlpha += 0.01f;
		m_fTimeDelta = 0.f;
		m_fTime -= 0.05f;
		if (m_fAlpha >= 1.f)
		{
			m_fTime = 0.1f;
			m_fAlpha = 0.f;
			m_bCheck = true;
		}
	}
	else if (m_fTimeDelta > m_fTime && m_bCheck && m_tInfo.bHit)
	{
		m_fAlpha -= 0.1f;
		m_fTimeDelta = 0.f;
		m_fTime -= 0.1f;
		if (m_fAlpha <= 0.f)
		{
			m_fTime = 0.001f;
			m_fAlpha = 0.f;
			m_tInfo.bHit = false;
			m_bCheck = false;
		}
	}


	if (nullptr != m_pRendererCom && m_tInfo.bHit && m_fAlpha != 0.f)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CAlphaUI::Render()
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

	m_pShaderCom->Begin(5);

	/*if (FAILED(SetUp_RenderState()))
		return E_FAIL;*/

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	/*if (FAILED(Release_RenderState()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CAlphaUI::SetUp_Components()
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

HRESULT CAlphaUI::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CAlphaUI::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CAlphaUI * CAlphaUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAlphaUI*	pInstance = new CAlphaUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayerMp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlphaUI::Clone(void* pArg)
{
	CAlphaUI*	pInstance = new CAlphaUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayerMp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CAlphaUI::Get_World(void)
{
	return _float4x4();
}

void CAlphaUI::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
