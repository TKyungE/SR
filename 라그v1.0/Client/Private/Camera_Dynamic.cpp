#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "Player.h"
#include "KeyMgr.h"

CCamera_Dynamic::CCamera_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
	ZeroMemory(&m_vecCameraNormal, sizeof(_float3));

	D3DXMatrixIdentity(&m_matRotY);
	D3DXMatrixIdentity(&m_matRotX);
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
	, m_vecCameraNormal(rhs.m_vecCameraNormal)
	, m_YfAngle(rhs.m_YfAngle)
	, m_matRotY(rhs.m_matRotY)
	, m_matRotX(rhs.m_matRotX)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	
	m_vecCameraNormal = *(_float3*)&m_CameraDesc.Info.pTarget->Get_World().m[2][0] * -1.f;
	D3DXVec3Normalize(&m_vecCameraNormal, &m_vecCameraNormal);

	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long MouseMove = 0;

	if (GetKeyState(VK_LEFT) < 0)
		CameraRotationX(fTimeDelta, -10.f);
	if (GetKeyState(VK_RIGHT) < 0)
		CameraRotationX(fTimeDelta, 10.f);

//	if ((GetKeyState(VK_LSHIFT)& 8000) &&  (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y)))
//		CameraRotationY(fTimeDelta, MouseMove);

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_WHEEL))
	{
		if ((1.7f <= m_CameraDesc.fFovy) && (0 < D3DXToRadian(fTimeDelta * MouseMove * -1.f)))
			m_CameraDesc.fFovy = 1.7f;

		else if ((0.4f >= m_CameraDesc.fFovy) && (0 >= D3DXToRadian(fTimeDelta * MouseMove * -1.f)))
			m_CameraDesc.fFovy = 0.4f;
		else
		{
			m_CameraDesc.fFovy += D3DXToRadian(fTimeDelta * MouseMove * -1.f);
			if (0 >= D3DXToRadian(fTimeDelta * MouseMove * -1.f))
			{
				m_YfAngle -= 1.f;
				if (m_YfAngle < 30.f)
					m_YfAngle = 30.f;
			}
			else if (0 < D3DXToRadian(fTimeDelta * MouseMove * -1.f))
			{
				m_YfAngle += 1.f;
				if (m_YfAngle > 50.f)
					m_YfAngle = 50.f;
			}
		}

		m_bWheelMove = true;
	}


	_float4x4 CameraRotationMatrix, CameraMatrix;
	D3DXMatrixRotationAxis(&CameraRotationMatrix, &m_pTransform->Get_State(CTransform::STATE_RIGHT), D3DXToRadian(45.f));

	_float3 Camera;
	CameraMatrix = m_matRotX * CameraRotationMatrix;
	D3DXVec3TransformNormal(&Camera, &m_vecCameraNormal, &CameraMatrix);

	if (CKeyMgr::Get_Instance()->Key_Pressing('B'))
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, (Camera * -5.f) + *(_float3*)&m_CameraDesc.Info.pTarget->Get_World().m[3][0]);
		if (!m_bTrue)
		{
			dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Camera(true);
			if (dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Get_Front())
				dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Front(false);
			else
				dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Front(true);
			m_bTrue = true;
		}
	}
	else
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, (Camera * 5.f) + *(_float3*)&m_CameraDesc.Info.pTarget->Get_World().m[3][0]);
		if (m_bTrue)
		{
			dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Camera(false);
			if (dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Get_Front())
				dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Front(false);
			else
				dynamic_cast<CPlayer*>(m_CameraDesc.Info.pTarget)->Set_Front(true);
			m_bTrue = false;
		}
	}
	Safe_Release(pGameInstance);

	m_pTransform->LookAt(*(_float3*)&m_CameraDesc.Info.pTarget->Get_World().m[3][0]);

	if (g_bCut && m_CameraDesc.fFovy > D3DXToRadian(40.f))
		m_CameraDesc.fFovy -= D3DXToRadian(0.25f);

	else if (!g_bCut && !m_bWheelMove && m_CameraDesc.fFovy < D3DXToRadian(60.f))
		m_CameraDesc.fFovy += D3DXToRadian(0.25f);

	if (FAILED(Bind_OnGraphicDev()))
		return;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dynamic::CameraRotationX(_float fTimeDelta, _float fIncrease)
{
	m_XfAngle += fTimeDelta * fIncrease * m_CameraDesc.TransformDesc.fRotationPerSec * 0.02f;

	//if (m_XfAngle > D3DXToRadian(-30.f) && m_XfAngle < D3DXToRadian(30.f))
		//D3DXMatrixRotationAxis(&m_matRotX, &m_pTransform->Get_State(CTransform::STATE_UP), m_XfAngle);
	/*_float4x4 matXRot;
	D3DXMatrixRotationAxis(&matXRot, &_float3(0.f, 1.f, 0.f), fTimeDelta * MouseMove * m_CameraDesc.TransformDesc.fRotationPerSec * 0.05f);
	D3DXVec3TransformNormal(&m_vecCameraNormal, &m_vecCameraNormal, &matXRot);*/

	D3DXMatrixRotationAxis(&m_matRotX, &_float3(0.f, 1.f, 0.f), m_XfAngle);
}

CCamera_Dynamic * CCamera_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CCamera_Dynamic::Get_World(void)
{
	return _float4x4();
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}
