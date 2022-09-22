#include "stdafx.h"
#include "..\Public\QuestUI.h"
#include "GameInstance.h"
#include "QuestManager.h"

CQuestUI::CQuestUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CQuestUI::CQuestUI(const CQuestUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CQuestUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestUI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX * 0.2f;
	m_fX = g_iWinSizeX * 1.1f;
	m_fY = g_iWinSizeY * 0.5f;

	return S_OK;
}

void CQuestUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pString)
		Safe_Delete_Array(m_pString);

	if (nullptr != m_pProcessing)
		Safe_Delete_Array(m_pProcessing);

	if (nullptr != m_pRect)
		Safe_Delete_Array(m_pRect);

	CQuestManager* pQuestManager = CQuestManager::Get_Instance();
	if (nullptr == pQuestManager)
		return;
	
	Safe_AddRef(pQuestManager);

	if (0 != m_iQuestCount)
		m_iQuestCount = 0;

	for (auto& Pair : pQuestManager->Get_Actives())
		++m_iQuestCount;

	if (m_iQuestCount > 0)
		m_fSizeY = (_float)g_iWinSizeY * (0.15f * m_iQuestCount);
	else
		m_fSizeY = (_float)g_iWinSizeY * 0.3f;
	
	m_pString = new wstring[m_iQuestCount];
	m_pProcessing = new wstring[m_iQuestCount];
	m_pRect = new RECT[m_iQuestCount];

	_int j = 0;
	for (_int i = m_iQuestCount - 1; i >= 0; --i)
	{
		m_pRect[j] = { 1035, _long(325 - (275 * (0.25f * i))), 1300, _long(575 - (275 * (0.25f * i))) };
		++j;
	}

	_int i = 0;
	for (auto& Pair : pQuestManager->Get_Actives())
	{
		wstring wQuest;
		switch (Pair.second->Get_QInfo().eType)
		{
		case CClientQuest::QUEST_HUNT:
		{
			m_iCount = dynamic_cast<CHuntQuest*>(Pair.second)->Get_QInfoDerived().iCount;
			if (Pair.second->Get_Clear())
			{
				m_pProcessing[i] = TEXT("[완료]");
				wQuest = TEXT("         목표 사냥\n");
			}
			else
			{
				m_pProcessing[i] = TEXT("[진행중]");
				wQuest = TEXT("            목표 사냥\n");
				for (_int i = 0; i < m_iCount; ++i)
				{
					switch (dynamic_cast<CHuntQuest*>(Pair.second)->Get_QInfoDerived().pMonType[i])
					{
					case MON_ALLIGATOR:
						wQuest += TEXT("악어 : ");
						break;
					case MON_ELDERWILOW:
						wQuest += TEXT("엘더 윌로우 : ");
						break;
					case MON_BIGFOOT:
						wQuest += TEXT("빅풋 : ");
						break;
					case MON_BYORGUE:
						wQuest += TEXT("뵤르그 : ");
						break;
					case MON_BLOODYMURDERER:
						wQuest += TEXT("피 묻은 살인자 : ");
						break;
					case MON_DANDELION:
						wQuest += TEXT("단델라이온 : ");
						break;
					case MON_ATROS:
						wQuest += TEXT("아트로스 : ");
						break;
					case MON_BAPHOMET:
						wQuest += TEXT("바포메트 : ");
						break;
					case MON_MINOROUS:
						wQuest += TEXT("미노로스 : ");
						break;
					case MON_SKELETON:
						wQuest += TEXT("스켈레톤 : ");
						break;
					case MON_WRAITH:
						wQuest += TEXT("레이스 : ");
						break;
					case MON_ZOMBIE:
						wQuest += TEXT("좀비 : ");
						break;
					}

					wQuest += to_wstring(dynamic_cast<CHuntQuest*>(Pair.second)->Get_Count()[i]);
					wQuest += TEXT(" / ");
					wQuest += to_wstring(dynamic_cast<CHuntQuest*>(Pair.second)->Get_QInfoDerived().pHuntGoal[i]);
					wQuest += TEXT("\n");
				}
			}
			m_pString[i] = wQuest;
			break;
		}
		case CClientQuest::QUEST_COLLECT:
		{
			m_iCount = dynamic_cast<CCollectQuest*>(Pair.second)->Get_QInfoDerived().iCount;
			if (Pair.second->Get_Clear())
			{
				m_pProcessing[i] = TEXT("[완료]");
				wQuest = TEXT("         아이템 수집\n");
			}
			else
			{
				m_pProcessing[i] = TEXT("[진행중]");
				wQuest = TEXT("            아이템 수집\n");
				for (_int i = 0; i < m_iCount; ++i)
				{
					switch (dynamic_cast<CCollectQuest*>(Pair.second)->Get_QInfoDerived().pItemType[i])
					{
					case CStatInfo::HPPOTION:
						wQuest += TEXT("빨강포션 : ");
						break;
					case CStatInfo::MPPOTION:
						wQuest += TEXT("파랑포션 : ");
						break;
					case CStatInfo::ENGINE:
						wQuest += TEXT("비행정엔진 : ");
						break;
					case CStatInfo::TIARA:
						wQuest += TEXT("쥬신티아라 : ");
						break;
					case CStatInfo::BOBY:
						wQuest += TEXT("쥬신타이즈 : ");
						break;
					case CStatInfo::SHOES:
						wQuest += TEXT("쥬신슈즈 : ");
						break;
					case CStatInfo::ROBE:
						wQuest += TEXT("쥬신로브 : ");
						break;
					case CStatInfo::PANDANT:
						wQuest += TEXT("쥬신팬던트 : ");
						break;
					case CStatInfo::EARRING:
						wQuest += TEXT("쥬신귀걸이 : ");
						break;
					case CStatInfo::BRACELET:
						wQuest += TEXT("쥬신팔찌 : ");
						break;
					case CStatInfo::RING:
						wQuest += TEXT("쥬신반지 : ");
						break;
					case CStatInfo::STAFF:
						wQuest += TEXT("쥬신스태프 : ");
						break;
					case CStatInfo::ORB:
						wQuest += TEXT("쥬신오브 : ");
						break;
					case CStatInfo::RIDEEGG:
						wQuest += TEXT("알파카알 : ");
						break;
					case CStatInfo::PETEGG:
						wQuest += TEXT("포링알 : ");
						break;
					case CStatInfo::WING:
						wQuest += TEXT("요정날개 : ");
						break;
					case CStatInfo::MON1:
						wQuest += TEXT("엘리게이터의 독 : ");
						break;
					case CStatInfo::MON2:
						wQuest += TEXT("윌로우의 몸통 : ");
						break;
					case CStatInfo::MON3:
						wQuest += TEXT("빅풋의 발바닥 : ");
						break;
					case CStatInfo::MON4:
						wQuest += TEXT("도적의 두건 : ");
						break;
					case CStatInfo::MON5:
						wQuest += TEXT("살인자의 식칼 : ");
						break;
					case CStatInfo::MON6:
						wQuest += TEXT("썩은 붕대 : ");
						break;
					case CStatInfo::MON7:
						wQuest += TEXT("아트로스의 발톱 : ");
						break;
					case CStatInfo::MON8:
						wQuest += TEXT("바포메트의 뚜껑: ");
						break;
					case CStatInfo::MON9:
						wQuest += TEXT("미노로스의 우유 : ");
						break;
					case CStatInfo::MON10:
						wQuest += TEXT("해골바가지 : ");
						break;
					case CStatInfo::MON11:
						wQuest += TEXT("레이스의 틀니 : ");
						break;
					case CStatInfo::MON12:
						wQuest += TEXT("좀비 고기 : ");
						break;
					}

					wQuest += to_wstring(dynamic_cast<CCollectQuest*>(Pair.second)->Get_Count()[i]);
					wQuest += TEXT(" / ");
					wQuest += to_wstring(dynamic_cast<CCollectQuest*>(Pair.second)->Get_QInfoDerived().pCollectGoal[i]);
					wQuest += TEXT("\n");
				}
			}
			m_pString[i] = wQuest;
			break;
		}
		}

		++i;
	}

	Safe_Release(pQuestManager);

	if (0 < m_iQuestCount)
	{
		if (g_iWinSizeX * 0.9f < m_fX)
			m_fX -= g_iWinSizeX * 0.005f;
		else
			m_bTextRender = true;
	}

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
}

void CQuestUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && 0 < m_iQuestCount)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CQuestUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4 WorldMatrix, ViewMatrix;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pShaderCom->Begin(8);

	m_pVIBufferCom->Render();

	m_pShaderCom->End();

	if (m_bTextRender)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();
		if (nullptr == pInstance)
			return E_FAIL;

		Safe_AddRef(pInstance);

		for (_int i = 0; i < m_iQuestCount; ++i)
		{
			pInstance->Get_Font2()->DrawText(nullptr, m_pProcessing[i].c_str(), (_int)m_pProcessing[i].length(), &m_pRect[i], DT_LEFT, D3DCOLOR_ARGB(255, 255, 150, 0));
			pInstance->Get_Font2()->DrawText(nullptr, m_pString[i].c_str(), (_int)m_pString[i].length(), &m_pRect[i], DT_LEFT, D3DCOLOR_ARGB(255, 0, 0, 0));
		}

		Safe_Release(pInstance);
	}

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestUI::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextBox"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestUI::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CQuestUI::Release_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

HRESULT CQuestUI::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CQuestUI::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

CQuestUI * CQuestUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQuestUI*	pInstance = new CQuestUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CQuestUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuestUI::Clone(void * pArg)
{
	CQuestUI*	pInstance = new CQuestUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CQuestUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CQuestUI::Get_World(void)
{
	return _float4x4();
}

void CQuestUI::Free()
{
	if (nullptr != m_pString)
		Safe_Delete_Array(m_pString);

	if (nullptr != m_pProcessing)
		Safe_Delete_Array(m_pProcessing);

	if (nullptr != m_pRect)
		Safe_Delete_Array(m_pRect);

	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
