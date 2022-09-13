#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;
public:
	HRESULT Initialize(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Tick();
	_bool IsinFrustum(_float3 vPos, _float3 vScale);
private:
	_float3 m_vProj[8];
	_float3 m_vProj2[6][3];
	LPDIRECT3DDEVICE9 m_pGraphic_Device;
public:
	virtual void Free() override;
};

END
