#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "TerrainRect.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	ZeroMemory(&m_tInfo, sizeof(CGameObject::INFO));
	memcpy(&m_tInfo, pArg, sizeof(CGameObject::INFO));
	
	//여기서 링크를 가져와서 대입하는 방식이 현명해보임,, 밑에는 임시방편

	if(FAILED(OnLoadData(m_tInfo.pstrPath)))
	{ 
		ERR_MSG(TEXT("Failed to OnLoadData"));
		return E_FAIL;
	}
	
	if (m_tInfo.iLevelIndex == LEVEL_CHOBOFIELD)
		m_tInfo.iMp = 91;
	else if (m_tInfo.iLevelIndex == LEVEL_MIDBOSS)
		m_tInfo.iMp = 112;
	else if (m_tInfo.iLevelIndex == LEVEL_TOWN2)
		m_tInfo.iMp = 81;
	else if (m_tInfo.iLevelIndex == LEVEL_DESERT1)
		m_tInfo.iMp = 123;
	else
		m_tInfo.iMp = 111;
	

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);		
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tInfo.iMp)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;



	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::OnLoadData(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pInstance);

#pragma region TerrainRect
	_tchar szSize[MAX_PATH];
	ReadFile(hFile, &szSize, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	_int TerrainRectSize = _wtoi(szSize);

	for (_int i = 0; i < TerrainRectSize; ++i)
	{
		CTerrainRect::RECTINFO tRectInfo;

		ReadFile(hFile, &tRectInfo.vPos, sizeof(tRectInfo.vPos), &dwByte, nullptr);

		_tchar szTex[MAX_PATH];
		ReadFile(hFile, &szTex, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		tRectInfo.iTex = _wtoi(szTex);

		for (_uint j = 0; j < 4; j++)
		{
			_float3 vPos;
			_float2 vTex;

			ReadFile(hFile, &vPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &vTex, sizeof(_float2), &dwByte, nullptr);

			tRectInfo.VertexArray[j] = vPos;
			tRectInfo.TextureArray[j] = vTex;
		}

		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TerrainRect"), m_tInfo.iLevelIndex, TEXT("Layer_TerrainRect"), &tRectInfo)))
		{
			ERR_MSG(TEXT("Failed to Cloned : CTerrainRect"));
			return E_FAIL;
		}
	}
#pragma endregion TerrainRect

	CVIBuffer::VIBINFO VIBInfo;
	CVIBuffer_Terrain::VIBINFO_DERIVED VIBInfo_Derived;
	ZeroMemory(&VIBInfo, sizeof(CVIBuffer::VIBINFO));
	ZeroMemory(&VIBInfo_Derived, sizeof(CVIBuffer_Terrain::VIBINFO_DERIVED));

	ReadFile(hFile, &VIBInfo, sizeof(CVIBuffer::VIBINFO), &dwByte, nullptr);
	ReadFile(hFile, &VIBInfo_Derived, sizeof(CVIBuffer_Terrain::VIBINFO_DERIVED), &dwByte, nullptr);


	m_pVIBufferCom->Release_Buffer();
	m_pVIBufferCom->Set_VIBInfo(VIBInfo);
	m_pVIBufferCom->Set_VIBInfoDerived(VIBInfo_Derived);

	if (FAILED(m_pVIBufferCom->Load_Terrain()))
	{
		ERR_MSG(TEXT("Failed to Load Terrain"));
		return E_FAIL;
	}

	LPDIRECT3DVERTEXBUFFER9 VB = m_pVIBufferCom->Get_VB();
	LPDIRECT3DINDEXBUFFER9 IB = m_pVIBufferCom->Get_IB();

	VTXTEX* pVertices = nullptr;
	FACEINDICES32* pIndices = nullptr;

	VB->Lock(0, 0, (void**)&pVertices, 0);

	for (_uint i = 0; i < m_pVIBufferCom->Get_VIBInfo().m_iNumVertices; ++i)
	{
		_float3 vPos;
		_float2 vTex;
		ReadFile(hFile, vPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, vTex, sizeof(_float2), &dwByte, nullptr);

		pVertices[i].vPosition = vPos;
		pVertices[i].vTexture = vTex;
		m_pVIBufferCom->m_pVerticesPos.push_back(pVertices[i].vPosition);
	}

	VB->Unlock();

	IB->Lock(0, 0, (void**)&pIndices, 0);

	for (_uint i = 0; i < m_pVIBufferCom->Get_VIBInfo().m_iNumPrimitive; ++i)
	{
		ReadFile(hFile, &pIndices[i]._0, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIndices[i]._1, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &pIndices[i]._2, sizeof(_uint), &dwByte, nullptr);

		m_pVIBufferCom->m_pIndices32.push_back(pIndices[i]);
	}

	IB->Unlock();

	Safe_Release(pInstance);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), m_tInfo.iLevelIndex, TEXT("Prototype_Component_Texture_TerrainRect"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTerrain::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;	

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return S_OK;
}

HRESULT CTerrain::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain*	pInstance = new CTerrain(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CTerrain::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
