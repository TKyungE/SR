#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
	ZeroMemory(&m_tVIBInfo, sizeof(VIBINFO));
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_pVerticesPos(rhs.m_pVerticesPos)
	, m_pIndices16(rhs.m_pIndices16)
	, m_pIndices32(rhs.m_pIndices32)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	memcpy(&m_tVIBInfo, &rhs.m_tVIBInfo, sizeof(VIBINFO));
}

void CVIBuffer::Set_VIBInfo(VIBINFO & tVIBInfo)
{
	m_tVIBInfo.m_iNumVertices = tVIBInfo.m_iNumVertices;
	m_tVIBInfo.m_iStride = tVIBInfo.m_iStride;
	m_tVIBInfo.m_dwFVF = tVIBInfo.m_dwFVF;
	m_tVIBInfo.m_ePrimitiveType = tVIBInfo.m_ePrimitiveType;
	m_tVIBInfo.m_iNumPrimitive = tVIBInfo.m_iNumPrimitive;
	m_tVIBInfo.m_iIndicesByte = tVIBInfo.m_iIndicesByte;
	m_tVIBInfo.m_eIndexFormat = tVIBInfo.m_eIndexFormat;
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;	

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_tVIBInfo.m_iStride);
	m_pGraphic_Device->SetFVF(m_tVIBInfo.m_dwFVF);
	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(m_tVIBInfo.m_ePrimitiveType, 0, 0, m_tVIBInfo.m_iNumVertices, 0, m_tVIBInfo.m_iNumPrimitive);
	return S_OK;
}

HRESULT CVIBuffer::Ready_Vertex_Buffer()
{
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_tVIBInfo.m_iNumVertices * m_tVIBInfo.m_iStride, 0, m_tVIBInfo.m_dwFVF, D3DPOOL_MANAGED, &m_pVB, 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Ready_Index_Buffer()
{
	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_tVIBInfo.m_iNumPrimitive * m_tVIBInfo.m_iIndicesByte, 0, m_tVIBInfo.m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

}
