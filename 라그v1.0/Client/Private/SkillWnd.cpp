#include "stdafx.h"
#include "..\Public\SkillWnd.h"
#include "GameInstance.h"


CSkillWnd::CSkillWnd(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CSkillWnd::CSkillWnd(const CSkillWnd & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkillWnd::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillWnd::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_fSizeX = 600.0f;
	m_fSizeY = 340.0f;
	m_fX = 700.f;
	m_fY = 300.f;

	m_tInfo.vPos.x = m_fX + 288;
	m_tInfo.vPos.y = m_fY - 158;
	m_tInfo.bHit = false;
	m_tInfo.pTarget = this;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_XBox"),m_tInfo.iLevelIndex, TEXT("Layer_UI"), &m_tInfo);
	Safe_Release(pGameInstance);


	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CSkillWnd::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);
	if (m_tInfo.bHit == true)
	{
		Set_Dead();
	}
	RECT		rcRect;
	SetRect(&rcRect,(int)( m_fX - m_fSizeX * 0.5f),(int)( m_fY - m_fSizeY * 0.5f),(int)( m_fX + m_fSizeX * 0.5f),(int)( m_fY - m_fSizeY * 0.35f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{

	}

}
void CSkillWnd::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));//여기가중점
}

HRESULT CSkillWnd::Render()
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

HRESULT CSkillWnd::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillWnd"), (CComponent**)&m_pTextureCom)))
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

HRESULT CSkillWnd::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}
HRESULT CSkillWnd::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CSkillWnd * CSkillWnd::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkillWnd*	pInstance = new CSkillWnd(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillWnd::Clone(void * pArg)
{
	CSkillWnd*	pInstance = new CSkillWnd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CSkillWnd::Get_World(void)
{
	return _float4x4();
}

void CSkillWnd::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

