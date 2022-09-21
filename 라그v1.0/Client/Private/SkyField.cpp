#include "stdafx.h"
#include "..\Public\SkyField.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "SoundMgr.h"
#include "KeyMgr.h"
#include "BackGroundRect.h" 
#include "BackGroundTree.h"
#include "Level_Loading.h"
#include "House.h"
#include "House2.h"
#include "BackGroundTree.h"
#include "Layer.h"
#include "Portal.h"
#include "Transparent_Wall.h"

CSkyField::CSkyField(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
	ZeroMemory(&Info, sizeof(CGameObject::INFO));
}

HRESULT CSkyField::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	g_bFirst = false;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;


	fSound = fSOUND;
	/*CSoundMgr::Get_Instance()->BGM_Pause();
	CSoundMgr::Get_Instance()->PlayBGM(L"Stage1_Sound.wav", fSOUND);*/
	CSoundMgr::Get_Instance()->PlayBGM(L"Boss_Sound1.wav", fSOUND);


	return S_OK;
}

void CSkyField::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//fSound += (pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*fTimeDelta) / 100;

	if (CKeyMgr::Get_Instance()->Key_Down('O'))
		fSound += 0.01f;
	if (CKeyMgr::Get_Instance()->Key_Down('P'))
		fSound -= 0.01f;

	if (fSound > 1.f)
		fSound = 1.f;
	if (fSound < 0.f)
		fSound = 0.f;

	CSoundMgr::Get_Instance()->SetSoundVolume(SOUND_BGM, fSound);

	if (GetKeyState('Y') < 0)
	{
		if (!g_bCollider)
			g_bCollider = true;
	}
	if (GetKeyState('U') < 0)
	{
		if (g_bCollider)
			g_bCollider = false;
	}



	Safe_Release(pGameInstance);
}

void CSkyField::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Open_Level();

	SetWindowText(g_hWnd, TEXT("스카이레벨입니다."));
}

HRESULT CSkyField::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.iLevelIndex = LEVEL_SKY;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_SKY, pLayerTag, &tInfo)))
		return E_FAIL;
	
	for (int i = 0; i < 100; ++i)
	{
		_float iSour = rand() % 30000 * 0.001f;
		_float iTemp = rand() % 20000 * 0.001f;
		_float iDest = rand() % 20000 * 0.001f;

		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y + iTemp;
		tInfo.vPos.z = vPos.z + iDest;
	

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Cloud"), LEVEL_SKY, pLayerTag, &tInfo)))
			return E_FAIL;

	}






	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkyField::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Get_Info();

	memcpy(&Info, &tInfo, sizeof(CGameObject::INFO));

	Info.vPos = {10.f,10.f,10.f};

	Info.iLevelIndex = LEVEL_SKY;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_SKY, pLayerTag, &Info)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkyField::Ready_Layer_Monster(const _tchar * pLayerTag)
{


	return S_OK;
}


HRESULT CSkyField::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	CameraDesc.CameraDesc.Info.pTarget = Info.pTarget;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_SKY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSkyField::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.iLevelIndex = LEVEL_SKY;
	tInfo.bHit = false;
	tInfo.bDead = false;
	tInfo.pTarget = Info.pTarget;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_SKY, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_QuestUI"), LEVEL_SKY, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CSkyField::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	
	return S_OK;
}

void CSkyField::Create_Monster(_float fTimeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	fRainTime += fTimeDelta;
	if (fRainTime > 1.f)
	{
		fRainTime = 0.f;
		
		_float iSour = rand() % 30000 * 0.001f;
		_float iTemp = rand() % 20000 * 0.001f;
		_float iDest = rand() % 20000 * 0.001f;
			
		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y + iTemp;
		tInfo.vPos.z = vPos.z + iDest;
		tInfo.iLevelIndex = LEVEL_SKY;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dandelion"), LEVEL_SKY,TEXT("Layer_Monster"), &tInfo)))
				return;

		
	}
	Safe_Release(pGameInstance);

}

void CSkyField::Open_Level(void)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_Portal")))
	{
		for (auto& iter : pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_Portal"))->Get_Objects())
		{
			if (dynamic_cast<CPortal*>(iter)->Get_Level())
			{
				pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Set_Info(pGameInstance->Find_Layer(LEVEL_SKY, TEXT("Layer_Player"))->Get_Objects().front()->Get_Info());
				LEVEL eLevel = (LEVEL)iter->Get_Info().iNextLevel;
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevel))))
					return;
			}
		}
	}
	Safe_Release(pGameInstance);
}

CSkyField * CSkyField::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkyField*	pInstance = new CSkyField(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkyField"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyField::Free()
{
	__super::Free();
}
