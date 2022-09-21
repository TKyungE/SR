#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CHouse final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_float3		vScale;
		_uint	iIndex;
		_uint iTrun;
	}INDEXPOS;

private:
	CHouse(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CHouse(const CHouse& rhs);
	virtual ~CHouse() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTransform* m_pTransformCom2 = nullptr;
	CTransform* m_pTransformCom3 = nullptr;
	CTransform* m_pTransformCom4 = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer2 = nullptr;
	CVIBuffer_Cube* m_pVIBufferCube = nullptr;
	CVIBuffer_Cube* m_pVIBufferCube2 = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureCom2 = nullptr;

	CCollider* m_pColliderCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
	CShader*	m_pShaderCom2 = nullptr;
private:
	INDEXPOS	m_IndexPos;

private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void OnTerrain(void);
	void OnBillBoard(void);
	void Set_vPos();
	HRESULT House_Render();
public:
	static CHouse* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END