#include "stdafx.h"
#include "..\Public\ExpBar.h"
#include"GameInstance.h"

CExpBar::CExpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CExpBar::CExpBar(const CExpBar & rhs)
	: CGameObject(rhs)
{
}
HRESULT CExpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = (float)m_tInfo.pTarget->Get_Info().iExp;//나중에 늘어나고 줄어듦을 여기서 조절한다
	m_fSizeY = 8.f;
	m_fX = 20.f;
	m_fY = 715.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CExpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	m_fSizeX = (float)m_tInfo.pTarget->Get_Info().iExp;


	if (m_fSizeX <= 0.f)
	{
		m_fSizeX = 0.00001f;
	}



	m_pTransformCom->Set_Scaled({ m_fSizeX, m_fSizeY, 1.f });

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CExpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}


HRESULT CExpBar::Render()
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

	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CExpBar::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ExpBar"), (CComponent**)&m_pTextureCom)))
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

HRESULT CExpBar::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CExpBar::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CExpBar * CExpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CExpBar*	pInstance = new CExpBar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CExpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExpBar::Clone(void * pArg)
{
	CExpBar*	pInstance = new CExpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpBar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

_float4x4 CExpBar::Get_World(void)
{
	return _float4x4();
}
