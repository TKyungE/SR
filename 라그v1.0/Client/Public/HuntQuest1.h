#pragma once

#include "Client_Defines.h"
#include "ClientQuest.h"

BEGIN(Client)

class CHuntQuest1 final : public CClientQuest
{
public:
	typedef struct tagQuestInfo
	{
		_uint iHuntGoal;
		MONSTERTYPE eMonType;
	}QINFO;

private:
	CHuntQuest1(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHuntQuest1(const CHuntQuest1& rhs);
	virtual ~CHuntQuest1() = default;

public:
	QINFO Get_QInfo(void) { return m_tQInfo; }
	void Increase_Count(void) { ++iCount; }

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(void) override;

private:
	QINFO m_tQInfo;
	_uint iCount;

public:
	static CHuntQuest1* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CHuntQuest1* Clone(void* pArg = nullptr) override;
	virtual void Free(void) override;
};

END