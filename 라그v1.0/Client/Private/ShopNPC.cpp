#include "stdafx.h"
#include "..\Public\ShopNPC.h"
#include "GameInstance.h"
#include "HuntQuest1.h"
#include "TextBox.h"
#include "QuestManager.h"
#include "Player.h"
#include "Layer.h"
#include "StatInfo.h"

CShopNPC::CShopNPC(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CShopNPC::CShopNPC(const CShopNPC& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShopNPC::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopNPC::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	
	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 400.f;
	m_fSizeY = 400.f;
	m_fX = 250.f;
	m_fY = 370.f;


	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));


	m_tInfo.vPos.y -= 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_tInfo.bDead = false;
	m_tInfo.iHp = 1;
	m_tInfo.fX = 0.1f;
	m_tFrame.iFrameStart = 0;

	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 0.7f, 0.7f, 1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);

	Safe_Release(pGameInstance);

	Ready_Script();

	return S_OK;
}

void CShopNPC::Tick(_float fTimeDelta)
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



	if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut)
	{
		g_iCut = 4;

		CTextBox::TINFO tTInfo;
		tTInfo.iScriptSize = (_int)m_vNormalScript.size();
		tTInfo.pScript = new wstring[m_vNormalScript.size()];

		for (_int i = 0; i < m_vNormalScript.size(); ++i)
			tTInfo.pScript[i] = m_vNormalScript[i];

		tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_ShopBox"), &tTInfo)))
			return;
	}

	if (CKeyMgr::Get_Instance()->Key_Down('J') && g_iCut != 0 && m_bSlot)
	{
		CGameObject::INFO tInfo;

		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		tInfo.bDead = false;
		tInfo.pTarget = m_tInfo.pTarget;
		tInfo.pTerrain = this;
		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_Shop"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tInfo)))
			return;
		dynamic_cast<CPlayer*>(m_tInfo.pTarget)->Set_UI(true);
	
		m_bShop = true;
		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_Shop(true);
		dynamic_cast<CTextBox*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_ShopBox"))->Get_Objects().front())->TextBoxDead();
		m_bSlot = false;
	}
	if (GetKeyState('H') & 8000 && g_iCut == 0 && !m_bSlot)
	{
		m_bShop = false;
	}
	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_NPC, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CShopNPC"));
		return;
	}


	Safe_Release(pInstance);
}

void CShopNPC::Late_Tick(_float fTimeDelta)
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
		m_bSlot = true;
		m_bTalk = true;
		g_iTalk = 4;
	}
	else
	{
		m_bTalk = false;
		if (4 == g_iTalk)
			g_iTalk = 0;
		m_bSlot = false;
	}

	Safe_Release(pInstance);
}

HRESULT CShopNPC::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(10)))
		return E_FAIL;

	m_pVIBuffer->Render();



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

HRESULT CShopNPC::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_QuestCollider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pQuestColliderCom)))
		return E_FAIL;




	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;




	return S_OK;
}

HRESULT CShopNPC::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CShopNPC::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}
HRESULT CShopNPC::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CShopNPC::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

void CShopNPC::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);


}

void CShopNPC::Ready_Script(void)
{

	m_vNormalScript.push_back(TEXT("필요 한게 있나? 마을을 나서려면 단단히 준비하고 가라고!"));
	m_vNormalScript.push_back(TEXT("상점을 이용하시겠습니까?"));

}

CShopNPC * CShopNPC::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CShopNPC* pInstance = new CShopNPC(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShopNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopNPC::Clone(void * pArg)
{
	CShopNPC* pInstance = new CShopNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShopNPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CShopNPC::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();;
}

void CShopNPC::Free(void)
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();



	__super::Free();

	Safe_Release(m_pQuestColliderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);

}