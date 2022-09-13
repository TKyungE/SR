#include "stdafx.h"
#include "..\Public\SoundMgr.h"

CSoundMgr* CSoundMgr::m_pInstance = nullptr;

CSoundMgr::CSoundMgr()
{
	
}


CSoundMgr::~CSoundMgr()
{
}

void CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	Result = FMOD::System_Create(&m_System);
	if (Result != FMOD_OK)
		return;

	Result = m_System->getVersion(&Version);
	if (Result != FMOD_OK)
		return;
	else printf("FMOD version %08x\n", Version);
	
	Result = m_System->init(32, FMOD_INIT_NORMAL, extradriverdata);  // 시스템 초기화 : chaneel 32개까지 사용하겠다.
	if (Result != FMOD_OK)
		return;

	LoadSoundFile();
	LoadBGMFile();
}
void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();
	m_System->release();
	m_System->close();

}

void CSoundMgr::SetSoundVolume(CHANNELID _Channel, float fVolume)
{
	m_Channel[_Channel]->setVolume(fVolume);
}

void CSoundMgr::PlayEffect(TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	m_System->playSound(iter->second, 0, false, &m_Channel[SOUND_EFFECT]);
	m_Channel[SOUND_EFFECT]->setVolume(fVolume);
}

void CSoundMgr::BGM_Pause()
{
	m_Channel[SOUND_BGM]->setPaused(true);
}



void CSoundMgr::PlayBGM(TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;
	
	m_System->playSound(iter->second, 0, false, &m_Channel[SOUND_BGM]);
	m_Channel[SOUND_BGM]->setVolume(fVolume);
}

void CSoundMgr::LoadBGMFile()
{
	
	_finddatai64_t fd;
	intptr_t hFile;

	char szCurPath[128] = "../Sound/BGM/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../Sound/BGM/*.*", &fd);

	do {
		strcpy_s(szFullPath, szCurPath);

		strcat_s(szFullPath, fd.name);

		FMOD::Sound*     m_Sound = nullptr;

		Result = m_System->createSound(szFullPath, FMOD_LOOP_NORMAL, 0, &m_Sound);
			
		int iLength = strlen(fd.name) + 1;

		TCHAR* pSoundKey = new TCHAR[iLength];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

		MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, m_Sound);

	} while (_findnexti64(hFile, &fd) == 0);
		_findclose(hFile); 

	m_System->update();

}
void CSoundMgr::LoadSoundFile()
{

	_finddatai64_t fd;
	intptr_t hFile;

	char szCurPath[128] = "../Sound/effectSound/";
	char szFullPath[128] = "";
	hFile = _findfirsti64("../Sound/effectSound/*.*", &fd);
	
	do {
			strcpy_s(szFullPath, szCurPath);

			strcat_s(szFullPath, fd.name);

			FMOD::Sound*     m_Sound = nullptr;

			Result = m_System->createSound(szFullPath, FMOD_LOOP_OFF, 0, &m_Sound);

			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, m_Sound);

		} while (_findnexti64(hFile, &fd) == 0);
		_findclose(hFile); 

	m_System->update();

}