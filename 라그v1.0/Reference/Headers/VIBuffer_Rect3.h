#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect3 final : public CVIBuffer
{
private:
	CVIBuffer_Rect3(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rect3(const CVIBuffer_Rect3& rhs);
	virtual ~CVIBuffer_Rect3() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_Rect3* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END