#include "stdafx.h"
#include "..\Public\LogoUI.h"
#include "GameInstance.h"

CLogoUI::CLogoUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CLogoUI::CLogoUI(const CLogoUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLogoUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX;
	m_fY = g_iWinSizeY;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 9;
	m_tFrame.fFrameSpeed = 0.1f;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));
	m_pMouseTransformCom->Set_Scaled(_float3(22.f, 31.f, 1.f));
	return S_OK;
}

void CLogoUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float3 vMousePos = { (float)(ptMouse.x + 9.f) - g_iWinSizeX * 0.5f, -(float)(ptMouse.y + 13.f) + g_iWinSizeY * 0.5f, 0.f };
	m_pMouseTransformCom->Set_State(CTransform::STATE_POSITION, vMousePos);
	ShowCursor(false);
	Move_Frame(fTimeDelta);
	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

}

void CLogoUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLogoUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);


	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(m_pMouseTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pMouseTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
		return E_FAIL;
	m_pMouseBuffer->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CLogoUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_LogoUI"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), (CComponent**)&m_pMouseTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer88"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pMouseBuffer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform88"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pMouseTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoUI::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CLogoUI::Release_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CLogoUI * CLogoUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogoUI*	pInstance = new CLogoUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLogoUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLogoUI::Clone(void* pArg)
{
	CLogoUI*	pInstance = new CLogoUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLogoUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CLogoUI::Get_World(void)
{
	return _float4x4();
}

void CLogoUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMouseTextureCom);
	Safe_Release(m_pMouseTransformCom);
	Safe_Release(m_pMouseBuffer);
}
void CLogoUI::Move_Frame(_float fTimeDelta)
{
	m_tFrame.iFrameStart = m_pMouseTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
}