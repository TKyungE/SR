#include "stdafx.h"
#include "..\Public\Inventory.h"
#include "GameInstance.h"
#include"Layer.h"

CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CInventory::CInventory(const CInventory & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_fSizeX = 300.0f;
	m_fSizeY = 200.0f;
	m_fX = 500.f;
	m_fY = 400.f;

	m_tInfo.vPos.x = m_fX + 138;
	m_tInfo.vPos.y = m_fY - 88;
	m_tInfo.bHit = false;
	m_tInfo.pTarget = nullptr;
	m_tInfo.pTerrain = this;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_XBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &m_tInfo);

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 5; ++i)
		{

			m_tInfo.vPos.x = 402 + i*50.f;
			m_tInfo.vPos.y = 365.f + j*50.f;
			m_tInfo.iHp = i + j * 30 + 1;
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_InvenSlot"), m_tInfo.iLevelIndex, TEXT("Layer_Slot"), &m_tInfo);
			m_vSlot.push_back(pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Slot"))->Get_Objects().back());
		}
	}


	m_lTemp = pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_SkillSlot"))->Get_Objects();
	for (auto& iter : m_lTemp)
	{
		m_vQuick.push_back(iter);
	}
	Safe_Release(pGameInstance);

	m_tInfo.bHit = true;
	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	Change();
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	for (int i = 0; i < 9; ++i)
	{
		RECT		rcRect;
		SetRect(&rcRect, (int)(m_vQuick[i]->Get_Info().vPos.x - 36 * 0.5f), (int)(m_vQuick[i]->Get_Info().vPos.y - 36 * 0.5f), (int)(m_vQuick[i]->Get_Info().vPos.x + 36 * 0.5f), (int)(m_vQuick[i]->Get_Info().vPos.y + 36 * 0.5f));
		if (PtInRect(&rcRect, ptMouse))
		{
			if ((CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON)) && m_bCheck == false && m_vQuick[i]->Get_Info().pTarget != nullptr)
			{
				m_iTemp = m_vQuick[i]->Get_Info().pTarget;
				m_iSour = i;
				m_vQuick[i]->Set_Target(nullptr);
				a = 3;
				m_bCheck = true;

			}
			if ((CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON)) && m_bCheck == true)
			{
				if (a == 3)
				{
					m_vQuick[m_iSour]->Set_Target(m_vQuick[i]->Get_Info().pTarget);
					m_vQuick[i]->Set_Target(m_iTemp);
				}
				else
				{
					a = 0;
					m_vQuick[i]->Set_Target(m_iTemp);
				}
				m_bCheck = false;
			}
		}
		/*if (m_iTemp != nullptr)
		{
		if (m_iTemp->Get_Info().vPos.y > 49 || m_iTemp->Get_Info().vPos.y<0 || m_iTemp->Get_Info().vPos.x<395.f || m_iTemp->Get_Info().vPos.x>805.f)
		{
		m_vQuick[m_iSour]->Set_Target(m_iTemp);
		m_bCheck = false;
		}

		}*/
	}
}
void CInventory::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_tInfo.bHit == false)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);

		m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));//여기가중점
	}
}

HRESULT CInventory::Render()
{
	if (m_tInfo.bHit == false)
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

	}


	return S_OK;
}

HRESULT CInventory::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inventory"), (CComponent**)&m_pTextureCom)))
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

HRESULT CInventory::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CInventory::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CInventory::Change(void)
{
	if (m_tInfo.bHit == false)
	{

		POINT		ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);
		for (int j = 0; j < 3; ++j)
		{
			for (int i = 0; i < 5; ++i)
			{
				RECT		rcRect;
				SetRect(&rcRect, (int)(m_vSlot[i + j * 5]->Get_Info().vPos.x - 36 * 0.5f), (int)(m_vSlot[i + j * 5]->Get_Info().vPos.y - 36 * 0.5f), (int)(m_vSlot[i + j * 5]->Get_Info().vPos.x + 36 * 0.5f), (int)(m_vSlot[i + j * 5]->Get_Info().vPos.y + 36 * 0.5f));
				if (PtInRect(&rcRect, ptMouse))
				{
					if ((CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON)) && m_bCheck == false && m_vSlot[i + j * 5]->Get_Info().pTarget != nullptr)
					{
						m_iTemp = m_vSlot[i + j * 5]->Get_Info().pTarget;
						m_iSour = i + j * 5;
						m_vSlot[i + j * 5]->Set_Target(nullptr);

						m_bCheck = true;

					}
					if ((CKeyMgr::Get_Instance()->Key_Up(VK_LBUTTON)) && m_bCheck == true)
					{
						//만약에 내린곳이 인벤토리와 같은 벡터 번째이면 타겟을 삭제하고 그거를 다시 인벤에 푸쉬백해주기
						if (m_iSour != i + j * 5)

							m_vSlot[m_iSour]->Set_Target(m_vSlot[i + j * 5]->Get_Info().pTarget);
						m_vSlot[i + j * 5]->Set_Target(m_iTemp);
						a = 0;
						m_bCheck = false;
					}
				}
				/*if (m_iTemp != nullptr)
				{
				if (m_iTemp->Get_Info().vPos.y > 49 || m_iTemp->Get_Info().vPos.y<0 || m_iTemp->Get_Info().vPos.x<395.f || m_iTemp->Get_Info().vPos.x>805.f)
				{
				m_vSlot[m_iSour]->Set_Target(m_iTemp);
				//m_iTemp -> Set_Dead();
				m_bCheck = false;
				}
				}*/


			}
		}
	}
}

CInventory * CInventory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory*	pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventory::Clone(void * pArg)
{
	CInventory*	pInstance = new CInventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CInventory::Get_World(void)
{
	return _float4x4();
}

void CInventory::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
