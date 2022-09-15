#include "stdafx.h"
#include "..\Public\NextBtn.h"
#include "GameInstance.h"

CNextBtn::CNextBtn(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMyButton(pGraphic_Device)
{
}

CNextBtn::CNextBtn(const CNextBtn & rhs)
	: CMyButton(rhs)
{
}

HRESULT CNextBtn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextBtn::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tBInfo.pOut = this;
	memcpy(pArg, &m_tBInfo, sizeof(BINFO));

	m_eType = BUTTON_NEXT;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	SetRect(&m_rcBtn, (int)(m_fX - m_fSizeX * 0.35f), (int)(m_fY - m_fSizeY * 0.15f), (int)(m_fX + m_fSizeX * 0.35f), (int)(m_fY + m_fSizeY * 0.15f));

	return S_OK;
}

void CNextBtn::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&m_rcBtn, ptMouse))
	{
		if (CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON) && m_bMouseCheck)
		{
			m_bClicked = true;
			if (g_bCut)
				g_bCut = false;

			m_iTex = 2;
		}
		else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON))
		{
			m_bMouseCheck = true;
			m_iTex = 2;
		}
		else
			m_iTex = 1;
	}
	else
	{
		m_iTex = 0;
		m_bMouseCheck = false;
	}
}

void CNextBtn::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CNextBtn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_iTex)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(Off_SamplerState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(On_SamplerState()))
		return E_FAIL;

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNextBtn::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Next_Button"), (CComponent**)&m_pTextureCom)))
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

CNextBtn * CNextBtn::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNextBtn*	pInstance = new CNextBtn(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNextBtn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNextBtn::Clone(void * pArg)
{
	CNextBtn*	pInstance = new CNextBtn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNextBtn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CNextBtn::Get_World(void)
{
	return _float4x4();
}

void CNextBtn::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
