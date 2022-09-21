#include "stdafx.h"
#include "..\Public\QuickSlot.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Layer.h"


CQuickSlot::CQuickSlot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotTrans[i] = nullptr;
		m_pSlotBuffer[i] = nullptr;
	}
}

CQuickSlot::CQuickSlot(const CQuickSlot & rhs)
	: CGameObject(rhs)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotBuffer[i] = rhs.m_pSlotBuffer[i];
		m_pSlotTrans[i] = rhs.m_pSlotTrans[i];
	}
}

HRESULT CQuickSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuickSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	m_fSizeX = 465.f;
	m_fSizeY = 55.f;
	m_fX = 400.f;
	m_fY = 684.f;
	m_pvecItem.reserve(10);
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 9;
	m_tFrame.fFrameSpeed = 0.1f;
	for (int i = 0; i < 10; ++i)
	{
		m_pvecItem.push_back({CStatInfo::EITEM_END,0,0});
	}
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	Set_Slot();
	m_pMouseTransformCom->Set_Scaled(_float3(22.f, 31.f, 1.f));
	return S_OK;
}

void CQuickSlot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if(m_tInfo.pTarget->Get_Info().iLevelIndex == LEVEL_SKY)
		m_SkyCool += fTimeDelta;
	Move_Frame(fTimeDelta);
	Check_Slot(fTimeDelta);
	Use_Slot();
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	ShowCursor(false);
	_float3 vMousePos = { (float)(ptMouse.x+9.f) - g_iWinSizeX * 0.5f, -(float)(ptMouse.y+13.f) + g_iWinSizeY * 0.5f, 0.f };
	m_pMouseTransformCom->Set_State(CTransform::STATE_POSITION, vMousePos);

	_int iDest;
	for (int i = 0; i < 10; ++i)
	{
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick()&& dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse())
		{
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
				{
					if (m_pvecItem[i].eItemNum == CStatInfo::EITEM_END)
					{
						switch (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().eItemNum)
						{
						case CStatInfo::HPPOTION:
						case CStatInfo::MPPOTION:
						case CStatInfo::WING:
							iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickSlot(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(iDest), i);
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemSlot(i, i);
							m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i);

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END ,iDest,0 }, iDest);

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenMouse(false);
							m_bMousePick = false;
							break;
						case CStatInfo::SKILL_THUNDER:
						case CStatInfo::SKILL_TORNADO:
						case CStatInfo::SKILL_FIREBALL:
						case CStatInfo::RIDE_ALPACA:
							iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickSlot(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkillSlot(iDest), i);
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemSlot(i, i);
							m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i);

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem({ CStatInfo::EITEM_END ,iDest,0 }, iDest);

							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenMouse(false);
							m_bMousePick = false;
							break;
						default:
							break;
						}
					}
				}

			}
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick() && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse())
		{
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;
				m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)ptMouse.x - g_iWinSizeX * 0.5f, -(float)ptMouse.y + g_iWinSizeY * 0.5f, 0.f));

				if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
				{
					m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)(m_rcSlot[iDest].left + 20) - g_iWinSizeX * 0.5f, -(float)(m_rcSlot[iDest].top + 20) + g_iWinSizeY * 0.5f, 0.f));

					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemCount(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).iCount, iDest);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemNum(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).eItemNum, iDest);
					

					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemCount(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iCount, i);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemNum(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().eItemNum, i);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemSlot(i, i);
					m_pvecItem[iDest] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iDest);
					m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i);

					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
					m_bMousePick = false;
					
					break;
						
				}

			}
		}
	}


	for (int i = 0; i < 10; ++i)
	{
		if (PtInRect(&m_rcSlot[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
			{
				if (m_pvecItem[i].eItemNum != CStatInfo::EITEM_END && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(true);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MouseItem(m_pvecItem[i]);
					m_bMousePick = true;
					
				}
			}
		}
	}
}

void CQuickSlot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MOUSE, this);
}

