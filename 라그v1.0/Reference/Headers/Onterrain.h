#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL COnterrain final : public CComponent
{
private:
	COnterrain(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual ~COnterrain() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Set_TerrainVIBuffer(class CVIBuffer_Terrain* _pVIBuffer);
	HRESULT Get_OnTerrainY(_float3 vecTarget, _float* pOut);

private:
	static _tchar* m_pVIBufferTag;
	class CVIBuffer_Terrain* m_pVIBuffer = nullptr;

public:
	static COnterrain* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free(void);
};

END