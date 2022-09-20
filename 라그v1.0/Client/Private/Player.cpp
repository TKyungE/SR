#include "stdafx.h"
#include "..\Public\Player.h"
#include "SoundMgr.h"
#include "GameInstance.h"
#include "Layer.h"
#include "KeyMgr.h"
#include "StatInfo.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_tInfo.pTarget = this;
	memcpy(pArg, &m_tInfo, sizeof(INFO));

	if(FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 0;
	m_tFrame.fFrameSpeed = 0.1f;
	if (m_tInfo.iMaxHp <= 0)
	{
		m_tInfo.iLv = 1;
		m_tInfo.fX = 0.5f;
		m_tInfo.iDmg = 10;
		m_tInfo.iMaxHp = 30000;
		m_tInfo.iHp = m_tInfo.iMaxHp;
		m_tInfo.iMaxMp = 1000;
		m_tInfo.iMp = m_tInfo.iMaxMp;
		m_tInfo.iMaxExp = 100;
		m_tInfo.iExp = 0;
	}
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 0.7f,0.7f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);

	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.vPos = vPos;
	tInfo.vPos.z += 0.35f;
	tInfo.vPos.x += 0.35f;
	//Pet
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Poring"), m_tInfo.iLevelIndex, TEXT("Layer_Pet"), &tInfo);


	Safe_Release(pGameInstance);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (CKeyMgr::Get_Instance()->Key_Down('C'))
	{
		switch (g_bFirst)
		{
		case true:
			g_bFirst = false;
			break;
		case false:
			m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			g_bFirst = true;
			break;
		}
	}
	if (!g_bFirst && !g_bCut)
	{
		if (GetKeyState('W') < 0)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (GetKeyState('S') < 0)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		if (GetKeyState('A') < 0)
		{
			m_pTransformCom->Go_Left(fTimeDelta);
		}

		if (GetKeyState('D') < 0)
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}
	}


	Check_Stat();
	OnTerrain();

	if (!g_bCut)
	{
		if (g_bFirst)
		{
			if (!m_bUI)
			{
				Key_Input(fTimeDelta);
			}

			Player_Move(fTimeDelta);
			Move_Frame(fTimeDelta);
			Get_PickingPoint();
		}
	}
	else
	{
		m_eCurState = IDLE;
		m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

	if (m_tInfo.iHp >= m_tInfo.iMaxHp)
		m_tInfo.iHp = m_tInfo.iMaxHp;
	
	if (GetKeyState('N') & 0x8000)
	{
		if (m_tInfo.iHp > 0)
			m_tInfo.iHp -= 10;
	}
	if (GetKeyState('M') & 0x8000)
	{
		if (m_tInfo.iHp<m_tInfo.iMaxHp)
			m_tInfo.iHp += 10;
		
		if (m_tInfo.iMp<m_tInfo.iMaxMp)
			m_tInfo.iMp += 10;

		m_tInfo.iExp += 10;
	}

	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;
	
	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_PLAYER, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CPlayer"));
		return;
	}

	Safe_Release(pInstance);

	if (m_tInfo.iMp > 0)
	{
		Use_Skill(99);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	/*if (GetKeyState('Q') & 0x8000)
	{
		CGameInstance* pInstance = CGameInstance::Get_Instance();

		Safe_AddRef(pInstance);

		CLayer* pLayer = pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Camera"));

		Safe_AddRef(pLayer);

		list<class CGameObject*> pGameObject = pLayer->Get_Objects();
		pGameObject.front()->Set_bHit(true);
		m_tInfo.bHit = true;

		Safe_Release(pLayer);
		Safe_Release(pInstance);
	}*/

	Motion_Change();
	
	Check_Hit();
	
	LevelUp();

	OnBillboard();

	CheckColl();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if(pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{	
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_NONALPHABLEND, this);
	}

	Safe_Release(pInstance);
}

