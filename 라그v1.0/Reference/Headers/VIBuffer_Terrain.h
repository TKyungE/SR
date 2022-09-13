#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagVIBufferInfo_Derived
	{
		_uint m_iNumVerticesX;
		_uint m_iNumVerticesZ;
	}VIBINFO_DERIVED;

private:
	CVIBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	VIBINFO_DERIVED Get_VIBInfoDerived(void) { return m_tVIBInfo_Derived; }
	void Set_VIBInfoDerived(VIBINFO_DERIVED& tVIBInfo_Derived);

	HRESULT Load_Terrain(void);
	void Release_Buffer(void);

public:
	virtual _bool Picking(_float4x4 WorldMatrix, _float3* pPickPoint = nullptr) override;
	_float Compute_Height(const _float3& vWorldPos, const _float4x4& WorldMatrix, _float fOffset = 0.f);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize(void* pArg) override;

private:
	VIBINFO_DERIVED m_tVIBInfo_Derived;
public:
	static CVIBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END