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
	
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::HPPOTION ,0,10 }, 0);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::MPPOTION ,1,20 }, 1);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::TIARA ,2,1 }, 2);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::BOBY ,3,1 }, 3);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::SHOES ,4,1 }, 4);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::ROBE ,5,1}, 5);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::PANDANT ,6,1 }, 6);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EARRING ,7,1 }, 7);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::BRACELET ,8,1 }, 8);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::RING ,9,1 }, 9);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::STAFF ,10,1}, 10);
	//dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::ORB ,11,1}, 11);
	return S_OK;
}

void CInven::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CUI*>(m_tInfo.pTerrain)->Get_Inven() == false)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(false);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
		Set_Dead();
		return;
	}
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_int iDest;
	if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse() && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().eItemNum != CStatInfo::EITEM_END)
	{
		iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;
		m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)(m_rcSlot[iDest].left + 23) - g_iWinSizeX * 0.5f, -(float)(m_rcSlot[iDest].top + 23) + g_iWinSizeY * 0.5f, 0.f));
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MouseItem({ CStatInfo::EITEM_END, 0, 0});
	}
	for (int i = 0; i < 24; ++i)
	{
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick() && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse())
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
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenMouse(false);
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
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenMouse(true);
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
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

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
	wstring szMoney = TEXT("");
	
	for (int i = 0; i < 24; ++i)
	{
		szCount[i] = TEXT("");
		szCount[i] += to_wstring(m_vecItem[i].iCount);
	}
	szMoney += to_wstring(m_tInfo.pTarget->Get_Info().iMoney);
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
	pGameInstance->Get_Font()->DrawText(nullptr, szMoney.c_str(), (int)szMoney.length(), &m_rcMoneyBox, DT_RIGHT, D3DCOLOR_ARGB(255, 0, 0, 0));
	_float3 vPos;
	wstring szText = TEXT("");
	if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
	{
		for (int i = 0; i < 24; ++i)
		{
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				if(m_vecItem[i].eItemNum == CStatInfo::EITEM_END)
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
					szText += TEXT("빨강포션: HP를 1000\n 회복 시켜준다.");
					break;
				case CStatInfo::MPPOTION:
					szText += TEXT("파랑포션: MP를 100\n 회복 시켜준다.");
					break;
				case CStatInfo::ENGINE:
					szText += TEXT("비행정엔진: 비행정의\n핵심 부품이다.");
					break;
				case CStatInfo::TIARA:
					szText += TEXT("쥬신티아라: DEX + 5 \n     INT + 5");
					break;
				case CStatInfo::BOBY:
					szText += TEXT("쥬신타이즈:DEX + 10");
					break;
				case CStatInfo::SHOES:
					szText += TEXT("쥬신슈즈: DEX + 5 \n     LUK + 5");
					break;
				case CStatInfo::ROBE:
					szText += TEXT("쥬신로브: LUK + 5 \n     INT + 5");
					break;
				case CStatInfo::PANDANT:
					szText += TEXT("쥬신팬던트: INT + 5");
					break;
				case CStatInfo::EARRING:
					szText += TEXT("쥬신귀걸이:LUK + 10");
					break;
				case CStatInfo::BRACELET:
					szText += TEXT("쥬신팔찌: STR + 5 \n     DEX + 5");
					break;
				case CStatInfo::RING:
					szText += TEXT("쥬신반지: STR + 5 \n     LUK + 5");
					break;
				case CStatInfo::STAFF:
					szText += TEXT("쥬신스태프:STR + 10");
					break;
				case CStatInfo::ORB:
					szText += TEXT("쥬신오브: STR + 5 \n     INT + 5");
					break;
				case CStatInfo::RIDEEGG:
					szText += TEXT("알파카알: 대박당첨\n 얼마에 뽑으심?");
					break;
				case CStatInfo::PETEGG:
					szText += TEXT("포링알: 포링의알\n 아이템을 주워준다.");
					break;
				case CStatInfo::WING:
					szText += TEXT("요정날개: 하늘을 \n 날개 해준다는 그!아이템");
					break;
				case CStatInfo::MON1:
					szText += TEXT("엘리게이터의독: \n 독니에서 추출한 독");
					break;
				case CStatInfo::MON2:
					szText += TEXT("윌로우의몸통: 윌로우는\n 튼튼한 목재로 사용 된다.");
					break;
				case CStatInfo::MON3:
					szText += TEXT("빅풋의발다박: 미식가들\n 사이에서 유명한 식재료.");
					break;
				case CStatInfo::MON4:
					szText += TEXT("도적의두건: 도적단을\n토벌했다는 증표로 쓰인다.");
					break;
				case CStatInfo::MON5:
					szText += TEXT("멀더러의식칼: 토막내기\n 좋은 식칼이다.");
					break;
				case CStatInfo::MON6:
					szText += TEXT("썩은붕대: 멀쩡한 붕대\n였지만 단델리온이\n사용하면서 썩었다.");
					break;
				case CStatInfo::MON7:
					szText += TEXT("아트로스의발톱: \n 매우 견고하여 좋은 무기\n 소재로 쓰이고 있다.");
					break;
				case CStatInfo::MON8:
					szText += TEXT("바포매트의뚜껑: 귀족들 \n 사이에서 장식품으로 \n 인기가 많다.");
					break;
				case CStatInfo::MON9:
					szText += TEXT("미노로스의우유: 칼슘이 \n풍부해 성장기 어린이한테 \n 좋은 음료다.");
					break;
				case CStatInfo::MON10:
					szText += TEXT("해골바가지: 물을 받아 \n 마시면 장수한다는데...");
					break;
				case CStatInfo::MON11:
					szText += TEXT("레이스의틀니: 노인들이 \n 갖고 싶어하는 템 \n 1순위 등극!");
					break;
				case CStatInfo::MON12:
					szText += TEXT("좀비고기: 한번만 먹은\n사람이 없다는\n 전설의 식재료.");
					break;

				default:
					break;
				}

				
				pGameInstance->Get_Font2()->DrawText(nullptr, szText.c_str(), (int)szText.length(), &m_rcTextBox, DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
				break;
			}
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
	m_rcMoneyBox = {820,522,920,580};
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
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index).iCount <= 0)
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(CStatInfo::EITEM_END, Index);
		break;
	case CStatInfo::MPPOTION:
		m_tInfo.pTarget->Set_Mp(100);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(-1, Index);
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index).iCount <= 0)
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum(CStatInfo::EITEM_END, Index);
		break;
	case CStatInfo::EARRING:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(0).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::EARRING,0,1 }, 0);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::TIARA:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(1).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::TIARA,1,1 }, 1);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::PANDANT:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(2).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::PANDANT,2,1 }, 2);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::BOBY:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(3).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::BOBY,3,1 }, 3);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::STAFF:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(4).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::STAFF,4,1 }, 4);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::ORB:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(5).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::ORB,5,1 }, 5);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::ROBE:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(6).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::ROBE,6,1 }, 6);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::SHOES:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(7).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::SHOES,7,1 }, 7);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::BRACELET:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(8).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::BRACELET,8,1 }, 8);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
		break;
	case CStatInfo::RING:
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(9).eItemNum == CStatInfo::EITEM_END)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::RING,9,1 }, 9);
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END,Index,0 }, Index);
			m_vecItem[Index] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(Index);
		}
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

	Safe_Release(m_pTextTrans);
	Safe_Release(m_pTextBuffer);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pItemTexture);
	Safe_Release(m_pTextTexture);
}
