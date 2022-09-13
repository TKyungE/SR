#include "..\Public\VIBuffer_RectLeft.h"

CVIBuffer_RectLeft::CVIBuffer_RectLeft(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CVIBuffer_RectLeft::CVIBuffer_RectLeft(const CVIBuffer_RectLeft & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_RectLeft::Initialize_Prototype()
{
	m_tVIBInfo.m_iNumVertices = 4;
	m_tVIBInfo.m_iStride = sizeof(VTXTEX);
	m_tVIBInfo.m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_tVIBInfo.m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_tVIBInfo.m_iNumPrimitive = 2;


	/* �������� �Ҵ��ߴ�. */
	if (FAILED(__super::Ready_Vertex_Buffer()))
		return E_FAIL;

	VTXTEX*			pVertices = nullptr;

	m_pVB->Lock(0, /*m_iNumVertices * m_iStride*/0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(-1.f, 0.5f, 0.f);
	pVertices[0].vTexture = _float2(0.0f, 0.f);
	m_pVerticesPos.push_back(_float3(-1.f, 0.5f, 0.f));

	pVertices[1].vPosition = _float3(0.f, 0.5f, 0.f);
	pVertices[1].vTexture = _float2(1.f, 0.f);
	m_pVerticesPos.push_back(_float3(0.f, 0.5f, 0.f));

	pVertices[2].vPosition = _float3(0.f, -0.5f, 0.f);
	pVertices[2].vTexture = _float2(1.f, 1.f);
	m_pVerticesPos.push_back(_float3(0.f, -0.5f, 0.f));

	pVertices[3].vPosition = _float3(-1.f, -0.5f, 0.f);
	pVertices[3].vTexture = _float2(0.f, 1.f);
	m_pVerticesPos.push_back(_float3(-1.f, -0.5f, 0.f));

	m_pVB->Unlock();

	m_tVIBInfo.m_iIndicesByte = sizeof(FACEINDICES16);
	m_tVIBInfo.m_eIndexFormat = D3DFMT_INDEX16;

	if (FAILED(__super::Ready_Index_Buffer()))
		return E_FAIL;

	FACEINDICES16*			pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;
	m_pIndices16.push_back(pIndices[0]);

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;
	m_pIndices16.push_back(pIndices[1]);

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_RectLeft::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_RectLeft * CVIBuffer_RectLeft::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_RectLeft*	pInstance = new CVIBuffer_RectLeft(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectLeft::Clone(void* pArg)
{
	CVIBuffer_RectLeft*	pInstance = new CVIBuffer_RectLeft(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectLeft::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		m_pVerticesPos.clear();
		m_pIndices16.clear();
	}
}
