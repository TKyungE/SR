#include "stdafx.h"
#include "..\Public\Inven.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "UI.h"
#include "Player.h"
#include "Layer.h"

CInven::CInven(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	for (_int i = 0; i < 24; ++i)
	{
		m_pSlotTrans[i] = nullptr;
		m_pSlotBuffer[i] = nullptr;
	}
}

CInven::CInven(const CInven & rhs)
	: CGameObject(rhs)
{
	for (_int i = 0; i < 24; ++i)
	{
		m_pSlotBuffer[i] = rhs.m_pSlotBuffer[i];
		m_pSlotTrans[i] = rhs.m_pSlotTrans[i];
	}
}

HRESULT CInven::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInven::Initialize(void* pArg)
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
		m_vecItem.push_back(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i));
	}
	for (int i = 0; i < 24; ++i)
	{
		m_pSlotTrans[i] = nullptr;
	}
	m_fSizeX = 300.f;
	m_fSizeY = 550.f;
	m_fX = 850.f;
	m_fY = 350.f;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	Set_Slot();
	dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(CStatInfo::HPPOTION, 0);
	dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemCount(115, 0);
	dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(CStatInfo::MPPOTION, 1);
	dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemCount(77, 1);
	return S_OK;
}

void CInven::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CUI*>(m_tInfo.pTerrain)->Get_Inven() == false)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(false);
		Set_Dead();
		return;
	}
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_int iDest;
	for (int i = 0; i < 24; ++i)
	{
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
		{
			iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;
			m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)ptMouse.x - g_iWinSizeX * 0.5f, -(float)ptMouse.y + g_iWinSizeY * 0.5f, 0.f));
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
				{
					m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)(m_rcSlot[iDest].left + 23) - g_iWinSizeX * 0.5f, -(float)(m_rcSlot[iDest].top + 23) + g_iWinSizeY * 0.5f, 0.f));
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemCount(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).iCount, iDest);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum, iDest);
					
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemCount(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iCount,i);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().eItemNum, i);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemSlot(i, i);
					
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
				}

			}
		}
	}
	for (int i = 0; i < 24; ++i)
	{
		if (PtInRect(&m_rcSlot[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
			{
				if (m_vecItem[i].eItemNum != CStatInfo::EITEM_END && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(true);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MouseItem(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i));
				}
			}
		}
	}
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

void CInven::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Check_Slot();

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInven::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;

	for (int i = 0; i < 24; ++i)
	{
		if (m_vecItem[i].eItemNum == CStatInfo::EITEM_END)
			continue;
		if (FAILED(m_pItemTexture->Bind_OnGraphicDev(m_vecItem[i].eItemNum)))
			return E_FAIL;
		m_pSlotTrans[i]->Bind_OnGraphicDev();
		m_pSlotBuffer[i]->Render();
	}
	wstring szCount[24];
	for (int i = 0; i < 24; ++i)
	{
		szCount[i] = TEXT("");
		szCount[i] += to_wstring(m_vecItem[i].iCount);
	}
	
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	for (int i = 0; i < 24; ++i)
	{
		if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
		{
			if (m_vecItem[i].iCount > 1)
				pGameInstance->Get_Font()->DrawText(nullptr, szCount[i].c_str(), (int)szCount[i].length(), &m_rcCount[i], DT_RIGHT, D3DCOLOR_ARGB(255, 0, 0, 0));

		}
	}
	Safe_Release(pGameInstance);

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInven::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inven"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pItemTexture)))
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

HRESULT CInven::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CInven::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CInven::Set_Slot()
{
	_float fSizeX = 46.f;
	_float fSizeY = 46.f;
	_float fX;
	_float fY;
	_int k = 0;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			fX = 745.f + 63.f * j;
			fY = 180.f + 60.f * i;
			m_pSlotTrans[k]->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
			m_pSlotTrans[k]->Set_State(CTransform::STATE_POSITION, _float3(fX- g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
			++k;
		}
	}
	k = 0;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_rcSlot[k] = { int(722.f + 63.f * j),int(157.f + 60.f * i),int(768.f + 63.f * j),int(203.f + 60.f * i)};
			++k;
		}
	}
	k = 0;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_rcCount[k] = { int(722.f + 63.f * j),int(182.f + 60.f * i),int(768.f + 63.f * j),int(203.f + 60.f * i) };
			++k;
		}
	}
	
}

void CInven::Check_Slot()
{
	_int iDest = 0;
	for (int i = 0; i < 24; ++i)
	{
		iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).iSlotNum;
		m_vecItem[iDest] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i);
	}
}

void CInven::UseItem(CStatInfo::EITEM _eItem,_int Index)
{
	switch (_eItem)
	{
	case CStatInfo::HPPOTION:
		m_tInfo.pTarget->Set_Hp(-1000);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(-1, Index);
		break;
	case CStatInfo::MPPOTION:
		m_tInfo.pTarget->Set_Mp(100);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(-1, Index);
		break;
	default:
		break;
	}
}

CInven * CInven::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInven*	pInstance = new CInven(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInven"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInven::Clone(void* pArg)
{
	CInven*	pInstance = new CInven(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInven"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CInven::Get_World(void)
{
	return _float4x4();
}

void CInven::Free()
{
	__super::Free();

	for (int i = 0; i < 24; ++i)
	{
		Safe_Release(m_pSlotBuffer[i]);
		Safe_Release(m_pSlotTrans[i]);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pItemTexture);
}
