#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{	
		_float3				vEye;
		_float3				vAt;

		_float				fAspect;
		_float				fFovy;
		_float				fNear;
		_float				fFar;

		CGameObject::INFO Info;

		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;
protected:
	CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	static _tchar*					m_pTransformTag;
	class CTransform*				m_pTransform = nullptr;
	CAMERADESC						m_CameraDesc;

protected:
	HRESULT Bind_OnGraphicDev();


public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual _float4x4 Get_World(void) = 0;
	virtual void Free() override;
};

END