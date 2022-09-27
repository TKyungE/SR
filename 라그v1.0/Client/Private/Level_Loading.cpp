#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Town.h"
#include "Component.h"
#include "Loading.h"
#include "Level_ChoboField.h"
#include "Level_MidBoss.h"
#include "Level_Town2.h"
#include "Level_Desert1.h"
#include "Level_Maze.h"
#include "SkyField.h"
#include "Level_FinalBoss.h"

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	
	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pGraphic_Device, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);
	if (m_eNextLevel != LEVEL_SKY)
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Loading"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Loading/%d.jpg"), 1))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Loading"), LEVEL_LOADING, TEXT("Layer_UI"))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkyLoading"), LEVEL_LOADING, TEXT("Layer_UI"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkyNPC"), LEVEL_LOADING, TEXT("Layer_UI"))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	if (true == m_pLoader->Get_Finished())
	{
		if (m_eNextLevel != LEVEL_SKY)
		{
			CLevel*			pNewLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pNewLevel = CLevel_Logo::Create(m_pGraphic_Device);
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLEVEL_GamePlay::Create(m_pGraphic_Device);
				break;
			case LEVEL_TOWN:
				pNewLevel = CTown::Create(m_pGraphic_Device);
				break;
			case LEVEL_CHOBOFIELD:
				pNewLevel = CLevel_ChoboField::Create(m_pGraphic_Device);
				break;
			case LEVEL_MIDBOSS:
				pNewLevel = CLevel_MidBoss::Create(m_pGraphic_Device);
				break;
			case LEVEL_TOWN2:
				pNewLevel = CLevel_Town2::Create(m_pGraphic_Device);
				break;
			case LEVEL_DESERT1:
				pNewLevel = CLevel_Desert1::Create(m_pGraphic_Device);
				break;
			case LEVEL_MAZE:
				pNewLevel = CLevel_Maze::Create(m_pGraphic_Device);
				break;
			case LEVEL_SKY:
				pNewLevel = CSkyField::Create(m_pGraphic_Device);
				break;
			case LEVEL_FINALBOSS:
				pNewLevel = CLevel_FinalBoss::Create(m_pGraphic_Device);
				break;
			}
		
			if (nullptr == pNewLevel)
				return;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (nullptr == pGameInstance)
				return;
			Safe_AddRef(pGameInstance);

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return;

			Safe_Release(pGameInstance);
		}		
		else if(g_bSky)
		{
			CLevel*			pNewLevel = nullptr;

			pNewLevel = CSkyField::Create(m_pGraphic_Device);
			
			if (nullptr == pNewLevel)
				return;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (nullptr == pGameInstance)
				return;
			Safe_AddRef(pGameInstance);

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				return;

			Safe_Release(pGameInstance);
		}

	}
}

void CLevel_Loading::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

CLevel_Loading * CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
