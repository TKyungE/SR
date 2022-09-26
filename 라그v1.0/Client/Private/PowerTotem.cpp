#include "stdafx.h"
#include "..\Public\PowerTotem.h"
#include "GameInstance.h"
#include "SoundMgr.h"

CPowerTotem::CPowerTotem(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CPowerTotem::CPowerTotem(const CPowerTotem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPowerTotem::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPowerTotem::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Set_vPos();
	m_tInfo.iMaxHp = 20000;
	m_tInfo.iHp = m_tInfo.iMaxHp;
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 1.f,2.6f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	tInfo.iMonsterType = MON_TOTEM;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), LEVEL_MIDBOSS, TEXT("Layer_Status"), &tInfo);
	tInfo.vPos = { 1.8f,1.8f,1.8f };

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CPowerTotem::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_fHitTime += fTimeDelta;
	OnTerrain();
	CreateDrain(fTimeDelta);

	_float3 vRatZ = { 0.f,0.f,1.f };

	m_pTransformCom2->Turn(vRatZ, fTimeDelta);



	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 1.f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_TOTEM, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CPowerTotem"));
		return;
	}

	Safe_Release(pInstance);
	if (m_tInfo.bDead)
	{
		m_tInfo.pTerrain->Set_Mp(-1);
		CSoundMgr::Get_Instance()->PlayEffect(L"Totem_Die.wav", fSOUND);
	}
}

void CPowerTotem::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CheckColl();
	Check_Hit();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);
	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_ALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CPowerTotem::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	Off_SamplerState();


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	House_Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;
	On_SamplerState();
	if (g_bCollider)
		m_pColliderCom->Render();
	return S_OK;
}

HRESULT CPowerTotem::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferCube"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), (CComponent**)&m_pVIBufferCube)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferRect"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferRect)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stone"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PowerTotem"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPowerTotem::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	
	return S_OK;
}

HRESULT CPowerTotem::Release_RenderState(void)
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}
HRESULT CPowerTotem::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CPowerTotem::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
void CPowerTotem::OnTerrain(void)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;

	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain* pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_MIDBOSS, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform* pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_MIDBOSS, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.2f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}
void CPowerTotem::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);;
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), LEVEL_MIDBOSS, TEXT("Layer_DmgFont"), &tInfo);
		tInfo.vPos = m_tInfo.vTargetPos;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo);
		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;
		Safe_Release(pGameInstance);
	}
}

HRESULT CPowerTotem::CreateDrain(_float fTimeDelta)
{
	m_fDrainTime += fTimeDelta;
	if (m_fDrainTime > 3.f)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - vPos;
		_float vDist = 0.5f;
		_float vSpeed = 0.f;
		tInfo.fX = 0.f;
		for (_int i = 0; i < 20; ++i)
		{
			tInfo.pTarget = m_tInfo.pTarget;
			tInfo.iLevelIndex = m_tInfo.iLevelIndex;
			vPos += *D3DXVec3Normalize(&vLook, &vLook) * vDist;
			tInfo.vPos = vPos;
			tInfo.fX += vSpeed;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BossSkillTarget"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo)))
				return E_FAIL;
			vDist += 0.1f;
			vSpeed += 0.01f;
		}
		CSoundMgr::Get_Instance()->PlayEffect(L"SkyDragon_Skill.wav", fSOUND);
		Safe_Release(pGameInstance);
		m_fDrainTime = 0.f;
	}
	return S_OK;
}


void CPowerTotem::Set_vPos()
{
	m_tInfo.bDead = false;
	m_pTransformCom->Set_Scaled({ 0.8f,3.2f,0.8f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 2.3f;
	m_pTransformCom2->Set_Scaled({ 1.f,1.f,1.f });
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);

}

HRESULT CPowerTotem::House_Render()
{
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferCube->Render();
	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferRect->Render();
	return S_OK;
}

CPowerTotem * CPowerTotem::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CPowerTotem* pInstance = new CPowerTotem(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPowerTotem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPowerTotem::Clone(void * pArg)
{
	CPowerTotem* pInstance = new CPowerTotem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPowerTotem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CPowerTotem::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();;
}

void CPowerTotem::Free(void)
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCube);
	Safe_Release(m_pVIBufferRect);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom2);
}
void CPowerTotem::CheckColl()
{
}