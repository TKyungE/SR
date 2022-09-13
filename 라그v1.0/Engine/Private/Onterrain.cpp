#include "..\Public\Onterrain.h"
#include "..\Public\VIBuffer_Terrain.h"

_tchar* COnterrain::m_pVIBufferTag = TEXT("Com_VIBuffer");

COnterrain::COnterrain(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CComponent(_pGraphic_Device)
{
}

HRESULT COnterrain::Initialize_Prototype(void)
{
	return S_OK;
}

HRESULT COnterrain::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT COnterrain::Set_TerrainVIBuffer(CVIBuffer_Terrain * _pVIBuffer)
{
	m_pVIBuffer = _pVIBuffer;

	if (nullptr == m_pVIBuffer)
	{
		ERR_MSG(TEXT("Failed to Set Terrain : COnterrain"));
		return E_FAIL;
	}

	Safe_AddRef(m_pVIBuffer);

	return S_OK;
}

HRESULT COnterrain::Get_OnTerrainY(_float3 vecTarget, _float* pOut)
{
	_int iLDIndex = _int(vecTarget.z) * m_pVIBuffer->Get_VIBInfoDerived().m_iNumVerticesX + vecTarget.x;

	_uint iTargetIndices[4] = {
		iLDIndex + m_pVIBuffer->Get_VIBInfoDerived().m_iNumVerticesX,
		iLDIndex + m_pVIBuffer->Get_VIBInfoDerived().m_iNumVerticesX + 1,
		iLDIndex + 1,
		iLDIndex
	};

	LPDIRECT3DVERTEXBUFFER9 VB = m_pVIBuffer->Get_VB();

	VTXTEX* pVertices = nullptr;

	VB->Lock(0, 0, (void**)&pVertices, 0);

	_float3 IndicesVertex[4];

	for (_uint i = 0; i < (sizeof(iTargetIndices) / sizeof(_uint)); ++i)
		IndicesVertex[i] = pVertices[iTargetIndices[i]].vPosition;

	VB->Unlock();

	_float fCompareX = sqrtf((IndicesVertex[0].y - vecTarget.y) * (IndicesVertex[0].y - vecTarget.y) + (IndicesVertex[0].x - vecTarget.x) * (IndicesVertex[0].x - vecTarget.x));
	_float fCompareZ = sqrtf((IndicesVertex[0].y - vecTarget.y) * (IndicesVertex[0].y - vecTarget.y) + (IndicesVertex[0].z - vecTarget.z) * (IndicesVertex[0].z - vecTarget.z));

	D3DXPLANE Plane;
	if (fCompareX > fCompareZ)
		D3DXPlaneFromPoints(&Plane, &IndicesVertex[0], &IndicesVertex[1], &IndicesVertex[2]);
	else
		D3DXPlaneFromPoints(&Plane, &IndicesVertex[0], &IndicesVertex[2], &IndicesVertex[3]);
	
	*pOut = ((vecTarget.x * Plane.a * -1.f) + (vecTarget.z * Plane.c * -1.f) - Plane.d) / Plane.b;
	
	return S_OK;
}

COnterrain * COnterrain::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	COnterrain* pInstance = new COnterrain(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : COnterrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * COnterrain::Clone(void * pArg)
{
	AddRef();

	return this;
}

void COnterrain::Free(void)
{
	__super::Free();

	Safe_Release(m_pVIBuffer);
}