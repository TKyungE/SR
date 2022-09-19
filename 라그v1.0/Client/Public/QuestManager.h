#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "HuntQuest1.h"

BEGIN(Client)

class CQuestManager final : public CBase
{
	DECLARE_SINGLETON(CQuestManager)
private:
	CQuestManager();
	virtual ~CQuestManager() = default;

public:
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CClientQuest* pPrototype);
	HRESULT Add_Quest(const _tchar* pPrototypeTag, const _tchar* pQuestTag, void* pArg = nullptr);
	void Tick(void);
	class CClientQuest* Find_Finish(const _tchar* pQuestTag);
	class CClientQuest* Find_Active(const _tchar* pQuestTag);
	void Increase_Count(MONSTERTYPE eType);
	HRESULT Clear_Quest(const _tchar* pQuestTag);
	
private:
	map<const _tchar*, class CClientQuest*>			m_Prototypes;
	typedef map<const _tchar*, class CClientQuest*>	PROTOTYPES;

private:
	map<const _tchar*, class CClientQuest*>			m_Actives;
	map<const _tchar*, class CClientQuest*>			m_Finished;
	typedef map<const _tchar*, class CClientQuest*>	QUESTS;

private:
	class CClientQuest* Find_Prototype(const _tchar* pPrototypeTag);

public:
	virtual void Free(void) override;
};

END