#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
END

BEGIN(Client)

class CSoundMgr
{
public:
	static CSoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr;

		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	CSoundMgr();
	virtual~CSoundMgr();

public:
	void Initialize();


public:
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void PlayEffect(TCHAR * pSoundKey, float fVolume);
	void BGM_Stop();
	void Free();
	void SetSoundVolume(CHANNELID  _Channel, float fVolume);
private:
	void LoadBGMFile();
	void LoadSoundFile();
private:
	static CSoundMgr* m_pInstance;

	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_mapSound;


	FMOD::System*    m_System = nullptr;
	FMOD::Channel*   m_Channel[MAXCHANNEL];
	FMOD_RESULT       Result;
	unsigned int      Version;
	void*             extradriverdata = nullptr;
};

END