#include "stdafx.h"
#include "..\Public\QuestManager.h"
#include "ClientQuest.h"

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

HRESULT CQuestManager::Add_Prototype(const _tchar * pPrototypeTag, CClientQuest * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;
	
	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CClientQuest * CQuestManager::Add_Quest(const _tchar * pPrototypeTag, const _tchar* pQuestTag, void * pArg)
{
	CClientQuest* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CClientQuest* pQuest = pPrototype->Clone(pArg);
	if (nullptr == pQuest)
		return nullptr;
	
	m_Actives.emplace(pQuestTag, pQuest);

	return pQuest;
}

void CQuestManager::Tick(void)
{
	for (auto iter = m_Actives.begin(); iter != m_Actives.end();)
	{
		iter->second->Tick();
		if (iter->second->Get_Clear())
		{
			m_Finished.emplace(iter->first, iter->second);
			iter = m_Actives.erase(iter);
		}
		else
			++iter;
	}
}

_bool CQuestManager::Find_Finish(const _tchar * pQuestTag)
{
	auto iter = find_if(m_Finished.begin(), m_Finished.end(), CTag_Finder(pQuestTag));

	if (iter == m_Finished.end())
		return false;

	return true;
}

_bool CQuestManager::Find_Active(const _tchar * pQuestTag)
{
	auto iter = find_if(m_Actives.begin(), m_Actives.end(), CTag_Finder(pQuestTag));

	if (iter == m_Actives.end())
		return false;

	return true;
}

void CQuestManager::Increase_Count(MONSTERTYPE eType)
{
	for (auto& iter : m_Actives)
	{
		if (eType == dynamic_cast<CHuntQuest1*>((&iter)->second)->Get_QInfo().eMonType)
		{
			dynamic_cast<CHuntQuest1*>((&iter)->second)->Increase_Count();
		}
	}
}

CClientQuest * CQuestManager::Find_Prototype(const _tchar * pPrototypeTag)
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