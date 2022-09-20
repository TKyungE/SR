#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CTransform;
END

class CTerrainRect final : public CGameObject
{
public:
	typedef struct tagRectInfo
	{
		_float3 VertexArray[4];
		_float2 TextureArray[4];

		_float3 vPos;
		_uint iTex; 
		_uint iLevelIndex;
	}RECTINFO;

private:
	CTerrainRect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTerrainRect(const CTerrainRect& rhs);
	virtual ~CTerrainRect() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_ShaderResource();
private:
	RECTINFO m_tRectInfo;
	_uint m_iIndex = 0;
	_uint m_iIndices[4];
	_bool m_isCloned = false;

	LPDIRECT3DVERTEXBUFFER9 m_pVBuffer = nullptr;
	LPDIRECT3DINDEXBUFFER9 m_pIBuffer = nullptr;

public:
	static CTerrainRect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};
