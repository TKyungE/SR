#include "stdafx.h"
#include "..\Public\HpPotion.h"
#include"GameInstance.h"
#include"Layer.h"
#include "Inventory.h"


CHpPotion::CHpPotion(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CHpPotion::CHpPotion(const CHpPotion & rhs)
	: CGameObject(rhs)
{
}
HRESULT CHpPotion::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpPotion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	m_tInfo.iExp = 0;
	m_fSizeX = 30.0f;
	m_fSizeY = 30.0f;
	m_iCount = 10;
	m_tInfo.bDead = false;
	D3DXCreateFont(m_pGraphic_Device,
		15,
		0,
		FW_NORMAL, 
		1, 
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE,
		TEXT("±Ã¼­"),
		&m_pFont);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));


	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	pTarget = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Player"))->Get_Objects().back();
	
	
	Safe_Release(pGameInstance);

	return S_OK;
}

void CHpPotion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_tInfo.iMp == 5)
	{
		RECT		rcRect;
		SetRect(&rcRect, (int)(m_tInfo.vPos.x - m_fSizeX * 0.5f), (int)(m_tInfo.vPos.y - m_fSizeY * 0.5f), (int)(m_tInfo.vPos.x + m_fSizeX * 0.5f), (int)(m_tInfo.vPos.y + m_fSizeY * 0.5f));

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		if (PtInRect(&rcRect, ptMouse))
		{

			if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON))
			{
				m_tInfo.vPos.x = ptMouse.x;
				m_tInfo.vPos.y = ptMouse.y;
			}

		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_tInfo.vPos.x - g_iWinSizeX * 0.5f, -m_tInfo.vPos.y + g_iWinSizeY * 0.5f, 0.f));
	}

	RECT		rcRect;
	SetRect(&rcRect, (int)(m_tInfo.vPos.x - m_fSizeX * 0.5f), (int)(m_tInfo.vPos.y - m_fSizeY * 0.5f), (int)(m_tInfo.vPos.x + m_fSizeX * 0.5f), (int)(m_tInfo.vPos.y + m_fSizeY * 0.5f));

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (PtInRect(&rcRect, ptMouse))
	{
		if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
		{
			m_tInfo.iExp = 2;
		}
	}

	if (m_tInfo.iExp == 2)
	{
		Use();
	}
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.3f);
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_ITEM, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CHP_POTION"));
		return;
	}

	Safe_Release(pInstance);
}
void CHpPotion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	CheckColl();
	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CHpPotion::Render()

{
	if (m_tInfo.iMp == 5)
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

		RECT rcText;
		TCHAR Count[VK_MAX];

		
		wsprintf(Count, (L"%d"), m_iCount);
		if (m_iCount > 9)
		{
			SetRect(&m_rcRect, m_tInfo.vPos.x, m_tInfo.vPos.y, 0, 0);
		}else
			SetRect(&m_rcRect, m_tInfo.vPos.x+8.f, m_tInfo.vPos.y, 0, 0);
		m_pFont->DrawText(NULL, Count, -1, &m_rcRect, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f));

		
	}
	if (g_bCollider)
		m_pColliderCom->Render();
	return S_OK;

}

void CHpPotion::Use(void)
{
	if (pTarget->Get_Info().iHp != pTarget->Get_Info().iMaxHp)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pGameInstance);
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LogRect"), m_tInfo.iLevelIndex, TEXT("Layer_Log"), &m_tInfo);
		Safe_Release(pGameInstance);
		pTarget->Set_Hp(-(pTarget->Get_Info().iMaxHp*0.3));
		if (m_iCount > 0)
		{
			--m_iCount;	
		}
		else if (m_iCount == 0)
		{
			Set_Dead();
			m_tInfo.pTarget->Set_bHit(false);
		}
	}
}

HRESULT CHpPotion::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Temp"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CHpPotion::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CHpPotion::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CHpPotion::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
	{
		
		Set_Dead();
		auto& iter = dynamic_cast<CInventory*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Inventory")))->Get_InvenSlot()->begin();
		for (; iter != dynamic_cast<CInventory*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Inventory")))->Get_InvenSlot()->end();++iter)
		{
			if ((*iter)->Get_Info().bHit == false)
			{
				CGameObject::INFO tInfo;
				tInfo.vPos = (*iter)->Get_Info().vPos;
				tInfo.pTarget = this;
				tInfo.iMp = 3;
				pInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Potion"), &tInfo);
				(*iter)->Set_bHit(true);
			}
		}
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PET, TEXT("Com_Collider"), &pTarget))
	{
		Set_Dead();
		auto& iter = dynamic_cast<CInventory*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Inventory")))->Get_InvenSlot()->begin();
		for (; iter != dynamic_cast<CInventory*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Inventory")))->Get_InvenSlot()->end(); ++iter)
		{
			if ((*iter)->Get_Info().bHit == false)
			{
				CGameObject::INFO tInfo;
				tInfo.vPos = (*iter)->Get_Info().vPos;
				tInfo.pTarget = this;
				tInfo.iMp = 3;
				pInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Potion"), &tInfo);
				(*iter)->Set_bHit(true);
			}
		}
	}
	Safe_Release(pInstance);
}

CHpPotion * CHpPotion::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpPotion*	pInstance = new CHpPotion(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHpPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpPotion::Clone(void * pArg)
{
	CHpPotion*	pInstance = new CHpPotion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHpPotion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CHpPotion::Get_World(void)
{
	return _float4x4();
}

void CHpPotion::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pFont);
}


