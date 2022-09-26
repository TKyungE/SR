#include "stdafx.h"
#include "..\Public\Town_Quest1.h"
#include "GameInstance.h"
#include "HuntQuest1.h"
#include "TextBox.h"
#include "QuestManager.h"
#include "StatInfo.h"
#include "Layer.h"

CTown_Quest1::CTown_Quest1(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTown_Quest1::CTown_Quest1(const CTown_Quest1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTown_Quest1::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTown_Quest1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 600.f;
	m_fSizeY = 600.f;
	m_fX = 250.f;
	m_fY = 200.f;

	m_pCharTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pCharTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));
	m_pQuestTransformCom->Set_Scaled(_float3(0.6f, 0.6f, 1.f));

	m_tInfo.vPos.y += 0.5f;
	_float3 vQuestPos = m_tInfo.vPos;
	vQuestPos.y += 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_pQuestTransformCom->Set_State(CTransform::STATE_POSITION, vQuestPos);

	m_tInfo.bDead = false;
	m_tInfo.fX = 0.1f;
	m_tFrame.iFrameStart = 0;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 0.7f, 0.7f, 1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);

	Safe_Release(pGameInstance);

	Ready_Script();

	return S_OK;
}

void CTown_Quest1::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	RECT	rcRect;
	SetRect(&rcRect, _int(m_fX - m_fSizeX * 0.5f), _int(m_fY - m_fSizeY * 0.5f), _int(m_fX + m_fSizeX * 0.5f), _int(m_fY + m_fSizeY * 0.5f));

	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);
	m_pQuestColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 2.f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	CQuestManager* pQuestManager = CQuestManager::Get_Instance();
	if (nullptr == pQuestManager)
		return;

	Safe_AddRef(pQuestManager);

	if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_TownQuest1")) && nullptr == pQuestManager->Find_Active(TEXT("Quest_TownQuest1")))
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 6;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vQuestScript.size();

			tTInfo.pScript = new wstring[m_vQuestScript.size()];
			for (_int i = 0; i < m_vQuestScript.size(); ++i)
				tTInfo.pScript[i] = m_vQuestScript[i];

			tTInfo.iQuestIndex = 3;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 6;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
				return;
		}
		else
			m_iQuestTex = 0;
	}
	else if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_TownQuest1")) && nullptr != pQuestManager->Find_Active(TEXT("Quest_TownQuest1")) && !pQuestManager->Find_Active(TEXT("Quest_TownQuest1"))->Get_Clear())
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 6;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vNotClearScript.size();
			tTInfo.pScript = new wstring[m_vNotClearScript.size()];

			for (_int i = 0; i < m_vNotClearScript.size(); ++i)
				tTInfo.pScript[i] = m_vNotClearScript[i];

			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
				return;
		}
		else
			m_iQuestTex = 2;
	}
	else if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_TownQuest1")) && nullptr != pQuestManager->Find_Active(TEXT("Quest_TownQuest1")) && pQuestManager->Find_Active(TEXT("Quest_TownQuest1"))->Get_Clear())
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 6;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vClearScript.size();

			tTInfo.pScript = new wstring[m_vClearScript.size()];
			for (_int i = 0; i < m_vClearScript.size(); ++i)
				tTInfo.pScript[i] = m_vClearScript[i];

			tTInfo.iQuestIndex = 1;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 6;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
				return;
		}
		else
			m_iQuestTex = 1;
	}
	else
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 6;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vNormalScript.size();

			tTInfo.pScript = new wstring[m_vNormalScript.size()];
			for (_int i = 0; i < m_vNormalScript.size(); ++i)
				tTInfo.pScript[i] = m_vNormalScript[i];

			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
				return;
		}
		else
			m_bQuestRender = false;
	}

	if (6 == g_iQuest && nullptr == pQuestManager->Find_Finish(TEXT("Quest_TownQuest1")) && nullptr == pQuestManager->Find_Active(TEXT("Quest_TownQuest1")))
	{
		CHuntQuest::QINFO_DERIVED tQInfo;
		tQInfo.iCount = 3;
		tQInfo.pHuntGoal = new _uint[tQInfo.iCount];
		tQInfo.pHuntGoal[0] = 3;
		tQInfo.pHuntGoal[1] = 3;
		tQInfo.pHuntGoal[2] = 3;

		tQInfo.pMonType = new MONSTERTYPE[tQInfo.iCount];
		tQInfo.pMonType[0] = MON_ATROS;
		tQInfo.pMonType[1] = MON_BAPHOMET;
		tQInfo.pMonType[2] = MON_MINOROUS;

		tQInfo.tQInfo.eType = CClientQuest::QUEST_HUNT;

		if (FAILED(pQuestManager->Add_Quest(TEXT("Prototype_Quest_HuntQuest1"), TEXT("Quest_TownQuest1"), &tQInfo)))
		{
			ERR_MSG(TEXT("Failed to Add Quest : TownQuest1"));
			return;
		}

		g_iQuest = 0;
		g_iReward = 0;
	}

	if (6 == g_iReward && pQuestManager->Find_Active(TEXT("Quest_TownQuest1"))->Get_Clear())
	{
		if (FAILED(pQuestManager->Clear_Quest(TEXT("Quest_TownQuest1"))))
			return;

		m_tInfo.pTarget->Set_Exp(50);

		g_iQuest = 0;
		g_iReward = 0;
	}

	m_pQuestTransformCom->Turn(m_pQuestTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta * 2.f);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_NPC, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CTown_Quest1"));
		return;
	}

	m_fAlpha += 0.006f;

	Safe_Release(pQuestManager);

	Safe_Release(pInstance);
}

