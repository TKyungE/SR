#include "stdafx.h"
#include "..\Public\Village_Quest2.h"
#include "GameInstance.h"
#include "CollectQuest.h"
#include "TextBox.h"
#include "QuestManager.h"
#include "Player.h"

CVillage_Quest2::CVillage_Quest2(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CVillage_Quest2::CVillage_Quest2(const CVillage_Quest2& rhs)
	: CGameObject(rhs)
{
}

HRESULT CVillage_Quest2::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVillage_Quest2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 400.f;
	m_fSizeY = 400.f;
	m_fX = 250.f;
	m_fY = 370.f;

	m_pCharTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pCharTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));
	m_pQuestTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));

	m_tInfo.vPos.y += 0.5f;
	_float3 vQuestPos = m_tInfo.vPos;
	vQuestPos.y += 0.7f;

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
	tInfo.vPos = { 0.7f,0.7f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
	
	Safe_Release(pGameInstance);

	Ready_Script();

	return S_OK;
}

void CVillage_Quest2::Tick(_float fTimeDelta)
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

	if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_CollectQuest1")) && nullptr == pQuestManager->Find_Active(TEXT("Quest_CollectQuest1")))
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 3;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vQuestScript.size();

			tTInfo.pScript = new wstring[m_vQuestScript.size()];
			for (_int i = 0; i < m_vQuestScript.size(); ++i)
				tTInfo.pScript[i] = m_vQuestScript[i];

			tTInfo.iQuestIndex = 3;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 3;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
				return;
		}
		else
			m_iQuestTex = 0;
	}
	else if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_CollectQuest1")) && nullptr != pQuestManager->Find_Active(TEXT("Quest_CollectQuest1")) && !pQuestManager->Find_Active(TEXT("Quest_CollectQuest1"))->Get_Clear())
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 3;

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
	else if (nullptr == pQuestManager->Find_Finish(TEXT("Quest_CollectQuest1")) && nullptr != pQuestManager->Find_Active(TEXT("Quest_CollectQuest1")) && pQuestManager->Find_Active(TEXT("Quest_CollectQuest1"))->Get_Clear())
	{
		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
		{
			g_iCut = 3;

			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vClearScript.size();

			tTInfo.pScript = new wstring[m_vClearScript.size()];
			for (_int i = 0; i < m_vClearScript.size(); ++i)
				tTInfo.pScript[i] = m_vClearScript[i];

			tTInfo.iQuestIndex = 1;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 3;

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
			g_iCut = 3;

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

	if (3 == g_iQuest && nullptr == pQuestManager->Find_Finish(TEXT("Quest_CollectQuest1")) && nullptr == pQuestManager->Find_Active(TEXT("Quest_CollectQuest1")))
	{
		CCollectQuest::QINFO_DERIVED tQInfo;
		tQInfo.iCount = 3;

		tQInfo.pCollectGoal = new _uint[tQInfo.iCount];
		tQInfo.pCollectGoal[0] = 2;
		tQInfo.pCollectGoal[1] = 2;
		tQInfo.pCollectGoal[2] = 1;
		
		tQInfo.pItemType = new CStatInfo::EITEM[tQInfo.iCount];
		tQInfo.pItemType[0] = CStatInfo::MON2;
		tQInfo.pItemType[1] = CStatInfo::MON1;
		tQInfo.pItemType[2] = CStatInfo::MON3;

		tQInfo.tQInfo.eType = CClientQuest::QUEST_COLLECT;

		if (FAILED(pQuestManager->Add_Quest(TEXT("Prototype_Quest_CollectQuest1"), TEXT("Quest_CollectQuest1"), &tQInfo)))
		{
			ERR_MSG(TEXT("Failed to Add Quest : CollectQuest1"));
			return;
		}

		g_iQuest = 0;
		g_iReward = 0;
	}

	if (3 == g_iReward && pQuestManager->Find_Active(TEXT("Quest_CollectQuest1"))->Get_Clear())
	{
		if (FAILED(pQuestManager->Clear_Quest(TEXT("Quest_CollectQuest1"))))
			return;

		m_tInfo.pTarget->Set_Money(1000);

		g_iQuest = 0;
		g_iReward = 0;
	}

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_NPC, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CVillage_Quest2"));
		return;
	}

	m_fAlpha += 0.006f;

	Safe_Release(pQuestManager);

	Safe_Release(pInstance);
}

void CVillage_Quest2::Late_Tick(_float fTimeDelta)
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
		g_iTalk = 3;
	}
	else
	{
		m_bTalk = false;
		if (3 == g_iTalk)
			g_iTalk = 0;
	}

	Safe_Release(pInstance);
}

HRESULT CVillage_Quest2::Render(void)
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

		m_pShaderCom->Set_Texture("g_Texture", m_pCharTextureCom->Get_Texture(0));

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

HRESULT CVillage_Quest2::SetUp_Components(void)
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

	if (FAILED(__super::Add_Components(TEXT("Com_CharTexture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Quest1"), (CComponent**)&m_pCharTextureCom)))
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

HRESULT CVillage_Quest2::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CVillage_Quest2::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}
HRESULT CVillage_Quest2::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CVillage_Quest2::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

void CVillage_Quest2::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);

	_float3 vScale = m_pQuestTransformCom->Get_Scale();

	m_pQuestTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pQuestTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0] * vScale.z);
}

void CVillage_Quest2::Ready_Script(void)
{
	m_vQuestScript.push_back(TEXT("반갑네. 자네는 모험가인가?"));
	m_vQuestScript.push_back(TEXT("마침 잘됐군. 자네 내 부탁 좀 들어주겠나?"));
	m_vQuestScript.push_back(TEXT("이번에 내린 큰 비로 집을 보수해야 하는데 마침 재료가 전부 떨어졌지 뭔가"));
	m_vQuestScript.push_back(TEXT("저쪽의 포탈을 타고 나가 엘더 윌로우의 몸통 2개와 엘리게이터의 독 2개, 빅풋의 발바닥 1개를 모아주면 사례하겠네."));

	m_vNotClearScript.push_back(TEXT("다음에도 이런 비가 온다면 버티지 못할거야.."));

	m_vNormalScript.push_back(TEXT("안녕하신가."));

	m_vClearScript.push_back(TEXT("벌써 가져왔나? 정말 고맙네!"));
	m_vClearScript.push_back(TEXT("이건 나를 도와준 보상이네. 다시 한 번 정말 고맙네. \n\n보상 : 골드 1000"));
}

CVillage_Quest2 * CVillage_Quest2::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CVillage_Quest2* pInstance = new CVillage_Quest2(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVillage_Quest2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVillage_Quest2::Clone(void * pArg)
{
	CVillage_Quest2* pInstance = new CVillage_Quest2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVillage_Quest2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CVillage_Quest2::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();;
}

void CVillage_Quest2::Free(void)
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