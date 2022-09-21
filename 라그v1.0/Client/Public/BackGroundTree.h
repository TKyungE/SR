#pragma once


#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CBackGroundTree final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_float3		vScale;
		_uint	iIndex;
		_uint	iLevelIndex;
	}INDEXPOS;
private:
	CBackGroundTree(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CBackGroundTree(const CBackGroundTree& rhs);
	virtual ~CBackGroundTree() = default;
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;
private:
	INDEXPOS	m_IndexPos;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Cube* m_pVIBuffer = nullptr;
	CTexture* m_pTextureCom = nullptr;

	CTexture* m_pRectTexture = nullptr;
	CTransform* m_pRectTransform = nullptr;
	CVIBuffer_Rect* m_VIBufferRect = nullptr;

	CTransform* m_pRectTransform2 = nullptr;
	CVIBuffer_Rect* m_VIBufferRect2 = nullptr;
	CTexture* m_pRectTexture2 = nullptr;


	CCollider*	m_pColliderCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
	CShader*	m_pShaderCom2 = nullptr;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT SetUp_Rect_RenderState();
	HRESULT Release_RenderState(void);
	void OnTerrain(void);

public:
	static CBackGroundTree* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;

};
END
