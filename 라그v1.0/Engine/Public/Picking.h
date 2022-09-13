#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)
private:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Tick(void);

	void Transform_ToLocalSpace(_float4x4 WorldMatrixInverse);
	_bool Intersect_InWorldSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut);
	_bool Intersect_InLocalSpace(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut);
	
private:
	HWND m_hWnd;
	LPDIRECT3DDEVICE9 m_pGraphic_Device = nullptr;
	
	_float3 m_MouseRayPos;
	_float3 m_MouseRayDir;
	_float3 m_MouseRayPos_Local;
	_float3 m_MouseRayDir_Local;

public:
	virtual void Free(void) override;
};

END