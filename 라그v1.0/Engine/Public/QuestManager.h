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
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CQuest* pPrototype);
	class CQuest* Add_Quest(const _tchar* pPrototypeTag, const _tchar* pQuestTag, void* pArg = nullptr);
	void Tick(void);

private:
	map<const _tchar*, class CQuest*>			m_Prototypes;
	typedef map<const _tchar*, class CQuest*>	PROTOTYPES;

private:
	map<const _tchar*, class CQuest*>			m_Actives;
	map<const _tchar*, class CQuest*>			m_Finished;
	typedef map<const _tchar*, class CQuest*>	QUESTS;

private:
	class CQuest* Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free(void) override;
};

END