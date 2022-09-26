#include "stdafx.h"
#include "..\Public\SkyNPC.h"
#include "GameInstance.h"
#include "TextBox.h"
#include "Player.h"
#include "Layer.h"
#include "SoundMgr.h"
#include "QuestManager.h"


CSkyNPC::CSkyNPC(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSkyNPC::CSkyNPC(const CSkyNPC & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyNPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyNPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX;
	m_fY = g_iWinSizeY;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 9;
	m_tFrame.fFrameSpeed = 0.1f;
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));
	
	m_fSizeX = 360.f;
	m_fSizeY = 470.f;
	m_fX = 280.f;
	m_fY = 234.f;

	m_pTransformCom2->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	m_fX = 1000.f;
	m_pTransformCom3->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom3->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_pMouseTransformCom->Set_Scaled(_float3(22.f, 31.f, 1.f));

	Ready_Script();
	return S_OK;
}

void CSkyNPC::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float3 vMousePos = { (float)(ptMouse.x + 9.f) - g_iWinSizeX * 0.5f, -(float)(ptMouse.y + 13.f) + g_iWinSizeY * 0.5f, 0.f };
	m_pMouseTransformCom->Set_State(CTransform::STATE_POSITION, vMousePos);
	Move_Frame(fTimeDelta);
	m_fCool += fTimeDelta;
	RECT		rcRect;
	SetRect(&rcRect, (int)(m_fX - m_fSizeX * 0.5f), (int)(m_fY - m_fSizeY * 0.5f), (int)(m_fX + m_fSizeX * 0.5f), (int)(m_fY + m_fSizeY * 0.5f));

	if (m_fCool > 2.f && !m_bTrue)
	{
		CSoundMgr::Get_Instance()->BGM_Stop();
		CSoundMgr::Get_Instance()->PlayBGM(L"Warning.wav", fSOUND);
		CTextBox::TINFO tTInfo;
		tTInfo.iScriptSize = (_int)m_vNormalScript.size();
		tTInfo.pScript = new wstring[m_vNormalScript.size()];

		for (_int i = 0; i < m_vNormalScript.size(); ++i)
			tTInfo.pScript[i] = m_vNormalScript[i];

		tTInfo.iQuestIndex = 7;
		tTInfo.iLevelIndex = LEVEL_LOADING;
		tTInfo.iNumQuest = 8; 

		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_LOADING, TEXT("Layer_SkyText"), &tTInfo)))
			return;
		Safe_Release(pGameInstance);
		m_bTrue = true;
	}

	CQuestManager* pQuestManager = CQuestManager::Get_Instance();
	if (nullptr == pQuestManager)
		return;

	Safe_AddRef(pQuestManager);

	if (8 == g_iQuest && nullptr == pQuestManager->Find_Finish(TEXT("Quest_HuntQuestSky")) && nullptr == pQuestManager->Find_Active(TEXT("Quest_HuntQuestSky")))
	{
		CHuntQuest::QINFO_DERIVED tQInfo;
		tQInfo.iCount = 2;

		tQInfo.pHuntGoal = new _uint[tQInfo.iCount];
		tQInfo.pHuntGoal[0] = 10;
		tQInfo.pHuntGoal[1] = 1;

		tQInfo.pMonType = new MONSTERTYPE[tQInfo.iCount];
		tQInfo.pMonType[0] = MON_WYVERN;
		tQInfo.pMonType[1] = MON_SKYDRAGON;

		tQInfo.tQInfo.eType = CClientQuest::QUEST_HUNT;

		if (FAILED(pQuestManager->Add_Quest(TEXT("Prototype_Quest_HuntQuest1"), TEXT("Quest_HuntQuestSky"), &tQInfo)))
		{
			ERR_MSG(TEXT("Failed to Add Quest : HuntQuestSky"));
			return;
		}
		if (!g_bSky)
			g_bSky = true;
		g_iQuest = 0;
		g_iReward = 0;
	}

	Safe_Release(pQuestManager);
	
	Check_Index();
}

void CSkyNPC::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MOUSE, this);
}

HRESULT CSkyNPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;


	if (m_bTrue)
	{
		if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_iNPC)))
			return E_FAIL;
		m_pVIBufferCom2->Render();
	}
	if (m_bTrue2)
	{
		if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
			return E_FAIL;
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_iNPC2)))
			return E_FAIL;
		m_pVIBufferCom3->Render();
	}

	if (FAILED(m_pMouseTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pMouseTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
		return E_FAIL;
	m_pMouseBuffer->Render();


	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CSkyNPC::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyNPC"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), (CComponent**)&m_pMouseTextureCom)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
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
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform88"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pMouseTransformCom, &TransformDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CSkyNPC::SetUp_RenderState()
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

HRESULT CSkyNPC::Release_RenderState()
{
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CSkyNPC * CSkyNPC::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkyNPC*	pInstance = new CSkyNPC(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkyNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyNPC::Clone(void* pArg)
{
	CSkyNPC*	pInstance = new CSkyNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkyNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CSkyNPC::Get_World(void)
{
	return _float4x4();
}

void CSkyNPC::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMouseTextureCom);
	Safe_Release(m_pMouseTransformCom);
	Safe_Release(m_pMouseBuffer);
}
void CSkyNPC::Ready_Script(void)
{
	m_vNormalScript.push_back(TEXT("성준: 크...큰일 났습니다!! 전방에 와이번 무리들이 다가오고 있습니다!!"));
	m_vNormalScript.push_back(TEXT("병규: 와이번 몇 마리 가지고 뭔 난리야!! 가서 코드 한줄이라도 더 치고 있어!"));
	m_vNormalScript.push_back(TEXT("병규: 오늘까지 UI 안끝나면 집에 못 갈 줄 알아!"));
	m_vNormalScript.push_back(TEXT("성준: ...(저 ㅆ...&^#@$%#&$*^$%@#$@^&##@#!!!!)"));
	m_vNormalScript.push_back(TEXT("석훈타릭: 비...비상!! 진짜 비상!! 와이번 무리들 사이에 뽜이어드래곤이 같이 있어!!"));
	m_vNormalScript.push_back(TEXT("석훈타릭: 내가 어떻게 고친 비행정인데...이렇게 침몰 될 순 없어!!"));
	m_vNormalScript.push_back(TEXT("석훈타릭: 너가 빨리 가서 저 와이번무리들과 뽜이어드래곤을 물리치라고!!"));
	m_vNormalScript.push_back(TEXT("석훈타릭: (저 녀석이 시간을 끄는 사이 튀어야겠다...뽜이어드래곤을 이기는건 무리라고!)"));
}

void CSkyNPC::Check_Index()
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Find_Layer(LEVEL_LOADING, TEXT("Layer_SkyText")) != nullptr && m_iNPC != 6)
	{
		switch (dynamic_cast<CTextBox*>(pGameInstance->Find_Layer(LEVEL_LOADING, TEXT("Layer_SkyText"))->Get_Objects().front())->Get_ScriptIndex())
		{
		case 0:
			m_iNPC = 4;
			break;
		case 1:
			m_bTrue2 = true;
			m_iNPC2 = 0;
			break;
		case 2:
			m_iNPC2 = 1;
			m_iNPC = 5;
			break;
		case 3:
			m_iNPC = 3;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			m_iNPC = 6;
			break;
		default:
			break;
		}
	}
	Safe_Release(pGameInstance);
}
void CSkyNPC::Move_Frame(_float fTimeDelta)
{
	m_tFrame.iFrameStart = m_pMouseTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
}