HRESULT CQuickSlot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	if (0 == g_iCut)
	{
		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
		m_pVIBufferCom->Render();

		for (int i = 0; i < 10; ++i)
		{
			if (m_pvecItem[i].eItemNum == CStatInfo::EITEM_END)
				continue;
			if (FAILED(m_pItemTexture->Bind_OnGraphicDev(m_pvecItem[i].eItemNum)))
				return E_FAIL;
			m_pSlotTrans[i]->Bind_OnGraphicDev();
			m_pSlotBuffer[i]->Render();
		}
		wstring szCount[10];
		for (int i = 0; i < 10; ++i)
		{
			szCount[i] = TEXT("");
			if(m_pvecItem[i].eItemNum != CStatInfo::WING)
				szCount[i] += to_wstring(m_pvecItem[i].iCount);
		}

		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		for (int i = 0; i < 10; ++i)
		{
			if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
			{
				if (m_pvecItem[i].iCount > 0)
					pGameInstance->Get_Font()->DrawText(nullptr, szCount[i].c_str(), (int)szCount[i].length(), &m_rcCount[i], DT_RIGHT, D3DCOLOR_ARGB(255, 0, 0, 0));

			}
		}

		Safe_Release(pGameInstance);
	}

	if (m_tInfo.pTarget->Get_Info().iLevelIndex != LEVEL_SKY)
	{
		if (FAILED(m_pMouseTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;
		if (FAILED(m_pMouseTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		m_pMouseBuffer->Render();
	}
	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CQuickSlot::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuickSlot"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pItemTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), (CComponent**)&m_pMouseTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer88"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pMouseBuffer)))
		return E_FAIL;
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform88"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pMouseTransformCom, &TransformDesc)))
		return E_FAIL;
	wstring szBuffer[10];
	wstring szTrans[10];
	for (int i = 0; i < 10; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBuffer");
		szTrans[i] = TEXT("Com_Transform");
	}
	for (int i = 0; i < 10; ++i)
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

HRESULT CQuickSlot::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CQuickSlot::Release_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CQuickSlot * CQuickSlot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQuickSlot*	pInstance = new CQuickSlot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CQuickSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuickSlot::Clone(void* pArg)
{
	CQuickSlot*	pInstance = new CQuickSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CQuickSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CQuickSlot::Get_World(void)
{
	return _float4x4();
}

void CQuickSlot::Free()
{
	__super::Free();

	for (int i = 0; i < 10; ++i)
	{
		Safe_Release(m_pSlotBuffer[i]);
		Safe_Release(m_pSlotTrans[i]);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMouseTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pMouseBuffer);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMouseTextureCom);
	Safe_Release(m_pItemTexture);
}
void CQuickSlot::Set_Slot()
{
	_float fSizeX = 45.f;
	_float fSizeY = 45.f;
	_float fX;
	_float fY;
	for (int i = 0; i < 10; ++i)
	{
		fX = 430.f + 45.f * i;
		fY = 683.f;
		m_pSlotTrans[i]->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
		m_pSlotTrans[i]->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
	}
	for (int i = 0; i < 10; ++i)
	{
		m_rcSlot[i] = { 410 + 45 * i,663,450 + 45 * i,703 };
	}
	
	for (int i = 0; i < 10; ++i)
	{
		m_rcCount[i] = { 410 + 45 * i,685,450 + 45 * i,723 };
	}
}
void CQuickSlot::Check_Slot(_float fTimeDelta)
{
	if (m_tInfo.pTarget->Get_Info().iLevelIndex != LEVEL_SKY)
	{
		m_ThunderCool += fTimeDelta;
		m_TornadoCool += fTimeDelta;
		m_FireBall += fTimeDelta;
		for (int i = 0; i < 10; ++i)
		{
			switch (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).eItemNum)
			{
			case CStatInfo::SKILL_THUNDER:
				if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).iCount > 0 && m_ThunderCool > 1.f)
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickUseItemCount(-1, i);
					m_ThunderCool = 0.f;
				}
				break;
			case CStatInfo::SKILL_TORNADO:
				if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).iCount > 0 && m_TornadoCool > 1.f)
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickUseItemCount(-1, i);
					m_TornadoCool = 0.f;
				}
				break;
			case CStatInfo::SKILL_FIREBALL:
				if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i).iCount > 0 && m_FireBall > 1.f)
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickUseItemCount(-1, i);
					m_FireBall = 0.f;
				}
				break;
			default:
				break;
			}
			m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(i);
		}
	}
	if (m_tInfo.pTarget->Get_Info().iLevelIndex == LEVEL_SKY)
	{
		m_ThunderCool += fTimeDelta;
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkyQuickSlot(1).iCount > 0 && m_ThunderCool > 1.f)
		{
			dynamic_cast<CStatInfo*>(m_StatInfo)->Set_SkyQuickUseItemCount(-1, 1);
			m_ThunderCool = 0.f;
		}
		m_pvecItem[0] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkyQuickSlot(0);
		m_pvecItem[1] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkyQuickSlot(1);
		for (int i = 2; i < 10; ++i)
		{
			m_pvecItem[i] = { CStatInfo::EITEM_END,0,0 };
		}
	}
}

