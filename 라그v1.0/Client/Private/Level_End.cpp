#include "stdafx.h"
#include "..\Public\Level_End.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SoundMgr.h"
#include "BackGroundRect.h" 
#include "Camera_Dynamic.h"
#include "KeyMgr.h"


CLevel_End::CLevel_End(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
	ZeroMemory(&Info, sizeof(CGameObject::INFO));
}

HRESULT CLevel_End::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO info;
	info.iLevelIndex = LEVEL_ENDING;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_ENDING, TEXT("Layer_Sky"), &info)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	Info.iMp = 1;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Fade"), LEVEL_ENDING, TEXT("Layer_Fade"), &info)))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Ending"), LEVEL_ENDING, TEXT("Layer_End"))))
		return E_FAIL;

	CSoundMgr::Get_Instance()->BGM_Stop();
	CSoundMgr::Get_Instance()->PlayBGM(L"TOWN.wav", fSOUND+0.1f);

	g_bEnd = true;

	return S_OK;
}

void CLevel_End::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CLevel_End::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("엔드레벨입니다."));
}




HRESULT CLevel_End::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.CameraDesc.vAt = _float3(0.f, 0.f, 0.f);

	CameraDesc.CameraDesc.fFovy = D3DXToRadian(100.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_ENDING, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_End * CLevel_End::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_End*	pInstance = new CLevel_End(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_End"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_End::Free()
{
	__super::Free();
}
