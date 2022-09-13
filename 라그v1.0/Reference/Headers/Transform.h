#pragma once

/* 월드스페이스에서의 상태를 보관한다.(월드행렬) */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORMDESC;
private:
	CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_float3 Get_State(STATE eState) const {		
		return *(_float3*)&m_WorldMatrix.m[eState][0];
	}

	_float4x4 Get_WorldMatrix() const {
		return m_WorldMatrix;
	}

	_float3 Get_Scale(); 

	void Set_State(STATE eState, _float3 vState) {
		memcpy(&m_WorldMatrix.m[eState][0], &vState, sizeof(_float3));
	}
	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) {
		m_TransformDesc = TransformDesc;
	}
	const TRANSFORMDESC& Get_TransformDesc(void) const { return m_TransformDesc; }

	void Set_Scaled(_float3 vScale);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_OnGraphicDev();

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Turn(_float3 vAxis, _float fTimeDelta);
	void LookAt(_float3 vPoint);

private:
	_float4x4			m_WorldMatrix;
	TRANSFORMDESC		m_TransformDesc;

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END