#pragma once

#include "VIBuffer_Cube.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Transform(_float4x4 matTargetWorld, _float fScale);
	HRESULT Render(void);
	_float3 Find_MinPoint(void);
	_float3 Find_MaxPoint(void);
	void Turn(_float3 vAxis, _float fTimeDelta);
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT Add_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent** ppOut, void* pArg = nullptr);
	CComponent* Find_Component(const _tchar* pComponentTag);
	
private:
	CVIBuffer_Cube* m_pVIBufferCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	
private:
	map<const _tchar*, class CComponent*> m_Components;
	_uint m_iLevelIndex = 0;

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free(void) override;
};

END