HRESULT CPlayer::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	Off_SamplerState();
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	if (g_bFirst)
	{
		m_pVIBuffer->Render();
	}
	if (FAILED(Release_RenderState()))
		return E_FAIL;
	
	On_SamplerState();

	if (g_bCollider)
		m_pColliderCom->Render();
	
	return S_OK;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CPlayer::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_OBJECT, TEXT("Com_Collider"), &pTarget))
	{
		_float3 vBackPos;
		if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
		{
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
		}
		else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
		{
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
		}
		vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_TOTEM, TEXT("Com_Collider"),&pTarget))
	{
		_float3 vBackPos;
		if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
		{
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
		}
		else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
		{
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
		}
		vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	}
	//if (pInstance->Collision(this, COLLISION_BOSS, &pTarget))
	//{
	//	_float3 vBackPos;
	//	if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
	//	{
	//		vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
	//		vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
	//	}
	//	else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
	//	{
	//		vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
	//		vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
	//	}
	//	vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	//}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_NPC, TEXT("Com_Collider"), &pTarget))
	{
		_float3 vBackPos;
		if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
		{
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
		}
		else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
		{
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
		}
		vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	}

	Safe_Release(pInstance);
}

void CPlayer::OnTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	if (m_bRide)
	{
		_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vUp.y += 0.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vUp);
	}
	else if (m_bFly)
	{
		_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vUp.y -= 0.1f + m_fFly_fY;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vUp);
	}
	else
	{
		_float3 vUp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vUp.y -= 0.1f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vUp);
	}
	Safe_Release(pGameInstance);
}

HRESULT CPlayer::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IDLE_Front"), (CComponent**)&m_pTextureComIDLE_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_IDLE_Back"), (CComponent**)&m_pTextureComIDLE_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Move_Front"), (CComponent**)&m_pTextureComMove_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Move_Back"), (CComponent**)&m_pTextureComMove_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Left_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Move_Left_Front"), (CComponent**)&m_pTextureComMove_Left_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Left_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Move_Left_Back"), (CComponent**)&m_pTextureComMove_Left_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Skill_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skill_Front"), (CComponent**)&m_pTextureComSkill_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Skill_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Skill_Back"), (CComponent**)&m_pTextureComSkill_Back)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_IDLE_Front"), (CComponent**)&m_pTextureComRide_IDLE_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_IDLE_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_IDLE_Back"), (CComponent**)&m_pTextureComRide_IDLE_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_Move_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_Move_Front"), (CComponent**)&m_pTextureComRide_Move_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_Move_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_Move_Back"), (CComponent**)&m_pTextureComRide_Move_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_Move_Left_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_Move_Left_Front"), (CComponent**)&m_pTextureComRide_Move_Left_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Ride_Move_Left_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ride_Move_Left_Back"), (CComponent**)&m_pTextureComRide_Move_Left_Back)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CPlayer::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}
void CPlayer::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale;
	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	if (m_bRight)
	{
		m_pTransformCom->Set_Scaled(_float3(-1.f, 1.f, 1.f));
		vRight.x = -1;
	}
	else if (m_bLeft)
		m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));


	
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
		m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
	
}

HRESULT CPlayer::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CPlayer::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

