#include "stdafx.h"

#include "..\Public\InventoryIcon.h"
#include "GameInstance.h"
#include"Layer.h"
CInventoryIcon::CInventoryIcon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CInventoryIcon::CInventoryIcon(const CInventoryIcon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventoryIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventoryIcon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	
	m_fSizeX = 30.0f;
	m_fSizeY = 30.0f;
	m_fX = 30.f;
	m_fY = 210.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

void CInventoryIcon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (m_tInfo.pTarget->Get_Info().bHit)
	{
		RECT		rcRect;
		SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		if (PtInRect(&rcRect, ptMouse))
		{
			m_Check = 2;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
				
				Safe_AddRef(pGameInstance);
				pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Inventory"))->Get_Objects().back()->Set_bHit(false);
				Safe_Release(pGameInstance);
			}
		}
		else
			m_Check = 3;
	}
}

void CInventoryIcon::Late_Tick(_float fTimeDelta)
{
	if (m_tInfo.pTarget->Get_Info().bHit)
	{
		__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CInventoryIcon::Render()
{
	if (m_tInfo.pTarget->Get_Info().bHit)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4		ViewMatrix;
		D3DXMatrixIdentity(&ViewMatrix);

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_Check)))
			return E_FAIL;

		if (FAILED(SetUp_RenderState()))
			return E_FAIL;

		m_pVIBufferCom->Render();

		if (FAILED(Release_RenderState()))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CInventoryIcon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryIcon"), (CComponent**)&m_pTextureCom)))
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

HRESULT CInventoryIcon::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CInventoryIcon::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CInventoryIcon * CInventoryIcon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventoryIcon*	pInstance = new CInventoryIcon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInventoryIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventoryIcon::Clone(void * pArg)
{
	CInventoryIcon*	pInstance = new CInventoryIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInventoryIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CInventoryIcon::Get_World(void)
{
	return _float4x4();
}

void CInventoryIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