void CTown_Quest1::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	OnBillboard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_NONALPHABLEND, this);
	}

	CGameObject* pTarget = nullptr;

	if (pInstance->Collision(this, TEXT("Com_QuestCollider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
	{
		m_bTalk = true;
		g_iTalk = 6;
	}
	else
	{
		m_bTalk = false;
		if (6 == g_iTalk)
			g_iTalk = 0;
	}

	Safe_Release(pInstance);
}

HRESULT CTown_Quest1::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (0 != g_iCut && m_bTalk)
	{
		if (FAILED(m_pCharTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4	WorldMatrix, ViewMatrix;

		WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pCharTransformCom->Get_WorldMatrix());

		D3DXMatrixIdentity(&ViewMatrix);

		_float4x4 SaveViewMatrix, SaveProjVatrix;

		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &SaveViewMatrix);
		m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &SaveProjVatrix);

		/*	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);*/

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Set_Texture("g_Texture", m_pCharTextureCom->Get_Texture(2));

		/*if (FAILED(m_pCharTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;*/

		m_pShaderCom->Begin(1);

		m_pCharVIBufferCom->Render();

		m_pShaderCom->End();

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &SaveViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &SaveProjVatrix);
	}

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tInfo.iMp)))
		return E_FAIL;

	m_pVIBuffer->Render();

	if (FAILED(m_pQuestTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pQuestTextureCom->Bind_OnGraphicDev(m_iQuestTex)))
		return E_FAIL;

	if (m_bQuestRender)
		m_pQuestVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();

	if (g_bCollider)
	{
		m_pColliderCom->Render();
		m_pQuestColliderCom->Render();
	}

	return S_OK;
}

HRESULT CTown_Quest1::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_QuestTexture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuestUI"), (CComponent**)&m_pQuestTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_QuestVIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pQuestVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_QuestCollider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pQuestColliderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_CharVIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pCharVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_CharTexture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TownQuest1"), (CComponent**)&m_pCharTextureCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_QuestTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pQuestTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_CharTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pCharTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTown_Quest1::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CTown_Quest1::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

HRESULT CTown_Quest1::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CTown_Quest1::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

void CTown_Quest1::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}

void CTown_Quest1::Ready_Script(void)
{
	m_vQuestScript.push_back(TEXT("저..저기요...! 혹시 저랑 둘이서 파티 사냥 하실래요?"));
	m_vQuestScript.push_back(TEXT("당연히 뻥이죠! 제가 왜 당신같은 허~졉이랑 파티 사냥을 하겠어요?"));
	m_vQuestScript.push_back(TEXT("허접이 아니라구요? 그럼 증명해보세요!"));
	m_vQuestScript.push_back(TEXT("사막에서 아트로스 3마리, 바포메트 3마리, 미노로스 3마리씩 잡아오면 인정해드릴게요."));

	m_vNotClearScript.push_back(TEXT("왜 아직도 안가셨어요? 허졉이라 '못' 잡는거에요?"));

	m_vNormalScript.push_back(TEXT("저와 파티 사냥을 하시려면 실력을 증명하셔야 할 거에요~"));

	m_vClearScript.push_back(TEXT("세..세상에..! 벌써 다 잡아왔다고요?! 그럴 리가..?"));
	m_vClearScript.push_back(TEXT("저도 못잡은 몬스터들을 잡아왔다니..! 인정할 수 없어요! \n\n보상 : 경험치 100"));
}

CTown_Quest1 * CTown_Quest1::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CTown_Quest1* pInstance = new CTown_Quest1(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTown_Quest1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTown_Quest1::Clone(void * pArg)
{
	CTown_Quest1* pInstance = new CTown_Quest1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTown_Quest1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CTown_Quest1::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CTown_Quest1::Free(void)
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();

	for (auto iter = m_vQuestScript.begin(); iter != m_vQuestScript.end();)
		iter = m_vQuestScript.erase(iter);

	m_vQuestScript.clear();

	for (auto iter = m_vNotClearScript.begin(); iter != m_vNotClearScript.end();)
		iter = m_vNotClearScript.erase(iter);

	m_vNotClearScript.clear();

	__super::Free();

	Safe_Release(m_pQuestTextureCom);
	Safe_Release(m_pQuestColliderCom);
	Safe_Release(m_pQuestVIBufferCom);
	Safe_Release(m_pQuestTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pCharVIBufferCom);
	Safe_Release(m_pCharTransformCom);
	Safe_Release(m_pCharTextureCom);
	Safe_Release(m_pShaderCom);
}
