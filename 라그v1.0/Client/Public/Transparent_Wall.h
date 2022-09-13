#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Cube;
class CCollider;
class CRenderer;
END

BEGIN(Client)
class Transparent_Wall final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_float3		vScale;
	}WALL;
private:
	Transparent_Wall(LPDIRECT3DDEVICE9 _pGraphic_Device);
	Transparent_Wall(const Transparent_Wall& rhs);
	virtual ~Transparent_Wall() = default;
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;
private:
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Cube*	m_pVIBufferCube = nullptr;
	CCollider*	m_pColliderCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;

	WALL	m_IndexPos;
private:
	HRESULT SetUp_Components(void);
public:
	static Transparent_Wall* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END