void CPlayer::Use_Skill(_int iIndex)
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();

	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	switch (iIndex)
	{
	case 1:
		if (!m_bUseSkill && !m_bThunder)
		{
			CGameObject::INFO tInfo;

			tInfo.vPos = m_fPickPoint;
			tInfo.pTarget = this;
			pInstance->Add_GameObject(TEXT("Prototype_GameObject_UseSkill"), m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"), &tInfo);

			m_bUseSkill = true;
			m_bThunder = true;
		}
		break;
	case 2:
		if (!m_bUseSkill && !m_bTornado)
		{
			CGameObject::INFO tInfo;

			tInfo.vPos = m_fPickPoint;
			tInfo.pTarget = this;
			pInstance->Add_GameObject(TEXT("Prototype_GameObject_UseSkill"), m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"), &tInfo);

			m_bUseSkill = true;
			m_bTornado = true;
		}
		break;
	case 3:
		if (!m_bUseSkill && !m_bFireBall)
		{
			CGameObject::INFO tInfo;

			tInfo.vPos = m_fPickPoint;
			tInfo.pTarget = this;
			pInstance->Add_GameObject(TEXT("Prototype_GameObject_UseSkill"), m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"), &tInfo);

			m_bUseSkill = true;
			m_bFireBall = true;
		}
		break;
	default:
		break;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON) && m_bUseSkill && m_bThunder)
	{
		
		pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"))->Get_Objects().front()->Set_Dead();
		Skill_Thunder(TEXT("Layer_Skill"), m_fPickPoint);
		m_bUseSkill = false;
		m_bThunder = false;
		m_eCurState = SKILL;
		m_tFrame.iFrameStart = 0;
		m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_tInfo.iMp -= 20;
	}
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON) && m_bUseSkill && m_bTornado)
	{
		
		pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"))->Get_Objects().front()->Set_Dead();
		Skill_Tornado(TEXT("Layer_Skill"), m_fPickPoint);
		m_bUseSkill = false;
		m_bTornado = false;
		m_eCurState = SKILL;
		m_tFrame.iFrameStart = 0;
		m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_tInfo.iMp -= 10;
	}
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON) && m_bUseSkill && m_bFireBall)
	{
		
		pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_UseSkill"))->Get_Objects().front()->Set_Dead();
		Skill_FireBall(TEXT("Layer_Skill"), m_fPickPoint);
		m_bUseSkill = false;
		m_bFireBall = false;
		m_eCurState = SKILL;
		m_tFrame.iFrameStart = 0;
		m_vTarget = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_tInfo.iMp -= 5;
	}

	Safe_Release(pInstance);
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();

	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
		
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RBUTTON) && m_eCurState != SKILL)
	{
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vTarget = m_fPickPoint;
		if (!m_bFly && m_vTarget.x > vPos.x + 1.f)
		{
			m_eCurState = MOVE_LEFT;
			m_tFrame.iFrameStart = 0;
			m_bRight = true;
			m_bLeft = false;
		}
		else if (!m_bFly && m_vTarget.x < vPos.x - 1.f)
		{
			m_eCurState = MOVE_LEFT;
			m_tFrame.iFrameStart = 0;
			m_bLeft = true;
			m_bRight = false;
		}
		else if(!m_bFly)
		{
			m_eCurState = MOVE;
			m_tFrame.iFrameStart = 0;
			m_bRight = false;
			m_bLeft = false;
		}
		
		Check_Front();
	}

	if (CKeyMgr::Get_Instance()->Key_Down('F') && !m_bRide)
	{
		switch (m_bFly)
		{
		case true:
			m_bFly = false;
			m_fFly_fY = 0.f;
			break;
		case false:
			m_bFly = true;
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			if (nullptr == pGameInstance)
				return;
			Safe_AddRef(pGameInstance);
			CGameObject::INFO tInfo;
			tInfo.pTarget = this;
			tInfo.vPos = { 0.5f,0.5f,1.f };
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wing"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
			Safe_Release(pGameInstance);
			break;
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Down('R') && !m_bFly && m_eCurState == IDLE)
	{
		switch (m_bRide)
		{
		case true:
			m_bRide = false;
			break;
		case false:
			m_bRide = true;
			if (m_eCurState == MOVE)
			{
				m_tFrame.iFrameStart = 0;
				m_tFrame.iFrameEnd = 4;
				m_tFrame.fFrameSpeed = 0.07f;
			}
			break;
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP) && m_bFly)
	{
		m_fFly_fY -= 0.1f;
		if (m_fFly_fY < -2.f)
			m_fFly_fY = -2.f;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN) && m_bFly)
	{
		m_fFly_fY += 0.1f;
		if (m_fFly_fY > 0.f)
			m_fFly_fY = 0.f;
	}
		
	Safe_Release(pInstance);
}

