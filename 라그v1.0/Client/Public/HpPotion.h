#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CHpPotion final : public CGameObject
{
private:
	CHpPotion(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CHpPotion(const CHpPotion& rhs);
	virtual ~CHpPotion() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	void OnTerrain();
	void	OnBillboard();
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void CheckColl();
private:
	CGameObject*			m_StatInfo = nullptr;

public:
	static CHpPotion* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END