#include "..\Public\Picking.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
	ZeroMemory(&m_MouseRayPos, sizeof(_float3));
	ZeroMemory(&m_MouseRayDir, sizeof(_float3));
	ZeroMemory(&m_MouseRayPos_Local, sizeof(_float3));
	ZeroMemory(&m_MouseRayDir_Local, sizeof(_float3));
}

HRESULT CPicking::Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device)
{
	m_hWnd = hWnd;
	m_pGraphic_Device = pGraphic_Device;
	Safe_AddRef(m_pGraphic_Device);

	return S_OK;
}

void CPicking::Tick(void)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	D3DVIEWPORT9 ViewPort;

	m_pGraphic_Device->GetViewport(&ViewPort);

	m_MouseRayDir.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	m_MouseRayDir.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	m_MouseRayDir.z = 0.f;

	_float4x4 View, Proj, InvView, InvProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &View);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &Proj);

	D3DXMatrixInverse(&InvProj, nullptr, &Proj);
	D3DXVec3TransformCoord(&m_MouseRayDir, &m_MouseRayDir, &InvProj);

	m_MouseRayPos = { 0.f, 0.f, 0.f };

	D3DXMatrixInverse(&InvView, nullptr, &View);

	D3DXVec3TransformCoord(&m_MouseRayPos, &m_MouseRayPos, &InvView);
	D3DXVec3TransformNormal(&m_MouseRayDir, &m_MouseRayDir, &InvView);

	D3DXVec3Normalize(&m_MouseRayDir, &m_MouseRayDir);
}

void CPicking::Transform_ToLocalSpace(_float4x4 WorldMatrixInverse)
{
	D3DXVec3TransformCoord(&m_MouseRayPos_Local, &m_MouseRayPos, &WorldMatrixInverse);
	D3DXVec3TransformNormal(&m_MouseRayDir_Local, &m_MouseRayDir, &WorldMatrixInverse);

	D3DXVec3Normalize(&m_MouseRayDir_Local, &m_MouseRayDir_Local);
}

_bool CPicking::Intersect_InWorldSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3 * pOut)
{
	_float fDist;

	if (D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_MouseRayPos, &m_MouseRayDir, nullptr, nullptr, &fDist))
	{
		*pOut = m_MouseRayPos + m_MouseRayDir * fDist;

		return true;
	}

	return false;
}

_bool CPicking::Intersect_InLocalSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3 * pOut)
{
	_float fDist;

	if (D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_MouseRayPos_Local, &m_MouseRayDir_Local, nullptr, nullptr, &fDist))
	{
		*pOut = m_MouseRayPos_Local + m_MouseRayDir_Local * fDist;

		return true;
	}

	return false;
}

void CPicking::Free(void)
{
	Safe_Release(m_pGraphic_Device);
}