HRESULT CPlayer::Skill_Thunder(const _tchar * pLayerTag, _float3 _vPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.vPos = _vPos;
	tInfo.pTarget = this;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ThunderCloud"), m_tInfo.iLevelIndex, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Skill_Tornado(const _tchar * pLayerTag, _float3 _vPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.vPos = _vPos;
	tInfo.pTarget = this;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tornado"), m_tInfo.iLevelIndex, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CPlayer::Skill_FireBall(const _tchar * pLayerTag, _float3 _vPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	tInfo.vTargetPos = _vPos;
	tInfo.pTarget = this;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_FireBall"), m_tInfo.iLevelIndex, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CPlayer::Skill_FireSpear(const _tchar * pLayerTag, _float3 _vPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	for (int i = 0; i < 10; ++i)
	{
		_float iSour = rand() % 6000 * 0.001f;
		_float iTemp = rand() % 6000 * 0.001f;
		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y;
		tInfo.vPos.z = vPos.z + iTemp;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_FireSpear"), m_tInfo.iLevelIndex, pLayerTag, &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CPlayer::Skill_Meteor(const _tchar * pLayerTag, _float3 _vPos)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	for (int i = 0; i < 10; ++i)
	{
		_float iSour = rand() % 6000 * 0.001f;
		_float iTemp = rand() % 6000 * 0.001f;

		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y;
		tInfo.vPos.z = vPos.z + iTemp;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Meteor"), m_tInfo.iLevelIndex, pLayerTag, &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}
_float4x4 CPlayer::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CPlayer::Free(void)
{
	__super::Free();

	CKeyMgr::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureComIDLE_Front);
	Safe_Release(m_pTextureComIDLE_Back);
	Safe_Release(m_pTextureComMove_Front);
	Safe_Release(m_pTextureComMove_Back);
	Safe_Release(m_pTextureComMove_Left_Front);
	Safe_Release(m_pTextureComMove_Left_Back);
	Safe_Release(m_pTextureComSkill_Front);
	Safe_Release(m_pTextureComSkill_Back);
	Safe_Release(m_pTextureComRide_IDLE_Front);
	Safe_Release(m_pTextureComRide_IDLE_Back);
	Safe_Release(m_pTextureComRide_Move_Front);
	Safe_Release(m_pTextureComRide_Move_Back);
	Safe_Release(m_pTextureComRide_Move_Left_Front);
	Safe_Release(m_pTextureComRide_Move_Left_Back);
}

_float3 CPlayer::Get_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}


void CPlayer::Player_Move(_float fTimeDelta)
{
	_float3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vLook;
	m_vTargetLook = m_vTarget - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &m_vTargetLook);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	if(m_bRide)
	{ 
		if (D3DXVec3Length(&m_vTargetLook) < 0.75f)
		{
			m_eCurState = IDLE;
			m_tFrame.iFrameStart = 0;
		}
		m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);
	}
	else
	{
		if (D3DXVec3Length(&m_vTargetLook) < 0.55f)
		{
			if (m_eCurState == SKILL)
			{
				if (m_tFrame.iFrameStart == 4)
				{
					m_eCurState = IDLE;
					m_tFrame.iFrameStart = 0;
				}
			}
			else
			{
				m_eCurState = IDLE;
				m_tFrame.iFrameStart = 0;
			}

		}
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	
}

