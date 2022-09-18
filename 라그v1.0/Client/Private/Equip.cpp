#include "stdafx.h"
#include "..\Public\Equip.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "Player.h"
#include "Layer.h"
#include "UI.h"

CEquip::CEquip(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotTrans[i] = nullptr;
		m_pSlotBuffer[i] = nullptr;
	}
}

CEquip::CEquip(const CEquip & rhs)
	: CGameObject(rhs)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pSlotBuffer[i] = rhs.m_pSlotBuffer[i];
		m_pSlotTrans[i] = rhs.m_pSlotTrans[i];
	}
}

HRESULT CEquip::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquip::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	m_fSizeX = 350.f;
	m_fSizeY = 225.f;
	m_fX = 300.f;
	m_fY = 200.f;
	m_pvecItem.reserve(10);
	for (int i = 0; i < 10; ++i)
	{
		m_pvecItem.push_back({ CStatInfo::EITEM_END,0,0 });
	}
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	Set_Slot();

	return S_OK;
}

void CEquip::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CUI*>(m_tInfo.pTerrain)->Get_Equip() == false)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(false);
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_MousePick(false);
		Set_Dead();
		return;
	}

	Check_Slot();
	
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	for (int i = 0; i < 10; ++i)
	{
		if (PtInRect(&m_rcSlot[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
			{
				if (m_pvecItem[i].eItemNum != CStatInfo::EITEM_END && !dynamic_cast<CStatInfo*>(m_StatInfo)->Get_MousePick())
				{
					
					for (int j = 0; j < 24; ++j)
					{
						if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(j).eItemNum == CStatInfo::EITEM_END)
						{
							m_pvecItem[i].iSlotNum = j;
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_InvenItem(m_pvecItem[i], j);
							dynamic_cast<CStatInfo*>(m_StatInfo)->Set_EquipSlot({ CStatInfo::EITEM_END,i,0 }, i);
							m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(i);
							return;
						}
					}
				}
			}
		}
	}

}

void CEquip::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquip::Render()
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

	for (int i = 0; i < 10; ++i)
	{
		if (m_pvecItem[i].eItemNum == CStatInfo::EITEM_END)
			continue;
		if (FAILED(m_pItemTexture->Bind_OnGraphicDev(m_pvecItem[i].eItemNum)))
			return E_FAIL;
		m_pSlotTrans[i]->Bind_OnGraphicDev();
		m_pSlotBuffer[i]->Render();
	}


	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CEquip::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equip"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pItemTexture)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_WingRect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
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

HRESULT CEquip::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CEquip::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CEquip * CEquip::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEquip*	pInstance = new CEquip(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEquip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquip::Clone(void* pArg)
{
	CEquip*	pInstance = new CEquip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEquip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CEquip::Get_World(void)
{
	return _float4x4();
}

void CEquip::Free()
{
	__super::Free();

	for (int i = 0; i < 10; ++i)
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
void CEquip::Set_Slot()
{
	_float fSizeX = 40.f;
	_float fSizeY = 40.f;
	_float fX;
	_float fY;
	_int k = 0;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			fX = 322.f + 305.f * j;
			fY = 132.f + 40.f * i;
			m_pSlotTrans[k]->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
			m_pSlotTrans[k]->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
			++k;
		}
	}
	k = 0;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			m_rcSlot[k] = { int(302.f + 305.f * j),int(112.f + 40.f * i),int(342.f + 305.f * j),int(152.f + 40.f * i) };
			++k;
		}
	}
}
void CEquip::Check_Slot()
{
	for (int i = 0; i < 10; ++i)
	{
		m_pvecItem[i] = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_EquipSlot(i);
	}
}

