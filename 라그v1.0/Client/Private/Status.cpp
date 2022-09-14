#include "stdafx.h"
#include "..\Public\Status.h"
#include "GameInstance.h"
#include "KeyMgr.h"
#include "UI.h"
#include "Player.h"
#include "Layer.h"
#include "StatInfo.h"

CStatus::CStatus(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStatus::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 300.f;
	m_fSizeY = 550.f;
	m_fX = 500.f;
	m_fY = 350.f;
	m_rcStat[0] = { 460,260,700,300 };
	m_rcStat[1] = { 460,302,700,340 };
	m_rcStat[2] = { 460,332,700,365 };
	m_rcStat[3] = { 455,400,600,450 };
	m_rcStat[4] = { 460,445,700,500 };
	m_rcStat[5] = { 460,475,700,500 };
	m_rcStat[6] = { 460,505,700,550 };
	m_rcStat[7] = { 460,535,700,580 };

	m_rcButton[0] = { 600,445,620,465 };
	m_rcButton[1] = { 600,475,620,495 };
	m_rcButton[2] = { 600,504,620,524 };
	m_rcButton[3] = { 600,533,620,553 };

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	_float fSizeX = 20.f;
	_float fSizeY = 20.f;
	_float fX = 610.f;
	_float fY = 455.f;

	m_pTransformCom2->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
	fY = 485.f;
	m_pTransformCom3->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
	m_pTransformCom3->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
	fY = 514.f;
	m_pTransformCom4->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
	m_pTransformCom4->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));
	fY = 543.f;
	m_pTransformCom5->Set_Scaled(_float3(fSizeX, fSizeY, 1.f));
	m_pTransformCom5->Set_State(CTransform::STATE_POSITION, _float3(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

void CStatus::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (dynamic_cast<CUI*>(m_tInfo.pTerrain)->Get_Status() == false)
		Set_Dead();

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	for (int i = 0; i < 4; ++i)
	{
		if (PtInRect(&m_rcButton[i], ptMouse))
		{
			if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
			{
				switch (i)
				{
				case 0:
					if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iStatsPoint > 0)
					{
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_STR(1);
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_StatsPoint(-1);
					}
					break;
				case 1:
					if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iStatsPoint > 0)
					{
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_DEX(1);
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_StatsPoint(-1);
					}
					break;
				case 2:
					if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iStatsPoint > 0)
					{
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_INT(1);
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_StatsPoint(-1);
					}
					break;
				case 3:
					if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iStatsPoint > 0)
					{
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_LUK(1);
						dynamic_cast<CStatInfo*>(m_StatInfo)->Set_StatsPoint(-1);
					}
					break;
				}
			}
		}
	}

}

void CStatus::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStatus::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferCom->Render();

	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;
	m_pVIBufferCom2->Render();

	if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
		return E_FAIL;
	m_pVIBufferCom3->Render();

	if (FAILED(m_pTransformCom4->Bind_OnGraphicDev()))
		return E_FAIL;
	m_pVIBufferCom4->Render();

	if (FAILED(m_pTransformCom5->Bind_OnGraphicDev()))
		return E_FAIL;
	m_pVIBufferCom5->Render();


	wstring szDmg = L"";
	wstring szHp = L"";
	wstring szMp = L"";
	wstring szPoint = L"";
	wstring szSTR = L"";
	wstring szDEX = L"";
	wstring szINT = L"";
	wstring szLUK = L"";

	szDmg = to_wstring(m_tInfo.pTarget->Get_Info().iDmg);
	szHp = to_wstring(m_tInfo.pTarget->Get_Info().iMaxHp);
	szMp = to_wstring(m_tInfo.pTarget->Get_Info().iMaxMp);
	szPoint = to_wstring(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iStatsPoint);
	szSTR = to_wstring(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iSTR);
	szDEX = to_wstring(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iDEX);
	szINT = to_wstring(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iINT);
	szLUK = to_wstring(dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iLUK);

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Get_Font()->DrawText(nullptr, szDmg.c_str(), (int)szDmg.length(), &m_rcStat[0], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szHp.c_str(), (int)szHp.length(), &m_rcStat[1], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szMp.c_str(), (int)szMp.length(), &m_rcStat[2], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szPoint.c_str(), (int)szPoint.length(), &m_rcStat[3], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szSTR.c_str(), (int)szSTR.length(), &m_rcStat[4], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szDEX.c_str(), (int)szDEX.length(), &m_rcStat[5], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szINT.c_str(), (int)szINT.length(), &m_rcStat[6], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
	pGameInstance->Get_Font()->DrawText(nullptr, szLUK.c_str(), (int)szLUK.length(), &m_rcStat[7], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));

	Safe_Release(pGameInstance);

	if (FAILED(Release_RenderState()))
		return E_FAIL;


	return S_OK;
}

HRESULT CStatus::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Status"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer4"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom4)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer5"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom5)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform4"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom4, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform5"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom5, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatus::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CStatus::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CStatus * CStatus::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStatus*	pInstance = new CStatus(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CStatus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatus::Clone(void* pArg)
{
	CStatus*	pInstance = new CStatus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CStatus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CStatus::Get_World(void)
{
	return _float4x4();
}

void CStatus::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pTransformCom4);
	Safe_Release(m_pTransformCom5);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pVIBufferCom4);
	Safe_Release(m_pVIBufferCom5);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
