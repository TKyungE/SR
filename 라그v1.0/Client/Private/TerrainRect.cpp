#include "stdafx.h"
#include "..\Public\TerrainRect.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Layer.h"

CTerrainRect::CTerrainRect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
	, m_isCloned(false)
{
	ZeroMemory(&m_tRectInfo, sizeof(RECTINFO));
}

CTerrainRect::CTerrainRect(const CTerrainRect & rhs)
	: CGameObject(rhs)
	, m_iIndex(rhs.m_iIndex)
	, m_isCloned(true)
{
	memcpy(&m_tRectInfo, &rhs.m_tRectInfo, sizeof(RECTINFO));
}

HRESULT CTerrainRect::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrainRect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tRectInfo, pArg, sizeof(RECTINFO));

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(4 * sizeof(VTXTEX), 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVBuffer, 0)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(2 * sizeof(FACEINDICES16), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIBuffer, nullptr)))
		return E_FAIL;

	VTXTEX* pVertices = nullptr;
	FACEINDICES16* pIndices = nullptr;

	m_pVBuffer->Lock(0, 0, (void**)&pVertices, 0);
	m_pIBuffer->Lock(0, 0, (void**)&pIndices, 0);

	for (_uint i = 0; i < 4; ++i)
	{
		pVertices[i].vPosition = m_tRectInfo.VertexArray[i];
		pVertices[i].vTexture = m_tRectInfo.TextureArray[i];
	}

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pVBuffer->Unlock();
	m_pIBuffer->Unlock();

	return S_OK;
}

void CTerrainRect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CTerrainRect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrainRect::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (m_tRectInfo.iLevelIndex != LEVEL_MAZE)
	{
		m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


		if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tRectInfo.iTex)))
			return E_FAIL;

		if (nullptr == m_pGraphic_Device)
			return E_FAIL;

		m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, sizeof(VTXTEX));
		m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		m_pGraphic_Device->SetIndices(m_pIBuffer);

		

		m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	}
	else
	{
		if (FAILED(SetUp_ShaderResource()))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pGraphic_Device->SetStreamSource(0, m_pVBuffer, 0, sizeof(VTXTEX));
		m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		m_pGraphic_Device->SetIndices(m_pIBuffer);

		m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		m_pShaderCom->End();
	}
	return S_OK;
}

HRESULT CTerrainRect::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TerrainRect"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CTerrainRect::SetUp_ShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;


	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix, PlayerWorldMatrix;
	_float4x4		ViewMatrixInv;
	_float4			vCamPosition;

	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	CLayer* pLayer = pInstance->Find_Layer(m_tRectInfo.iLevelIndex, TEXT("Layer_Player"));

	list<class CGameObject*> GameObject = pLayer->Get_Objects();

	PlayerWorldMatrix = GameObject.front()->Get_World();

	memcpy(&vCamPosition, &PlayerWorldMatrix.m[3][0], sizeof(_float4));

	Safe_Release(pInstance);

	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(m_tRectInfo.iTex))))
		return E_FAIL;
	return S_OK;
}

CTerrainRect * CTerrainRect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrainRect* pInstance = new CTerrainRect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMyTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrainRect::Clone(void * pArg)
{
	CTerrainRect* pInstance = new CTerrainRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMyTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CTerrainRect::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CTerrainRect::Free(void)
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pIBuffer);
		Safe_Release(m_pVBuffer);
	}
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}