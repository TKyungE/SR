#include "stdafx.h"
#include "..\Public\LogRect.h"
#include"GameInstance.h"
#include"Layer.h"

CLogRect::CLogRect(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CLogRect::CLogRect(const CLogRect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLogRect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogRect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_fSizeX = 360.f;
	m_fSizeY = 20.f;
	m_fX = 1100.0f;
	m_fY = 650.0f;
	m_tInfo.vPos.x = m_fX;
	m_tInfo.vPos.y = m_fY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	D3DXCreateFont(m_pGraphic_Device,
		15,
		0,
		FW_BOLD,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±Ã¼­"),
		&m_pFont);

	m_iTimer = 0;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);
	if (pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Log")) != nullptr)
	{
		m_lObj = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Log"))->Get_Objects();
		if (!m_lObj.empty())
		{
			m_Obj = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Log"))->Get_Objects().back();//·Î±×°¡ ÀÖ¾î
			m_bPreCheck = true;
		}
		for (auto& iter : m_lObj)
		{
			m_iCount += 1;

		}

	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CLogRect::Tick(_float fTimeDelta)
{

	++m_iTimer;
	__super::Tick(fTimeDelta);
	if (m_bPreCheck == true)
	{
		_float3 vPos = _float3(0.f, -m_iCount * 15.f, 0.f);
		m_Obj->Set_UiPos(vPos);
		m_bPreCheck = false;

	}
	m_fX = m_tInfo.vPos.x;
	m_fY = m_tInfo.vPos.y;
	if (m_iTimer == 120)
	{
		Set_Dead();
	}
	if (m_fY <= 600)
	{
		Set_Dead();
	}


	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}
void CLogRect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CLogRect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	//	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	TCHAR Count[VK_MAX];

	wsprintf(Count, (L"¹ìÆÄÀÌ¾îÀÇ ÇÇ¸¦ È¹µæÇÏ¼Ì½À´Ï´Ù."));
	SetRect(&m_rcRect, (int)m_fX - m_fSizeX*0.5, (int)m_fY + m_fSizeY*0.5, (int)m_fX + m_fSizeX*0.5, (int)m_fY + m_fSizeX*0.5);
	m_pFont->DrawText(NULL, Count, -1, &m_rcRect, DT_RIGHT, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f));


	return S_OK;
}

HRESULT CLogRect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
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

HRESULT CLogRect::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return S_OK;
}
HRESULT CLogRect::Release_RenderState()
{
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CLogRect * CLogRect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogRect*	pInstance = new CLogRect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLogRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLogRect::Clone(void * pArg)
{
	CLogRect*	pInstance = new CLogRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLogRect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CLogRect::Get_World(void)
{
	return _float4x4();
}

void CLogRect::Free()
{
	Safe_Release(m_pFont);
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

