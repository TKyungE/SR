#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CFireBall final : public CGameObject
{
private:
	CFireBall(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFireBall(const CFireBall& rhs);
	virtual ~CFireBall() = default;
public:
	enum STATE { IDLE, STATE_END };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_float				m_fDeadTime = 0.f;
	_float3				vLook;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	HRESULT TextureRender();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	OnBillboard();
	void CheckColl();
	_float3 Get_CollisionPos(CGameObject* pDest, CGameObject* pSour);
public:
	static CFireBall* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END