void CQuickSlot::Use_Slot()
{
	_int iIndex = 99;
	if (m_tInfo.pTarget->Get_Info().iLevelIndex != LEVEL_SKY)
	{
		if (CKeyMgr::Get_Instance()->Key_Down('1'))
			iIndex = 0;
		if (CKeyMgr::Get_Instance()->Key_Down('2'))
			iIndex = 1;
		if (CKeyMgr::Get_Instance()->Key_Down('3'))
			iIndex = 2;
		if (CKeyMgr::Get_Instance()->Key_Down('4'))
			iIndex = 3;
		if (CKeyMgr::Get_Instance()->Key_Down('5'))
			iIndex = 4;
		if (CKeyMgr::Get_Instance()->Key_Down('6'))
			iIndex = 5;
		if (CKeyMgr::Get_Instance()->Key_Down('7'))
			iIndex = 6;
		if (CKeyMgr::Get_Instance()->Key_Down('8'))
			iIndex = 7;
		if (CKeyMgr::Get_Instance()->Key_Down('9'))
			iIndex = 8;
		if (CKeyMgr::Get_Instance()->Key_Down('0'))
			iIndex = 9;

		if (iIndex != 99)
		{
			switch (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).eItemNum)
			{
			case CStatInfo::HPPOTION:
				m_tInfo.pTarget->Set_Hp(-1000);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickUseItemCount(-1, iIndex);
				if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).iCount <= 0)
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemNum(CStatInfo::EITEM_END, iIndex);
				break;
			case CStatInfo::MPPOTION:
				m_tInfo.pTarget->Set_Mp(100);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickUseItemCount(-1, iIndex);
				if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).iCount <= 0)
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemNum(CStatInfo::EITEM_END, iIndex);
				break;
			case CStatInfo::SKILL_THUNDER:
				if (m_tInfo.pTarget->Get_Info().iMp >= 20 && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).iCount == 0)
				{
					dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Use_Skill(1);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemCount(5, iIndex);
					m_ThunderCool = 0.f;
				}
				break;
			case CStatInfo::SKILL_TORNADO:
				if (m_tInfo.pTarget->Get_Info().iMp >= 10 && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).iCount == 0)
				{
					dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Use_Skill(2);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemCount(3, iIndex);
					m_TornadoCool = 0.f;
				}
				break;
			case CStatInfo::SKILL_FIREBALL:
				if (m_tInfo.pTarget->Get_Info().iMp >= 5 && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_QuickSlot(iIndex).iCount == 0)
				{
					dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Use_Skill(3);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_QuickItemCount(1, iIndex);
					m_FireBall = 0.f;
				}
				break;
			case CStatInfo::RIDE_ALPACA:
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Ride_Alpaca();
				break;
			case CStatInfo::WING:
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Wing_Fly();
				break;
			default:
				break;
			}
		}
	}
	if (m_tInfo.pTarget->Get_Info().iLevelIndex == LEVEL_SKY)
	{
		if (GetKeyState(VK_RBUTTON) < 0 && m_SkyCool > 0.1f)
		{
			iIndex = 1;
			m_SkyCool = 0.f;
		}
		else if (GetKeyState(VK_LBUTTON) < 0 && m_SkyCool > 0.1f)
		{
			iIndex = 0;
			m_SkyCool = 0.f;
		}
		
		switch (iIndex)
		{
		case 0:
			dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Use_Skill(1);
			break;
		case 1:
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkyQuickSlot(1).iCount == 0)
			{
				dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Use_Skill(2);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_SkyQuickItemCount(3, 1);
				m_ThunderCool = 0.f;
			}
			break;
		default:
			break;
		}
	}
}
void CQuickSlot::Move_Frame(_float fTimeDelta)
{
	m_tFrame.iFrameStart = m_pMouseTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
}