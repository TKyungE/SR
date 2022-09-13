#include "..\Public\Frustum.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;

	Safe_AddRef(m_pGraphic_Device);

	m_vProj[0] = _float3(-1.f, 1.f, 0.f);
	m_vProj[1] = _float3(1.f, 1.f, 0.f);
	m_vProj[2] = _float3(1.f, -1.f, 0.f);
	m_vProj[3] = _float3(-1.f, -1.f, 0.f);
	m_vProj[4] = _float3(-1.f, 1.f, 1.f);
	m_vProj[5] = _float3(1.f, 1.f, 1.f);
	m_vProj[6] = _float3(1.f, -1.f, 1.f);
	m_vProj[7] = _float3(-1.f, -1.f, 1.f);
	
	return S_OK;
}

void CFrustum::Tick()
{
	_float3 vProj[8];

	for (_uint i = 0; i < 8; ++i)
		vProj[i] = m_vProj[i];

	_float4x4 matProj, matView, matProjInv, matViewInv;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProj);

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matViewInv, nullptr, &matView);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&vProj[i], &vProj[i], &matProjInv);
		D3DXVec3TransformCoord(&vProj[i], &vProj[i], &matViewInv);
	}
	/*D3DXVec3TransformCoord(&vProj[0], &vProj[0], &matProjInv);
	D3DXVec3TransformCoord(&vProj[0], &vProj[0], &matViewInv);
	
	D3DXVec3TransformCoord(&vProj[1], &vProj[1], &matProjInv);
	D3DXVec3TransformCoord(&vProj[1], &vProj[1], &matViewInv);

	D3DXVec3TransformCoord(&vProj[2], &vProj[2], &matProjInv);
	D3DXVec3TransformCoord(&vProj[2], &vProj[2], &matViewInv);

	D3DXVec3TransformCoord(&vProj[3], &vProj[3], &matProjInv);
	D3DXVec3TransformCoord(&vProj[3], &vProj[3], &matViewInv);

	D3DXVec3TransformCoord(&vProj[4], &vProj[4], &matProjInv);
	D3DXVec3TransformCoord(&vProj[4], &vProj[4], &matViewInv);

	D3DXVec3TransformCoord(&vProj[5], &vProj[5], &matProjInv);
	D3DXVec3TransformCoord(&vProj[5], &vProj[5], &matViewInv);

	D3DXVec3TransformCoord(&vProj[6], &vProj[6], &matProjInv);
	D3DXVec3TransformCoord(&vProj[6], &vProj[6], &matViewInv);

	D3DXVec3TransformCoord(&vProj[7], &vProj[7], &matProjInv);
	D3DXVec3TransformCoord(&vProj[7], &vProj[7], &matViewInv);*/

	//+z
	m_vProj2[0][0] = vProj[0];
	m_vProj2[0][1] = vProj[1];
	m_vProj2[0][2] = vProj[2];
	// -z
	m_vProj2[1][0] = vProj[5];
	m_vProj2[1][1] = vProj[4];
	m_vProj2[1][2] = vProj[7];

	// -x
	m_vProj2[2][0] = vProj[4];
	m_vProj2[2][1] = vProj[0];
	m_vProj2[2][2] = vProj[3];

	// +x
	m_vProj2[3][0] = vProj[1];
	m_vProj2[3][1] = vProj[5];
	m_vProj2[3][2] = vProj[6];
	
	// +y
	m_vProj2[4][0] = vProj[4];
	m_vProj2[4][1] = vProj[5];
	m_vProj2[4][2] = vProj[1];
	

	//-y
	m_vProj2[5][0] = vProj[3];
	m_vProj2[5][1] = vProj[2];
	m_vProj2[5][2] = vProj[6];
}

_bool CFrustum::IsinFrustum(_float3 vPos, _float3 vScale)
{
	_float fLength = D3DXVec3Length(&vScale);

	for (_uint i = 0; i < 6; ++i)
	{
		D3DXPLANE			Plane;

		D3DXPlaneFromPoints(&Plane, &m_vProj2[i][0], &m_vProj2[i][1], &m_vProj2[i][2]);

		_float		fHeight = Plane.a * vPos.x + Plane.b * vPos.y + Plane.c * vPos.z + Plane.d;

		if (fHeight > 0)
		{
			if (fHeight > fLength)
				return false;
		}
	}

	return true;
}

void CFrustum::Free()
{
	Safe_Release(m_pGraphic_Device);
}
