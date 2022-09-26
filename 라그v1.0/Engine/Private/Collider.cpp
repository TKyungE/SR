#include "..\Public\Collider.h"

#include "GameInstance.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
{
}

HRESULT CCollider::Initialize_Prototype(void)
{
	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	if (FAILED(SetUp_Components()))
	{
		ERR_MSG(TEXT("Failed to Set Components : CCollider"));
		return E_FAIL;
	}

	m_iLevelIndex = *(_uint*)&pArg;

	return S_OK;
}

void CCollider::Set_Transform(_float4x4 matTargetWorld, _float fScale)
{
	_float3 vTargetRight = *(_float3*)&matTargetWorld.m[0][0];
	_float3 vTargetUp = *(_float3*)&matTargetWorld.m[1][0];
	_float3 vTargetLook = *(_float3*)&matTargetWorld.m[2][0];

	m_pTransformCom->Set_Scaled(_float3(D3DXVec3Length(&vTargetRight) * fScale, D3DXVec3Length(&vTargetUp) * fScale, D3DXVec3Length(&vTargetLook) * fScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_float3*)&matTargetWorld.m[3][0]);
}

HRESULT CCollider::Render(void)
{
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	
	if (FAILED(SetUp_RenderState()))
		return E_FAIL;
	
	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

_float3 CCollider::Find_MinPoint(void)
{
	_float3 vMin = m_pVIBufferCom->m_pVerticesPos[0];

	for (_uint i = 0; i < m_pVIBufferCom->Get_VIBInfo().m_iNumVertices; ++i)
	{
		if (vMin.x >= m_pVIBufferCom->m_pVerticesPos[i].x && vMin.y >= m_pVIBufferCom->m_pVerticesPos[i].y && vMin.z >= m_pVIBufferCom->m_pVerticesPos[i].z)
			vMin = m_pVIBufferCom->m_pVerticesPos[i];
	}

	D3DXVec3TransformCoord(&vMin, &vMin, &m_pTransformCom->Get_WorldMatrix());

	return vMin;
}

_float3 CCollider::Find_MaxPoint(void)
{
	_float3 vMax;

	for (_uint i = 0; i < m_pVIBufferCom->Get_VIBInfo().m_iNumVertices; ++i)
	{
		if (vMax.x <= m_pVIBufferCom->m_pVerticesPos[i].x && vMax.y <= m_pVIBufferCom->m_pVerticesPos[i].y && vMax.z <= m_pVIBufferCom->m_pVerticesPos[i].z)
			vMax = m_pVIBufferCom->m_pVerticesPos[i];
	}

	D3DXVec3TransformCoord(&vMax, &vMax, &m_pTransformCom->Get_WorldMatrix());

	return vMax;
}

HRESULT CCollider::SetUp_Components(void)
{
	/* For.Com_VIBuffer */
	if (FAILED(Add_Components(TEXT("Com_VIBuffer"), m_iLevelIndex, TEXT("Prototype_Component_VIBuffer_Cube"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	
	if (FAILED(Add_Components(TEXT("Com_Transform"), m_iLevelIndex, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider::SetUp_RenderState(void)
{
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	return S_OK;
}

HRESULT CCollider::Release_RenderState(void)
{
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return S_OK;
}

HRESULT CCollider::Add_Components(const _tchar * pComponentTag, _uint iLevelIndex, const _tchar * pPrototypeTag, CComponent ** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;
	
	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CCollider::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;
	
	return iter->second;
}

void CCollider::Turn(_float3 vAxis, _float fTimeDelta)
{
	_float3	vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_float3	vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_float3	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float4x4	RotationMatrix;
	D3DXMatrixRotationAxis(&RotationMatrix, &vAxis, m_pTransformCom->Get_TransformDesc().fRotationPerSec * fTimeDelta);

	D3DXVec3TransformNormal(&vRight, &vRight, &RotationMatrix);
	D3DXVec3TransformNormal(&vUp, &vUp, &RotationMatrix);
	D3DXVec3TransformNormal(&vLook, &vLook, &RotationMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider* pInstance = new CCollider(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free(void)
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
