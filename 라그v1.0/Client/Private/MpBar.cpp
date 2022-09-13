#include "stdafx.h"
#include "..\Public\MpBar.h"

#include "GameInstance.h"


CMpBar::CMpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CMpBar::CMpBar(const CMpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = (float)m_tInfo.pTarget->Get_Info().iMp;//나중에 늘어나고 줄어듦을 여기서 조절한다
	m_fSizeY = 16.0f;
	m_fX = 140.f;
	m_fY = 114.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	
	return S_OK;
}

void CMpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fSizeX = (float)m_tInfo.pTarget->Get_Info().iMp;

	if (m_fSizeX >= 186.0f)
	{
		m_fSizeX = 186.0;
	}

	if (m_fSizeX <= 0.f)
	{
		m_fSizeX = 0.00001f;
	}

	m_fX = m_fSizeX *0.5f + 47.f;

	m_pTransformCom->Set_Scaled({ m_fSizeX, m_fSizeY, 1.f });

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CMpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CMpBar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_MpBar"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));


	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMpBar::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CMpBar::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CMpBar * CMpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMpBar*	pInstance = new CMpBar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CExpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMpBar::Clone(void * pArg)
{
	CMpBar*	pInstance = new CMpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CExpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMpBar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

_float4x4 CMpBar::Get_World(void)
{
	return _float4x4();
}
