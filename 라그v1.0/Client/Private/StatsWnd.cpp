#include "stdafx.h"
#include "..\Public\StatsWnd.h"
#include"GameInstance.h"


CStatsWnd::CStatsWnd(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CStatsWnd::CStatsWnd(const CStatsWnd & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStatsWnd::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatsWnd::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_fSizeX = 280.0f;
	m_fSizeY = 103.0f;
	m_fX = 300.f;
	m_fY = 400.f;

	m_tInfo.vPos.x = m_fX + 128;
	m_tInfo.vPos.y = m_fY - 78;
	m_tInfo.bHit = false;
	m_tInfo.pTarget = this;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_XBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &m_tInfo);
	//pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlusBox"), LEVEL_STATIC, TEXT("Layer_UI"), &m_StrPlus);
	Safe_Release(pGameInstance);


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));//여기가중점
	return S_OK;
}

void CStatsWnd::Tick(_float fTimeDelta)
{
	if (m_tInfo.bHit == true)
	{
		Set_Dead();
	}
	if (m_StrPlus.bNext == true)
	{
		m_Str += 1;
		_tchar m_szFPS[MAX_PATH] = L"";
		wsprintf(m_szFPS, L"STR : %d", m_Str);
		ERR_MSG(m_szFPS);
		//플레이어 한테 받으면 스텟 오르고 
		m_StrPlus.bNext = false;
	}
	__super::Tick(fTimeDelta);

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f),(int)( m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));


}

void CStatsWnd::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CStatsWnd::Render()
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

HRESULT CStatsWnd::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatsWnd"), (CComponent**)&m_pTextureCom)))
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

HRESULT CStatsWnd::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CStatsWnd::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CStatsWnd * CStatsWnd::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStatsWnd*	pInstance = new CStatsWnd(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatsWnd::Clone(void* pArg)
{
	CStatsWnd*	pInstance = new CStatsWnd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CStatsWnd::Get_World(void)
{
	return _float4x4();
}

void CStatsWnd::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}

