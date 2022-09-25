#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CDust final : public CGameObject
{

private:
	CDust(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CDust(const CDust& rhs);
	virtual ~CDust() = default;
	
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
private:
	_uint		m_iCheck = 0;
	_float		m_fTime = 0.f;
	_uint		m_iIndex = 0;
private:
	HRESULT SetUp_Components(void);
	void OnBillBoard(void);
	
public:
	static CDust* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END