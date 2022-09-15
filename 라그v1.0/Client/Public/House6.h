#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect2;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CHouse6 final : public CGameObject
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
	CHouse6(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CHouse6(const CHouse6& rhs);
	virtual ~CHouse6() = default;
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
	CTransform* m_pTransformCom5 = nullptr;
	CTransform* m_pTransformCom6 = nullptr;


	CVIBuffer_Rect2*	m_pVIBufferCom = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom3 = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom4 = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom5 = nullptr;
	CVIBuffer_Rect2*	m_pVIBufferCom6 = nullptr;


	CTexture*	m_pTextureCom = nullptr;

	CCollider*	m_pColliderCom = nullptr;

	INDEXPOS m_IndexPos;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);

	void SetPos(void);
	HRESULT RectHouse_Render(void);
public:
	static CHouse6* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};
END
