#include "..\Public\Camera.h"

_tchar*	CCamera::m_pTransformTag = TEXT("Com_Transform");

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pGraphic_Device);
	if (nullptr == m_pTransform)
		return E_FAIL;	

	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_State(CTransform::STATE_POSITION, m_CameraDesc.vEye);
	m_pTransform->LookAt(m_CameraDesc.vAt);

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	m_Components.emplace(m_pTransformTag, m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_OnGraphicDev()
{
	/* 카메라의 월드행렬을 얻어온다. */
	_float4x4		WorldMatrix = m_pTransform->Get_WorldMatrix();

	/* 월드 행렬의 역행렬(뷰스페이스 변환행렬)을 구하자. */
	_float4x4		ViewMatrix = *D3DXMatrixInverse(&ViewMatrix, nullptr, &WorldMatrix);
	_float4x4		ProjMatrix = *D3DXMatrixPerspectiveFovLH(&ProjMatrix, m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &ProjMatrix);

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);

}
