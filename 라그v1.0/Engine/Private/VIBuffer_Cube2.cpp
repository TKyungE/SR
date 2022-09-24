#include "..\Public\VIBuffer_Cube2.h"
#include "Picking.h"

CVIBuffer_Cube2::CVIBuffer_Cube2(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_Cube2::CVIBuffer_Cube2(const CVIBuffer_Cube2 & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube2::Initialize_Prototype()
{
	m_tVIBInfo.m_iNumVertices = 8;
	m_tVIBInfo.m_iStride = sizeof(VTXCUBETEX);
	m_tVIBInfo.m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_tVIBInfo.m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_tVIBInfo.m_iNumPrimitive = 12;

	/* 정점들을 할당했다. */
	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	VTXCUBETEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(0.f, 1.0f, -0.5f);
	pVertices[0].vTexture = pVertices[0].vPosition;
	m_pVerticesPos.push_back(_float3(0.f, 1.0f, -0.5f));

	pVertices[1].vPosition = _float3(1.f, 1.f, -0.5f);
	pVertices[1].vTexture = pVertices[1].vPosition;
	m_pVerticesPos.push_back(_float3(1.f, 1.f, -0.5f));

	pVertices[2].vPosition = _float3(1.f, 0.f, -0.5f);
	pVertices[2].vTexture = pVertices[2].vPosition;
	m_pVerticesPos.push_back(_float3(1.f, 0.f, -0.5f));

	pVertices[3].vPosition = _float3(0.f, 0.f, -0.5f);
	pVertices[3].vTexture = pVertices[3].vPosition;
	m_pVerticesPos.push_back(_float3(0.5f, 0.f, -0.5f));

	pVertices[4].vPosition = _float3(0.f, 1.f, 0.5f);
	pVertices[4].vTexture = pVertices[4].vPosition;
	m_pVerticesPos.push_back(_float3(0.f, 1.f, 0.5f));

	pVertices[5].vPosition = _float3(1.0f, 1.f, 0.5f);
	pVertices[5].vTexture = pVertices[5].vPosition;
	m_pVerticesPos.push_back(_float3(1.f, 1.f, 0.5f));

	pVertices[6].vPosition = _float3(1.0f, 0.f, 0.5f);
	pVertices[6].vTexture = pVertices[6].vPosition;
	m_pVerticesPos.push_back(_float3(1.f, 0.f, 0.5f));

	pVertices[7].vPosition = _float3(0.f, 0.f, 0.5f);
	pVertices[7].vTexture = pVertices[7].vPosition;
	m_pVerticesPos.push_back(_float3(0.f, 0.f, 0.5f));

	m_pVB->Unlock();

	m_tVIBInfo.m_iIndicesByte = sizeof(FACEINDICES16);
	m_tVIBInfo.m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES16*			pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/* +X */
	pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
	pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;
	m_pIndices16.push_back(pIndices[0]);
	m_pIndices16.push_back(pIndices[1]);

	/* -X */
	pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
	pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;
	m_pIndices16.push_back(pIndices[2]);
	m_pIndices16.push_back(pIndices[3]);

	/* +Y */
	pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
	pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;
	m_pIndices16.push_back(pIndices[4]);
	m_pIndices16.push_back(pIndices[5]);

	/* -Y */																											
	pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
	pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;
	m_pIndices16.push_back(pIndices[6]);
	m_pIndices16.push_back(pIndices[7]);

	/* +Z */																											
	pIndices[8]._0 = 5; pIndices[8]._1 = 4; pIndices[8]._2 =7;
	pIndices[9]._0 = 5; pIndices[9]._1 = 7; pIndices[9]._2 =6;
	m_pIndices16.push_back(pIndices[8]);
	m_pIndices16.push_back(pIndices[9]);

	/* -Z */
	pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
	pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;
	m_pIndices16.push_back(pIndices[10]);
	m_pIndices16.push_back(pIndices[11]);

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Cube2::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Cube2::Picking(_float4x4 WorldMatrix, _float3 * pPickPoint)
{
//	CPicking* pPicking = CPicking::Get_Instance();
//	Safe_AddRef(pPicking);
//
//	_float4x4 WorldMatrixInv;
//	D3DXMatrixInverse(&WorldMatrixInv, nullptr, &WorldMatrix);
//
//	pPicking->Transform_ToLocalSpace(WorldMatrixInv);
//
//	/*for (_uint i = 0; i < m_tVIBInfo_Derived.m_iNumVerticesZ - 1; ++i)
//	{
//		for (_uint j = 0; j < m_tVIBInfo_Derived.m_iNumVerticesX - 1; ++j)
//		{
//			_uint iIndex = i * m_tVIBInfo_Derived.m_iNumVerticesX + j;
//
//			_uint iIndices[4] = {
//				iIndex + m_tVIBInfo_Derived.m_iNumVerticesX,
//				iIndex + m_tVIBInfo_Derived.m_iNumVerticesX + 1,
//				iIndex + 1,
//				iIndex
//			};
//
//			if (true == pPicking->Intersect_InLocalSpace(m_pVerticesPos[iIndices[0]], m_pVerticesPos[iIndices[1]], m_pVerticesPos[iIndices[2]], pPickPoint))
//				goto Coll;
//
//			else if (true == pPicking->Intersect_InLocalSpace(m_pVerticesPos[iIndices[0]], m_pVerticesPos[iIndices[2]], m_pVerticesPos[iIndices[3]], pPickPoint))
//				goto Coll;
//		}
//	}*/
//
//	Safe_Release(pPicking);
//
//	return false;
//
//Coll:
//	D3DXVec3TransformCoord(pPickPoint, pPickPoint, &WorldMatrix);
//
//	Safe_Release(pPicking);
//
	return true;
}

CVIBuffer_Cube2 * CVIBuffer_Cube2::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Cube2*	pInstance = new CVIBuffer_Cube2(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Cube2::Clone(void* pArg)
{
	CVIBuffer_Cube2*	pInstance = new CVIBuffer_Cube2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_Cube"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube2::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		m_pVerticesPos.clear();
		m_pIndices16.clear();
	}
}
