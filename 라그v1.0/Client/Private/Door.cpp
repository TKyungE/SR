#include "stdafx.h"
#include "..\Public\Door.h"

#include "GameInstance.h"
#include "TextBox.h"
#include "Layer.h"
#include "SoundMgr.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CDoor::CDoor(const CDoor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDoor::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoor::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_IndexPos, pArg, sizeof(INDEXPOS));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_tInfo.iLevelIndex = m_IndexPos.iLevelIndex;


	m_pTransformCom->Set_Scaled(_float3(3.f,3.f,0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_IndexPos.vPos);

	if (m_IndexPos.iIndex == 1)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 1.f);
	/*	m_pColliderCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 1.f);
		m_pQuestColliderCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 1.f);*/

	}

	Ready_Script();

	m_bCheck = true;		//  << 이걸로 열쇠 유무 확인 할 생각임.

	return S_OK;
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_IndexPos.iIndex == 0)
	{
		_float4x4 WorldMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.m[3][0] += 1.5f;
		WorldMatrix.m[3][1] += 0.5f * 3.f;

		m_pQuestColliderCom->Set_Transform(WorldMatrix, 2.f);

		m_pColliderCom->Set_Transform(WorldMatrix, 1.f);

		CGameInstance*	pInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pInstance);

		if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut && !m_bCheck)		// 열쇠 없으면
		{
			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vNormalScript.size();
			tTInfo.pScript = new wstring[m_vNormalScript.size()];

			for (_int i = 0; i < m_vNormalScript.size(); ++i)
				tTInfo.pScript[i] = m_vNormalScript[i];

			tTInfo.iQuestIndex = 0;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 50;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_DoorText"), &tTInfo)))
				return;

			m_bTalk = false;
			m_bFirst = true;
		}
		else if ((GetKeyState(VK_SPACE) < 0) && m_bTalk && 0 == g_iCut && m_bCheck)	//열쇠 있으면
		{
			CTextBox::TINFO tTInfo;
			tTInfo.iScriptSize = (_int)m_vFindScript.size();
			tTInfo.pScript = new wstring[m_vFindScript.size()];

			for (_int i = 0; i < m_vFindScript.size(); ++i)
				tTInfo.pScript[i] = m_vFindScript[i];

			tTInfo.iQuestIndex = 0;
			tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
			tTInfo.iNumQuest = 50;

			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_DoorText"), &tTInfo)))
				return;

			g_bCheck = true;
			m_bTalk = false;
			m_bFirst = true;
		}


		// 50 
		if (g_iQuest == 50 && m_iCount < 99 && m_bCheck)
		{
			g_iTalk = 0;
			m_pColliderCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 0.01f);
			m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), 0.01f);

			if (m_iCount == 0)
				CSoundMgr::Get_Instance()->PlayEffect(L"Door_Crack.wav", fSOUND + 0.2f);
			

			++m_iCount;
		}

		if (m_iCount  >= 99 && m_iDustCount < 30)
		{
			CGameObject::INFO Info2;
			ZeroMemory(&Info2, sizeof(CGameObject::INFO));

			Info2.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION); 
			Info2.vPos.x += 0.5f;

			_uint iX = rand() % 30;
			_uint iY = rand() % 3;
			_uint iZ = rand() % 30;
			
			Info2.vPos.x += (_float)iX * 0.1f;
			Info2.vPos.y += (_float)iY * 0.1f;
			Info2.vPos.z += (_float)iZ * 0.1f;


			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_Dust"), m_tInfo.iLevelIndex, TEXT("Layer_Dust"), &Info2)))
				return;

			if (m_iDustCount == 0)
				CSoundMgr::Get_Instance()->PlayEffect(L"Door_Effect.wav", fSOUND + 0.1f);
			


			++m_iDustCount;
		}

		if (m_iCount < 1)	//회전 시 충돌 안할 것.
		{
			if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_OBJECT, this)))
			{
				ERR_MSG(TEXT("Failed to Add CollisionGroup : CDoor"));
				return;
			}
		}
		Safe_Release(pInstance);
	}


	else if (m_IndexPos.iIndex == 1)
	{
		_float4x4 WorldMatrix;
		WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		WorldMatrix.m[0][0] = 0.2f;
		WorldMatrix.m[1][1] += 1.f;
		WorldMatrix.m[2][2] = 4.f;
		WorldMatrix.m[3][1] += 0.5f * 3.f;
		WorldMatrix.m[3][2] -= 1.5f;
		m_pColliderCom->Set_Transform(WorldMatrix, 0.5f);


		_float4x4 WorldMatrix2;
		WorldMatrix2 = m_pTransformCom->Get_WorldMatrix();

		WorldMatrix2.m[0][0] = 5.f;
		WorldMatrix2.m[1][1] = 15.f;
		WorldMatrix2.m[2][2] = 15.f;
		WorldMatrix2.m[3][0] += 1.5f;
		WorldMatrix2.m[3][1] = 0.5f;
		WorldMatrix2.m[3][2] -= 1.5f;

		m_pQuestColliderCom->Set_Transform(WorldMatrix2, 0.1f);

		CGameInstance*	pInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pInstance);

		if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_OBJECT, this)))
		{
			ERR_MSG(TEXT("Failed to Add CollisionGroup : CDoor"));
			return;
		}

		Safe_Release(pInstance);
	}

	if (m_IndexPos.iIndex == 1 && m_bTalk && m_iCount < 1)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.y -= fTimeDelta * 1.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		if (vPos.y <= 0.f)
		{
			m_iCount = 2;
		}
	}

	if (m_IndexPos.iIndex == 1 && m_iCount == 2 && m_iDustCount < 20)
	{
		CGameObject::INFO Info2;
		ZeroMemory(&Info2, sizeof(CGameObject::INFO));

		Info2.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Info2.vPos.x += 0.5f;
		Info2.vPos.z -= 3.5f;

		_uint iX = rand() % 2;
		_uint iY = rand() % 3;
		_uint iZ = rand() % 30;

		Info2.vPos.x += (_float)iX * 0.1;
		Info2.vPos.y += (_float)iY * 0.1;
		Info2.vPos.z += (_float)iZ * 0.1;

		CGameInstance*	pInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pInstance);

		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_Dust"), m_tInfo.iLevelIndex, TEXT("Layer_Dust"), &Info2)))
			return;

		if (m_iDustCount == 0)
			CSoundMgr::Get_Instance()->PlayEffect(L"Door_Effect.wav", fSOUND + 0.1f);
		
		++m_iDustCount;

		Safe_Release(pInstance);
		
	}
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	CGameObject* pTarget = nullptr;

	if (m_iCount < 1 && m_IndexPos.iIndex == 0 && !m_bFirst)
	{
		if (pInstance->Collision(this, TEXT("Com_QuestCollider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
		{
			g_iTalk = 50;
			m_bTalk = true;
		}
		else
		{
			if (50 == g_iTalk)
				g_iTalk = 0;
			m_bTalk = false;
			m_bFirst = false;
		}
	}
	else if (m_bFirst)
	{
		g_iTalk = 0;
		m_bTalk = false;
	}
	else if (m_IndexPos.iIndex == 1 && !m_bTalk)
	{
		if (pInstance->Collision(this, TEXT("Com_QuestCollider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
		{
			m_bTalk = true;
			g_iCut = 50;
		}
	}
	

	Safe_Release(pInstance);
}

HRESULT CDoor::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix, PlayerWorldMatrix;
	_float4x4		ViewMatrixInv;
	_float4			vCamPosition;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	CLayer* pLayer = pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Player"));

	list<class CGameObject*> GameObject = pLayer->Get_Objects();

	PlayerWorldMatrix = GameObject.front()->Get_World();

	memcpy(&vCamPosition, &PlayerWorldMatrix.m[3][0], sizeof(_float4));

	Safe_Release(pInstance);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	_float fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0))))
		return E_FAIL;

	_float	fMin = 3.f;
	_float	fMax = 6.f;


	if (FAILED(m_pShaderCom->Set_RawValue("g_fMinRange", &fMin, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxRange", &fMax, sizeof(_float))))
		return E_FAIL;
	
	m_pShaderCom->Begin(3);

	m_pVIBuffer->Render();

	m_pShaderCom->End();

	if (g_bCollider)
	{
		m_pQuestColliderCom->Render();
		m_pColliderCom->Render();
	}

	return S_OK;
}

HRESULT CDoor::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect3"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Door"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;
	

	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_QuestCollider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pQuestColliderCom)))
		return E_FAIL;

	return S_OK;
}

void CDoor::Ready_Script(void)
{
	m_vNormalScript.push_back(TEXT("열쇠가 없습니다."));

	m_vFindScript.push_back(TEXT("문을 여시겠습니까?"));
}

CDoor * CDoor::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CDoor* pInstance = new CDoor(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoor::Clone(void * pArg)
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDoor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CDoor::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CDoor::Free(void)
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pQuestColliderCom);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureCom);
}
