#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CClientQuest abstract : public CBase
{
protected:
	CClientQuest(LPDIRECT3DDEVICE9 pGraphic_Device);
	CClientQuest(const CClientQuest& rhs);
	virtual ~CClientQuest() = default;

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
	virtual CClientQuest* Clone(void* pArg = nullptr) = 0;
	virtual void Free(void) override;
};

END