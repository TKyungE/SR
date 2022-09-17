#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagCameraDesc_Derived
	{
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
private:
	CCamera_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	void CameraRotationX(_float fTimeDelta, _float fIncrease);
	
private:
	_float3 m_vecCameraNormal;
	_float m_YfAngle = 40.f;
	_float m_XfAngle = 0.f;
	_float4x4 m_matRotY;
	_float4x4 m_matRotX;
	_bool  m_bTrue = false;
	_bool m_bWheelMove = false;
	_bool m_bTest = false;

public:
	static CCamera_Dynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END