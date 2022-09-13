#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDefaultAttack final : public CGameObject
{
private:
	CDefaultAttack(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDefaultAttack(const CDefaultAttack& rhs);
	virtual ~CDefaultAttack() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:

	_float				m_fDeadTime = 0.f;
	
private:
	HRESULT SetUp_Components();

public:
	static CDefaultAttack* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
	void CheckColl();
	_float3 Get_CollisionPos(CGameObject* pDest, CGameObject* pSour);
};

END