#pragma once

#include "Base.h"

BEGIN(Engine)

class CQuestManager final : public CBase
{
	DECLARE_SINGLETON(CQuestManager)
private:
	CQuestManager();
	virtual ~CQuestManager() = default;

public:
	HRESULT Add_Quest(class CQuest* pQuest, _int* pOut);
	void Tick(void);
	HRESULT Release_Quest(_int iIndex);

private:
	list<class CQuest*> m_Quest;
	typedef list<class CQuest*> QUESTS;

public:
	virtual void Free(void) override;
};

END