void CPlayer::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 0;
			m_tFrame.fFrameSpeed = 0.1f;
			break;
		case MOVE:
			if (m_bRide)
			{
				m_tFrame.iFrameStart = 0;
				m_tFrame.iFrameEnd = 4;
				m_tFrame.fFrameSpeed = 0.1f;
			}
			else
			{
				m_tFrame.iFrameStart = 0;
				m_tFrame.iFrameEnd = 7;
				m_tFrame.fFrameSpeed = 0.1f;
			}
			break;
		case MOVE_LEFT:
			if (m_bRide)
			{
				m_tFrame.iFrameStart = 0;
				m_tFrame.iFrameEnd = 3;
				m_tFrame.fFrameSpeed = 0.1f;
			}
			else
			{
				m_tFrame.iFrameStart = 0;
				m_tFrame.iFrameEnd = 7;
				m_tFrame.fFrameSpeed = 0.1f;
			}
			break;
		case SKILL:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 4;
			m_tFrame.fFrameSpeed = 0.1f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CPlayer::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		if(m_bFront)
			if(m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_IDLE_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComIDLE_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			if (m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_IDLE_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComIDLE_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case MOVE:
		if (m_bFront)
			if(m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_Move_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComMove_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			if (m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_Move_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComMove_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case MOVE_LEFT:
		if (m_bFront)
			if (m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_Move_Left_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComMove_Left_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			if (m_bRide)
				m_tFrame.iFrameStart = m_pTextureComRide_Move_Left_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
			else
				m_tFrame.iFrameStart = m_pTextureComMove_Left_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case SKILL:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComSkill_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComSkill_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
}

void CPlayer::Check_Front()
{
	
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		
	if (m_vTarget.z > vPos.z && !m_bCamera)
		m_bFront = false;
	else if (m_vTarget.z <= vPos.z && !m_bCamera)
		m_bFront = true;
	else if (m_vTarget.z > vPos.z && m_bCamera)
		m_bFront = true;
	else if (m_vTarget.z <= vPos.z && m_bCamera)
		m_bFront = false;



}

HRESULT CPlayer::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (m_bFront)
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_IDLE_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComIDLE_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		else
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_IDLE_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComIDLE_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		break;
	case MOVE:
		if (m_bFront)
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_Move_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComMove_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		else
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_Move_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComMove_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		break;
	case MOVE_LEFT:
		if (m_bFront)
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_Move_Left_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComMove_Left_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		else
		{
			if (m_bRide)
			{
				if (FAILED(m_pTextureComRide_Move_Left_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pTextureComMove_Left_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
					return E_FAIL;
			}
		}
		break;
	case SKILL:
		if (m_bFront)
		{
			if (FAILED(m_pTextureComSkill_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureComSkill_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		break;
	default:
		break;
	}
	return S_OK;
}

void CPlayer::Get_PickingPoint(void)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);

	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	pVIBuffer_Terrain->Picking(pTransform_Terrain->Get_WorldMatrix(), &m_fPickPoint);

	Safe_Release(pGameInstance);
	return;
}

void CPlayer::LevelUp()
{
	if (m_tInfo.iExp > m_tInfo.iMaxExp)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LevelUp"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
		Safe_Release(pGameInstance);

		dynamic_cast<CStatInfo*>(m_StatInfo)->Set_StatsPoint(5);
		m_tInfo.iExp = 0;
		m_tInfo.iHp = m_tInfo.iMaxHp;
		m_tInfo.iMp = m_tInfo.iMaxMp;
		m_tInfo.iMaxExp += 10;
		++m_tInfo.iLv;
	}
}

void CPlayer::Check_Stat()
{
	if (!m_bCheckStat)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		if (nullptr == pGameInstance)
			return;
		Safe_AddRef(pGameInstance);
		m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
		Safe_Release(pGameInstance);
		m_bCheckStat = true;
	}

	if (m_StatInfo != nullptr)
	{
		m_StatDmg = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iSTR * 10;
		m_StatHp = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iDEX * 100;
		m_StatMp = dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iINT * 20;
	}
	m_tInfo.iDmg = 10 + m_StatDmg;
	m_tInfo.iMaxHp = 30000 + m_StatHp;
	m_tInfo.iMaxMp = 1000 + m_StatMp;
	if (m_tInfo.iMp < 0)
		m_tInfo.iMp = 0;
}


void CPlayer::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), m_tInfo.iLevelIndex, TEXT("Layer_DmgFont"), &tInfo);
		tInfo.vPos = m_tInfo.vTargetPos;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);

		CLayer* pLayer = pGameInstance->Find_Layer(m_tInfo.iLevelIndex,TEXT("Layer_Camera"));

		Safe_AddRef(pLayer);

		list<class CGameObject*> pGameObject = pLayer->Get_Objects();
		pGameObject.front()->Set_bHit(true);

		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;

		Safe_Release(pLayer);

		Safe_Release(pGameInstance);
	}
}
