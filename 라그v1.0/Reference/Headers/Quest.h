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
	_int Get_Index(void) { return m_iIndex; }
	void Set_Index(_int iIndex) { m_iIndex = iIndex; }

public:
	virtual HRESULT Initialize_Prototype(void);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(void);

protected:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = nullptr;
	_bool m_bActive;
	_bool m_bClear;
	_int m_iIndex;

public:
	virtual CQuest* Clone(void* pArg = nullptr) = 0;
	virtual void Free(void) override;
};

END