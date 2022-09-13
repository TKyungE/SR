#include "..\Public\QuestManager.h"
#include "Quest.h"

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
{
}

HRESULT CQuestManager::Add_Quest(CQuest * pQuest, _int* pOut)
{
	if (nullptr == pQuest)
		return E_FAIL;
	
	_int i = 0;

	for (auto& iter : m_Quest)
		++i;

	pQuest->Set_Index(i);

	m_Quest.push_back(pQuest);

	Safe_AddRef(pQuest);

	*pOut = i;

	return S_OK;
}

void CQuestManager::Tick(void)
{
	for (auto& iter : m_Quest)
		iter->Tick();
}

HRESULT CQuestManager::Release_Quest(_int iIndex)
{
	if (iIndex < 0 || iIndex > m_Quest.size())
		return E_FAIL;
	
	for (auto iter = m_Quest.begin(); iter != m_Quest.end();)
	{
		if (iIndex == (*iter)->Get_Index())
		{
			Safe_Release(*iter);
			iter = m_Quest.erase(iter);
		}
		else
			++iter;
	}

	return S_OK;
}

void CQuestManager::Free(void)
{
	for (auto& iter : m_Quest)
		Safe_Release(iter);
	
	m_Quest.clear();
}
