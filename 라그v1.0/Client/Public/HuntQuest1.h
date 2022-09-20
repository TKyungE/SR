#pragma once

#include "Client_Defines.h"
#include "ClientQuest.h"

BEGIN(Client)

class CHuntQuest final : public CClientQuest
{
public:
	typedef struct tagQuestInfo_Derived
	{
		_uint iCount;
		_uint* pHuntGoal;
		MONSTERTYPE* pMonType;
		QINFO tQInfo;
	}QINFO_DERIVED;

private:
	CHuntQuest(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHuntQuest(const CHuntQuest& rhs);
	virtual ~CHuntQuest() = default;

public:
	QINFO_DERIVED Get_QInfoDerived(void) { return m_tQInfo; }
	void Increase_Count(MONSTERTYPE eType);
	_uint* Get_Count(void) { return m_pCount; }

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(void) override;
	virtual QINFO Get_QInfo(void) override { return m_tQInfo.tQInfo; }

private:
	QINFO_DERIVED m_tQInfo;
	_uint* m_pCount = nullptr;

public:
	static CHuntQuest* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CHuntQuest* Clone(void* pArg = nullptr) override;
	virtual void Free(void) override;
};

END