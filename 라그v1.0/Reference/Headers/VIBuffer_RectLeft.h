#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectLeft final : public CVIBuffer
{
private:
	CVIBuffer_RectLeft(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_RectLeft(const CVIBuffer_RectLeft& rhs);
	virtual ~CVIBuffer_RectLeft() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_RectLeft* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END