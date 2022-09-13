#include "stdafx.h"
#include "..\Public\QuickSlot.h"
#include"GameInstance.h"
#include"SkillSlot.h"
#include "Layer.h"

CQuickSlot::CQuickSlot(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CQuickSlot::CQuickSlot(const CQuickSlot & rhs)
	: CGameObject(rhs)
{
}

HRESULT CQuickSlot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuickSlot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	m_tInfo.pTarget = nullptr;

	m_fSizeX = 400.0f;
	m_fSizeY = 50.0f;
	m_fX = 600.f;
	m_fY = 25.f;


	m_Pass.fPosX = m_fX + 188;//반지름에서 12(x박스크기의 반값)만큼 추가로 빼야함
	m_Pass.fPosY = m_fY - 13;//반지름에서 12(x박스크기의 반값)만큼 추가로 빼야함
	m_Pass.bNext = false;
	m_tInfo.bHit = m_Pass.bNext;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	//pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_XBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &m_tInfo);
	for (int i = 0; i < 9; ++i)
	{

		m_tInfo.vPos.x = 423 + i*41.5f;
		m_tInfo.vPos.y = 25.f;
		m_tInfo.iHp = i;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkillSlot"), m_tInfo.iLevelIndex, TEXT("Layer_SkillSlot"), &m_tInfo);
		m_vSlot.push_back(pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_SkillSlot"))->Get_Objects().back());
	}
	Safe_Release(pGameInstance);


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CQuickSlot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//Change();
	QuickUse();



}
void CQuickSlot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && !g_bCut)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));//여기가중점
}

HRESULT CQuickSlot::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;




	return S_OK;
}

void CQuickSlot::QuickUse()
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_vSlot[i]->Get_Info().pTarget != nullptr)
		{
			if (i == 0)
			{
				if (CKeyMgr::Get_Instance()->Key_Down('Z'))
				{
					m_vSlot[i]->Get_Info().pTarget->Set_UiExp(2);
				}
			}
			if (i == 1)
			{
				if (CKeyMgr::Get_Instance()->Key_Down('X'))
				{
					m_vSlot[i]->Get_Info().pTarget->Set_UiExp(2);
				}
			}
			if (i == 2)
			{
				if (CKeyMgr::Get_Instance()->Key_Down('C'))
				{
					m_vSlot[i]->Get_Info().pTarget->Set_UiExp(2);
				}

			}
		}

	}
}

HRESULT CQuickSlot::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuickSlot"), (CComponent**)&m_pTextureCom)))
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

HRESULT CQuickSlot::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}
HRESULT CQuickSlot::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CQuickSlot::Change(void)
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	for (int i = 0; i < 9; ++i)
	{
		RECT		rcRect;
		SetRect(&rcRect, (int)(m_vSlot[i]->Get_Info().vPos.x - 36 * 0.5f), (int)(m_vSlot[i]->Get_Info().vPos.y - 36 * 0.5f), (int)(m_vSlot[i]->Get_Info().vPos.x + 36 * 0.5f), (int)(m_vSlot[i]->Get_Info().vPos.y + 36 * 0.5f));
		if (PtInRect(&rcRect, ptMouse))
		{
			if ((CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON)) && m_bCheck == false && m_vSlot[i]->Get_Info().pTarget != nullptr)
			{
				m_iTemp = m_vSlot[i]->Get_Info().pTarget;
				m_iSour = i;
				m_vSlot[i]->Set_Target(nullptr);

				m_bCheck = true;

			}
			if ((CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON)) && m_bCheck == true)
			{
				m_vSlot[m_iSour]->Set_Target(m_vSlot[i]->Get_Info().pTarget);
				m_vSlot[i]->Set_Target(m_iTemp);

				m_bCheck = false;
			}
		}
		if (m_iTemp != nullptr)
		{
			if (m_iTemp->Get_Info().vPos.y > 49 || m_iTemp->Get_Info().vPos.y<0 || m_iTemp->Get_Info().vPos.x<395.f || m_iTemp->Get_Info().vPos.x>805.f)
			{
				m_vSlot[m_iSour]->Set_Target(m_iTemp);
				m_bCheck = false;
			}

		}
	}
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

CGameObject * CQuickSlot::Clone(void * pArg)
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

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
