#pragma once

#include "Client_Defines.h"
#include "Quest.h"

BEGIN(Client)

class CHuntQuest1 final : public CQuest
{
public:
	typedef struct tagQuestInfo
	{
		_uint iHuntGoal;
		LEVEL eLevelIndex;
		MONSTERTYPE eMonType;
		CQuest* pQuest;
	}QINFO;

private:
	CHuntQuest1(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHuntQuest1(const CHuntQuest1& rhs);
	virtual ~CHuntQuest1() = default;

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