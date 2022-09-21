#include "stdafx.h"
#include "..\Public\Shop.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Layer.h"
#include "ShopNPC.h"

CShop::CShop(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	for (_int i = 0; i < 24; ++i)
	{
		m_pSlotTrans[i] = nullptr;
		m_pSlotBuffer[i] = nullptr;
	}
}

CShop::CShop(const CShop & rhs)
	: CGameObject(rhs)
{
	for (_int i = 0; i < 24; ++i)
	{
		m_pSlotBuffer[i] = rhs.m_pSlotBuffer[i];
		m_pSlotTrans[i] = rhs.m_pSlotTrans[i];
	}
}

HRESULT CShop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	m_vecItem.reserve(24);
	for (int i = 0; i < 24; ++i)
	{
		m_vecItem.push_back({ CStatInfo::EITEM_END,i,0});
	}
	for (int i = 0; i < 24; ++i)
	{
		m_pSlotTrans[i] = nullptr;
	}
	m_fSizeX = 300.f;
	m_fSizeY = 550.f;
	m_fX = 450.f;
	m_fY = 350.f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	Set_Slot();

	m_vecItem[0] = { CStatInfo::HPPOTION ,0,0 };
	m_vecItem[1] = { CStatInfo::MPPOTION ,1,0 };
	m_vecItem[4] = { CStatInfo::TIARA ,4,0 };
	m_vecItem[5] = { CStatInfo::BOBY ,5,0};
	m_vecItem[8] = { CStatInfo::SHOES ,8,0 };
	m_vecItem[9] = { CStatInfo::ROBE ,9,0};
	m_vecItem[12] = { CStatInfo::PANDANT ,12,0 };
	m_vecItem[13] = { CStatInfo::EARRING ,13,0};
	m_vecItem[16] = { CStatInfo::BRACELET ,16,0};
	m_vecItem[17] = { CStatInfo::RING ,17,0};
	m_vecItem[20] = { CStatInfo::STAFF ,20,0};
	m_vecItem[21] = { CStatInfo::ORB ,21,0};
	return S_OK;
}

void CShop::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CShopNPC*>(m_tInfo.pTerrain)->Get_Shop() == false)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(false);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_Shop(false);
		Set_Dead();
		return;
	}
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	ShowCursor(false);
	
	
	for (int i = 0; i < 24; ++i)
	{
		if (PtInRect(&m_rcSlot[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
			{
				if (m_vecItem[i].eItemNum != CStatInfo::EITEM_END && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
				{
					UseItem(m_vecItem[i].eItemNum, i);
				}
			}
		}
	}
}

void CShop::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);



	if (nullptr != m_pRendererCom && 0 == g_iCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShop::Render()
{
	_int iItemPrice = 0;
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	ShowCursor(false);
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();
	

	for (int i = 0; i < 24; ++i)
	{
		if (m_vecItem[i].eItemNum == CStatInfo::EITEM_END)
			continue;
		if (FAILED(m_pItemTexture->Bind_OnGraphicDev(m_vecItem[i].eItemNum)))
			return E_FAIL;
		m_pSlotTrans[i]->Bind_OnGraphicDev();
		m_pSlotBuffer[i]->Render();
		
	}



	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_float3 vPos;
	wstring szMoney = TEXT("");
	wstring szText = TEXT("");
	if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
	{
		for (int i = 0; i < 24; ++i)
		{
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				if (m_vecItem[i].eItemNum == CStatInfo::EITEM_END)
					continue;
				vPos = m_pSlotTrans[i]->Get_State(CTransform::STATE_POSITION);
				vPos.x += 130;
				m_pTextTrans->Set_State(CTransform::STATE_POSITION, vPos);
				m_rcTextBox = { m_rcSlot[i].left + 60,m_rcSlot[i].top,m_rcSlot[i].right + 250,m_rcSlot[i].bottom + 80 };
				m_pTextTrans->Bind_OnGraphicDev();
				m_pTextTexture->Bind_OnGraphicDev(0);
				m_pTextBuffer->Render();

				switch (m_vecItem[i].eItemNum)
				{
				case CStatInfo::HPPOTION:
					szText += TEXT("»¡°­Æ÷¼Ç: HP¸¦ 1000\n È¸º¹ ½ÃÄÑÁØ´Ù.");
					iItemPrice = 30;
					break;
				case CStatInfo::MPPOTION:
					szText += TEXT("ÆÄ¶ûÆ÷¼Ç: MP¸¦ 100\n È¸º¹ ½ÃÄÑÁØ´Ù.");
					iItemPrice = 20;
					break;
				case CStatInfo::TIARA:
					szText += TEXT("Áê½ÅÆ¼¾Æ¶ó: DEX + 5 \n     INT + 5");
					iItemPrice = 100;
					break;
				case CStatInfo::BOBY:
					szText += TEXT("Áê½ÅÅ¸ÀÌÁî:DEX + 10");
					iItemPrice = 100;
					break;
				case CStatInfo::SHOES:
					szText += TEXT("Áê½Å½´Áî: DEX + 5 \n     LUK + 5");
					iItemPrice = 50;
					break;
				case CStatInfo::ROBE:
					szText += TEXT("Áê½Å·Îºê: LUK + 5 \n     INT + 5");
					iItemPrice = 50;
					break;
				case CStatInfo::PANDANT:
					szText += TEXT("Áê½ÅÆÒ´øÆ®: INT + 5");
					iItemPrice = 70;
					break;
				case CStatInfo::EARRING:
					szText += TEXT("Áê½Å±Í°ÉÀÌ:LUK + 10");
					iItemPrice = 60;
					break;
				case CStatInfo::BRACELET:
					szText += TEXT("Áê½ÅÆÈÂî: STR + 5 \n     DEX + 5");
					iItemPrice = 40;
					break;
				case CStatInfo::RING:
					szText += TEXT("Áê½Å¹ÝÁö: STR + 5 \n     LUK + 5");
					iItemPrice = 40;
					break;
				case CStatInfo::STAFF:
					szText += TEXT("Áê½Å½ºÅÂÇÁ:STR + 10");
					iItemPrice = 200;
					break;
				case CStatInfo::ORB:
					szText += TEXT("Áê½Å¿Àºê: STR + 5 \n     INT + 5");
					iItemPrice = 150;
					break;
				default:
					break;
				}


				pGameInstance->Get_Font2()->DrawText(nullptr, szText.c_str(), (int)szText.length(), &m_rcTextBox, DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
				break;
			}
		}
	}
	szMoney += to_wstring(iItemPrice);
	pGameInstance->Get_Font()->DrawText(nullptr, szMoney.c_str(), (int)szMoney.length(), &m_rcMoneyBox, DT_RIGHT, D3DCOLOR_ARGB(255, 0, 0, 0));
	Safe_Release(pGameInstance);
	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShop::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Shop"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pItemTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextBox2"), (CComponent**)&m_pTextTexture)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer99"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pTextBuffer)))
		return E_FAIL;
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);


	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform99"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTextTrans, &TransformDesc)))
		return E_FAIL;
	wstring szBuffer[24];
	wstring szTrans[24];
	for (int i = 0; i < 24; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBuffer");
		szTrans[i] = TEXT("Com_Transform");
	}
	for (int i = 0; i < 24; ++i)
	{
		szBuffer[i] += to_wstring(i);
		szTrans[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pSlotBuffer[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pSlotTrans[i], &TransformDesc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CShop::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CShop::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CShop::Set_Slot()
{
	_float fSizeX = 60.f;
	_float fSizeY = 60.f;
	_float fX;
	_float fY;
	_int k = 0;
	m_pTextTrans->Set_Scaled(_float3(200, 80, 1.f));
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fX = 345.f + 63.f * j;
			fY = 180.f + 60.f * i;
			m_pSlotTrans[k]->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
			m_pSlotTrans[k]->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
			++k;
		}
	}
	k = 0;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_rcSlot[k] = { int(322.f + 63.f * j),int(157.f + 60.f * i),int(368.f + 63.f * j),int(203.f + 60.f * i) };
			++k;
		}
	}

	m_rcMoneyBox = { 420,522,520,580 };
}

