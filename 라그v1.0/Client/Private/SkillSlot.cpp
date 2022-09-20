#include "stdafx.h"
#include "..\Public\SkillSlot.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Layer.h"
#include "UI.h"

CSkillSlot::CSkillSlot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotTrans[i] = nullptr;
		m_pSlotBuffer[i] = nullptr;
	}
}

CSkillSlot::CSkillSlot(const CSkillSlot & rhs)
	: CGameObject(rhs)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotBuffer[i] = rhs.m_pSlotBuffer[i];
		m_pSlotTrans[i] = rhs.m_pSlotTrans[i];
	}
}

HRESULT CSkillSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	m_fSizeX = 560.f;
	m_fSizeY = 400.f;
	m_fX = 640.f;
	m_fY = 360.f;
	m_pvecItem.reserve(7);
	for (int i = 0; i < 7; ++i)
	{
		m_pvecItem.push_back({ CStatInfo::EITEM_END,i,0 });
	}
	m_pvecItem[0].eItemNum = CStatInfo::SKILL_THUNDER;
	m_pvecItem[1].eItemNum = CStatInfo::SKILL_TORNADO;
	m_pvecItem[2].eItemNum = CStatInfo::SKILL_FIREBALL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	Set_Slot();
	Check_Slot();
	return S_OK;
}

void CSkillSlot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Slot();
	if (dynamic_cast<CUI*>(m_tInfo.pTerrain)->Get_Skill() == false)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(false);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
		Set_Dead();
		return;
	}
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	ShowCursor(false);
	_int iDest;
	if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse() && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().eItemNum != CStatInfo::EITEM_END)
	{
		iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;
		m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)(m_rcSlot[iDest].left + 10) - g_iWinSizeX * 0.5f, -(float)(m_rcSlot[iDest].top + 10) + g_iWinSizeY * 0.5f, 0.f));
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MouseItem({ CStatInfo::EITEM_END, 0, 0 });
	}
	for (int i = 0; i < 7; ++i)
	{
		if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick() && dynamic_cast<CStatInfo*>(m_StatInfo)->Get_InvenMouse())
		{
			iDest = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MouseItem().iSlotNum;
			m_pSlotTrans[iDest]->Set_State(CTransform::STATE_POSITION, _float3((float)ptMouse.x - g_iWinSizeX * 0.5f, -(float)ptMouse.y + g_iWinSizeY * 0.5f, 0.f));
			
		}
	}
	for (int i = 0; i < 7; ++i)
	{
		if (PtInRect(&m_rcSlot[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
			{
				if (m_pvecItem[i].eItemNum != CStatInfo::EITEM_END && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
				{
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(true);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenMouse(true);
					dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MouseItem(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkillSlot(i));

				}
			}
		}
	}
}

void CSkillSlot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && 0 == g_iCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkillSlot::Render()
{
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

	for (int i = 0; i < 7; ++i)
	{
		if (m_pvecItem[i].eItemNum == CStatInfo::EITEM_END)
			continue;
		if (FAILED(m_pItemTexture->Bind_OnGraphicDev(m_pvecItem[i].eItemNum)))
			return E_FAIL;
		m_pSlotTrans[i]->Bind_OnGraphicDev();
		m_pSlotBuffer[i]->Render();
	}
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	_float3 vPos;
	wstring szText = TEXT("");
	if (!dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
	{
		for (int i = 0; i < 7; ++i)
		{
			if (PtInRect(&m_rcSlot[i], ptMouse))
			{
				if (m_pvecItem[i].eItemNum == CStatInfo::EITEM_END)
					continue;
				vPos = m_pSlotTrans[i]->Get_State(CTransform::STATE_POSITION);
				vPos.x += 120;
				vPos.y -= 20;
				m_pTextTrans->Set_State(CTransform::STATE_POSITION, vPos);
				m_rcTextBox = { m_rcSlot[i].left + 35,m_rcSlot[i].top,m_rcSlot[i].right + 250,m_rcSlot[i].bottom + 80 };
				m_pTextTrans->Bind_OnGraphicDev();
				m_pTextTexture->Bind_OnGraphicDev(0);
				m_pTextBuffer->Render();

				switch (m_pvecItem[i].eItemNum)
				{
				case CStatInfo::SKILL_THUNDER:
					szText += TEXT("백만볼트: Mp 20 소모 \n 번개가 무수히 떨어진다.\n쿨타임 5초");
					break;
				case CStatInfo::SKILL_TORNADO:
 					szText += TEXT("토네이도: Mp 10 소모 \n 토네이도를 일으켜 \n 지속 피해를 준다.\n쿨타임 3초");
					break;
				case CStatInfo::SKILL_FIREBALL:
					szText += TEXT("파이어볼: Mp 5 소모 \n 화염구를 발사 한다.\n쿨타임 1초");
					break;
				case CStatInfo::RIDE_ALPACA:
					szText += TEXT("알파카탑승: 이동속도가 \n 알파카만큼 빨라 집니다?.");
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

HRESULT CSkillSlot::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillSlot"), (CComponent**)&m_pTextureCom)))
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
	wstring szBuffer[7];
	wstring szTrans[7];
	for (int i = 0; i < 7; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBuffer");
		szTrans[i] = TEXT("Com_Transform");
	}
	for (int i = 0; i < 7; ++i)
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

HRESULT CSkillSlot::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CSkillSlot::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CSkillSlot * CSkillSlot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkillSlot*	pInstance = new CSkillSlot(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillSlot::Clone(void* pArg)
{
	CSkillSlot*	pInstance = new CSkillSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkillSlot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CSkillSlot::Get_World(void)
{
	return _float4x4();
}

void CSkillSlot::Free()
{
	__super::Free();

	for (int i = 0; i < 7; ++i)
	{
		Safe_Release(m_pSlotBuffer[i]);
		Safe_Release(m_pSlotTrans[i]);
	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextBuffer);
	Safe_Release(m_pItemTexture);
	Safe_Release(m_pTextTexture);
	Safe_Release(m_pTextTrans);
}
void CSkillSlot::Set_Slot()
{
	_float fSizeX = 20.f;
	_float fSizeY = 20.f;
	_float fX;
	_float fY;
	m_pTextTrans->Set_Scaled(_float3(200, 80, 1.f));
	for (int i = 0; i < 7; ++i)
	{
		fX = 432.f + 71.f * i;
		fY = 216.f;
		m_pSlotTrans[i]->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
		m_pSlotTrans[i]->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
	}
	for (int i = 0; i < 7; ++i)
	{
		m_rcSlot[i] = { 422 + 71 * i,206,442 + 71 * i,226 };
	}

}
void CSkillSlot::Check_Slot()
{
	for (int i = 0; i < 7; ++i)
	{
		m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_SkillSlot(i);
	}
}
