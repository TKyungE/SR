#include "..\Public\QuestManager.h"
#include "Quest.h"

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

HRESULT CQuestManager::Add_Prototype(const _tchar * pPrototypeTag, CQuest * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;
	
	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CQuest * CQuestManager::Add_Quest(const _tchar * pPrototypeTag, const _tchar* pQuestTag, void * pArg)
{
	CQuest* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CQuest* pQuest = pPrototype->Clone(pArg);
	if (nullptr == pQuest)
		return nullptr;
	
	m_Actives.emplace(pQuestTag, pQuest);

	return pQuest;
}

void CQuestManager::Tick(void)
{
	for (auto& Pair : m_Actives)
	{
		Pair.second->Tick();
		if (Pair.second->Get_Clear())
		{
			m_Finished.emplace(Pair.first, Pair.second);
			m_Actives.erase(Pair);
		}
	}
}

CQuest * CQuestManager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;
	
	return iter->second;
}

void CQuestManager::Free(void)
{
	for (auto& Pair : m_Actives)
		Safe_Release(Pair.second);

	m_Actives.clear();

	for (auto& Pair : m_Finished)
		Safe_Release(Pair.second);
	
	m_Finished.clear();

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	
	m_Prototypes.clear();
}
