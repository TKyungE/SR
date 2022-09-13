#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_WingRect final : public CVIBuffer
{
private:
	CVIBuffer_WingRect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_WingRect(const CVIBuffer_WingRect& rhs);
	virtual ~CVIBuffer_WingRect() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CVIBuffer_WingRect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END