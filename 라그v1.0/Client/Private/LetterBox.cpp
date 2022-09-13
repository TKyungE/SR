#include "stdafx.h"
#include "..\Public\LetterBox.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"

CLetterBox::CLetterBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CLetterBox::CLetterBox(const CLetterBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLetterBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLetterBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY * 0.25f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fUpY = g_iWinSizeY * -0.125f;
	m_fDownY = g_iWinSizeY * 1.125f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pUpTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pUpTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fUpY + g_iWinSizeY * 0.5f, 0.f));

	m_pDownTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pDownTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fDownY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

void CLetterBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if ((g_iWinSizeY * 0.125f) > m_fUpY && g_bCut)
		m_fUpY += 2.f;
		
	if ((g_iWinSizeY * 0.875f) < m_fDownY && g_bCut)
		m_fDownY -= 2.f;

	if (!g_bCut)
	{
		if (m_fUpY > (g_iWinSizeY * -0.125f))
			m_fUpY -= 2.f;
		if (m_fDownY < (g_iWinSizeY * 1.125f))
			m_fDownY += 2.f;
	}

	m_pUpTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fUpY + g_iWinSizeY * 0.5f, 0.f));
	m_pDownTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fDownY + g_iWinSizeY * 0.5f, 0.f));
}

void CLetterBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && (m_fUpY > (g_iWinSizeY * -0.125f)) && (m_fDownY < (g_iWinSizeY * 1.125f)))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLetterBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pUpTransformCom->Bind_OnGraphicDev()))
	return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pUpVIBufferCom->Render();

	if (FAILED(m_pDownTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	
	m_pDownVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLetterBox::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_LetterBox"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_UpVIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pUpVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_DownVIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pDownVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_UpTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pUpTransformCom, &TransformDesc)))
	return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_DownTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pDownTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLetterBox::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CLetterBox::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CLetterBox * CLetterBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLetterBox*	pInstance = new CLetterBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLetterBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLetterBox::Clone(void * pArg)
{
	CLetterBox*	pInstance = new CLetterBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLetterBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CLetterBox::Get_World(void)
{
	return _float4x4();
}

void CLetterBox::Free()
{
	__super::Free();

	Safe_Release(m_pDownTransformCom);
	Safe_Release(m_pUpTransformCom);
	Safe_Release(m_pDownVIBufferCom);
	Safe_Release(m_pUpVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
