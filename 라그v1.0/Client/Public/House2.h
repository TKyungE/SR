#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect2;
class CVIBuffer_Cube;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CHouse2 final : public CGameObject
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
	CHouse2(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CHouse2(const CHouse2& rhs);
	virtual ~CHouse2() = default;
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;
private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom1 = nullptr;
	CTransform* m_pTransformCom2 = nullptr;
	CTransform* m_pTransformCom3 = nullptr;
	CTransform* m_pTransformCom4 = nullptr;
	CTransform* m_pTransformCom5 = nullptr;
	CVIBuffer_Cube* m_pVIBufferCube = nullptr;
	CVIBuffer_Rect2* m_pVIBuffer1 = nullptr;
	CVIBuffer_Rect2* m_pVIBuffer2 = nullptr;
	CVIBuffer_Rect2* m_pVIBuffer3 = nullptr;
	CVIBuffer_Rect2* m_pVIBuffer4 = nullptr;
	CTexture* m_pTextureCom1 = nullptr;
	CTexture* m_pTextureCom2 = nullptr;
	
	CCollider*	m_pColliderCom = nullptr;

	INDEXPOS	m_IndexPos;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);

	void SetPos(void);
	HRESULT RectHouse_Render(void);
public:
	static CHouse2* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END