void CShop::UseItem(CStatInfo::EITEM _eItem, _int Index)
{
	_int iItemPrice = 0;
	switch (_eItem)
	{
	case CStatInfo::HPPOTION:
		iItemPrice = 30;
		break;
	case CStatInfo::MPPOTION:
		iItemPrice = 20;
		break;
	case CStatInfo::TIARA:
		iItemPrice = 100;
		break;
	case CStatInfo::BOBY:
		iItemPrice = 100;
		break;
	case CStatInfo::SHOES:
		iItemPrice = 50;
		break;
	case CStatInfo::ROBE:
		iItemPrice = 50;
		break;
	case CStatInfo::PANDANT:
		iItemPrice = 70;
		break;
	case CStatInfo::EARRING:
		iItemPrice = 60;
		break;
	case CStatInfo::BRACELET:
		iItemPrice = 40;
		break;
	case CStatInfo::RING:
		iItemPrice = 40;
		break;
	case CStatInfo::STAFF:
		iItemPrice = 200;
		break;
	case CStatInfo::ORB:
		iItemPrice = 150;
		break;
	default:
		break;
	}
	if (m_tInfo.pTarget->Get_Info().iMoney >= iItemPrice)
	{
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == _eItem)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				m_tInfo.pTarget->Set_Money(-iItemPrice);
				return;
			}
		}
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == CStatInfo::EITEM_END)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(_eItem, i);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				m_tInfo.pTarget->Set_Money(-iItemPrice);
				return;
			}
		}
	}
}

CShop * CShop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShop*	pInstance = new CShop(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop::Clone(void* pArg)
{
	CShop*	pInstance = new CShop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CShop::Get_World(void)
{
	return _float4x4();
}

void CShop::Free()
{
	__super::Free();

	for (int i = 0; i < 24; ++i)
	{
		Safe_Release(m_pSlotBuffer[i]);
		Safe_Release(m_pSlotTrans[i]);
	}

	Safe_Release(m_pTextTrans);
	Safe_Release(m_pTextBuffer);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pItemTexture);
	Safe_Release(m_pTextTexture);
}
