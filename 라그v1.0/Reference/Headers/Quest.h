#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CQuest abstract : public CBase
{
protected:
	CQuest(LPDIRECT3DDEVICE9 pGraphic_Device);
	CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

public:
	_bool Get_Clear(void) { return m_bClear; }

public:
	virtual HRESULT Initialize_Prototype(void);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(void);

protected:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = nullptr;
	_bool m_bClear;

public:
	virtual CQuest* Clone(void* pArg = nullptr) = 0;
	virtual void Free(void) override;
};

END