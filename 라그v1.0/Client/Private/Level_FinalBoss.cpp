#include "stdafx.h"
#include "..\Public\Level_FinalBoss.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "SoundMgr.h"
#include "Transparent_Wall.h"
#include "Layer.h"
#include "Portal.h"
#include "Level_Loading.h"
#include "QuestManager.h"

CLevel_FinalBoss::CLevel_FinalBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
	ZeroMemory(&Info, sizeof(CGameObject::INFO));
}

HRESULT CLevel_FinalBoss::Initialize(void)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	LoadData();

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Portal(TEXT("Layer_Portal"))))
		return E_FAIL;*/

	fSound = fSOUND;
	CSoundMgr::Get_Instance()->BGM_Stop();
	CSoundMgr::Get_Instance()->PlayBGM(L"FinalBoss.wav", fSOUND + 0.1f);

	return S_OK;
}

void CLevel_FinalBoss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	
	Safe_AddRef(pGameInstance);

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

	//Create_Rain(fTimeDelta);

	CQuestManager* pQuestManager = CQuestManager::Get_Instance();
	if (nullptr == pQuestManager)
		return;
	
	Safe_AddRef(pQuestManager);

	if (nullptr != pQuestManager->Find_Active(TEXT("Quest_HuntQuestFinal")) && (pQuestManager->Find_Active(TEXT("Quest_HuntQuestFinal"))->Get_Clear() && !m_bPortal))
	{
		if (FAILED(Ready_Layer_Portal(TEXT("Layer_Portal"))))
			return;

		m_bPortal = true;
	}

	Safe_Release(pQuestManager);

	Safe_Release(pGameInstance);
}

void CLevel_FinalBoss::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("최종보스 레벨입니다."));

	Open_Level();
}

HRESULT CLevel_FinalBoss::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO info;
	ZeroMemory(&info, sizeof(CGameObject::INFO));
	info.pstrPath = TEXT("../../Data/Terrain/FinalBoss.dat");
	info.iLevelIndex = LEVEL_FINALBOSS;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_FINALBOSS, pLayerTag, &info)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_FINALBOSS, pLayerTag, &info)))
		return E_FAIL;

	for (auto& iter : m_vecWall)
	{
		Transparent_Wall::WALL Wall;
		ZeroMemory(&Wall, sizeof(Transparent_Wall::WALL));

		Wall.vPos = iter.BackGroundPos;
		Wall.vScale = iter.vScale;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Transparent_Wall"), LEVEL_FINALBOSS, pLayerTag, &Wall)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Get_Info();

	memcpy(&Info, &tInfo, sizeof(CGameObject::INFO));
	if (Info.iLevelIndex == LEVEL_MAZE)
		Info.vPos = m_vPlayerPos;
	else
		Info.vPos = m_vBackPos;

	Info.iLevelIndex = LEVEL_FINALBOSS;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_FINALBOSS, pLayerTag, &Info)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	Info.iLevelIndex = LEVEL_FINALBOSS;
	Info.vPos = { 7.f,1.f,8.f };
	Info.iMonsterType = (_int)MON_RICH;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Rich"), LEVEL_GAMEPLAY, TEXT("Layer_Boss"), &Info)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

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
	CameraDesc.CameraDesc.Info.iLevelIndex = LEVEL_FINALBOSS;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_FINALBOSS, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.iLevelIndex = LEVEL_FINALBOSS;
	tInfo.bHit = false;
	tInfo.bDead = false;
	tInfo.pTarget = Info.pTarget;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_FINALBOSS, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_QuestUI"), LEVEL_FINALBOSS, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LetterBox"), LEVEL_FINALBOSS, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_FinalBoss::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	auto iter = m_vecPortal.begin();

	_uint iCount = 0;
	for (; iter != m_vecPortal.end(); ++iter)
	{
		if (iCount > 0)
		{
			iCount = 0;
			break;
		}
		CGameObject::INFO tInfo;
		tInfo.iLevelIndex = LEVEL_FINALBOSS;
		tInfo.vPos = iter->BackGroundPos;
		tInfo.vScale = iter->vScale;
		tInfo.iNextLevel = LEVEL_TOWN2;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_FINALBOSS, pLayerTag, &tInfo)))
			return E_FAIL;

		++iCount;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

_float3 CLevel_FinalBoss::Get_CollisionPos(CGameObject * pDest, CGameObject * pSour)
{
	_float3 vLook = *(_float3*)&pDest->Get_World().m[3][0] - *(_float3*)&pSour->Get_World().m[3][0];
	D3DXVec3Normalize(&vLook, &vLook);

	vLook = vLook * 0.5f;

	_float Angle = D3DXVec3Dot(&vLook, (_float3*)&pSour->Get_World().m[1][0]);
	_float3 SourUp = *(_float3*)&pSour->Get_World().m[1][0];
	_float3 Proj = (Angle / D3DXVec3Length(&SourUp) * D3DXVec3Length(&SourUp)) * *(_float3*)&pSour->Get_World().m[1][0];

	_float3 CollisionPos = *(_float3*)&pSour->Get_World().m[3][0] + Proj;
	CollisionPos.y -= 0.5;
	CollisionPos.x = pDest->Get_World().m[3][0];

	return CollisionPos;
}

void CLevel_FinalBoss::Open_Level(void)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Find_Layer(LEVEL_FINALBOSS, TEXT("Layer_Portal")))
	{
		for (auto& iter : pGameInstance->Find_Layer(LEVEL_FINALBOSS, TEXT("Layer_Portal"))->Get_Objects())
		{
			if (dynamic_cast<CPortal*>(iter)->Get_Level())
			{
				pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Set_Info(pGameInstance->Find_Layer(LEVEL_FINALBOSS, TEXT("Layer_Player"))->Get_Objects().front()->Get_Info());
				LEVEL eLevel = (LEVEL)iter->Get_Info().iNextLevel;
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevel))))
					return;
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CLevel_FinalBoss::LoadData(void)
{
	HANDLE hFile = CreateFile(TEXT("../../Data/FinalBossPos.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte = 0;


	_float3 vPos1, vPos2;
	_uint iMSize, iIndexSize, iTreeSize, iHouseSize, iHouse2Size, iPortalSize, iNPCSize, iWallSize;
	_tchar str1[MAX_PATH];
	_tchar str2[MAX_PATH];
	_tchar str3[MAX_PATH];
	_tchar str4[MAX_PATH];
	_tchar str5[MAX_PATH];
	_tchar str6[MAX_PATH];
	_tchar str7[MAX_PATH];
	_tchar str8[MAX_PATH];

	ReadFile(hFile, vPos1, sizeof(_float3), &dwByte, nullptr);
	m_vPlayerPos = vPos1;

	ReadFile(hFile, vPos2, sizeof(_float3), &dwByte, nullptr);
	m_vBackPos = vPos2;

	ReadFile(hFile, str1, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str2, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str4, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str5, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str6, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str7, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, str8, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);



	iMSize = stoi(str1);
	iIndexSize = stoi(str2);
	iTreeSize = stoi(str3);
	iHouseSize = stoi(str4);
	iHouse2Size = stoi(str5);
	iPortalSize = stoi(str6);
	iNPCSize = stoi(str7);
	iWallSize = stoi(str8);



	while (true)
	{
		for (_uint i = 0; i < iMSize; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 Pos;

			ReadFile(hFile, &Pos, sizeof(_float3), &dwByte, nullptr);

			_float3 vPos;

			vPos = Pos;

			m_vMonsterPos1.push_back(vPos);
		}

		for (_uint i = 0; i < iIndexSize; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 BackPos, Scale;
			_tchar str3[MAX_PATH];
			_uint Index;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);

			INDEXPOS IndexPos;

			IndexPos.BackGroundPos = BackPos;
			IndexPos.vScale = Scale;
			IndexPos.iIndex = Index;

			m_vecIndex.push_back(IndexPos);
		}

		for (_uint i = 0; i < iTreeSize; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 BackPos, Scale;
			_tchar str3[MAX_PATH];
			_uint Index;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);

			INDEXPOS TreePos;

			TreePos.BackGroundPos = BackPos;
			TreePos.vScale = Scale;
			TreePos.iIndex = Index;

			m_vecTree.push_back(TreePos);
		}

		for (_uint i = 0; i < iHouseSize; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 BackPos, Scale;
			_tchar str3[MAX_PATH];
			_tchar str4[MAX_PATH];
			_uint Index, turn;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, str4, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);
			turn = stoi(str4);

			INDEXPOS HousePos;

			HousePos.BackGroundPos = BackPos;
			HousePos.vScale = Scale;
			HousePos.iIndex = Index;
			HousePos.iTurn = turn;

			m_vecHouse.push_back(HousePos);
		}

		for (_uint i = 0; i < iHouse2Size; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 BackPos, Scale;
			_tchar str3[MAX_PATH];
			_tchar str4[MAX_PATH];
			_uint Index, turn;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, str4, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);
			turn = stoi(str4);

			INDEXPOS HousePos;

			HousePos.BackGroundPos = BackPos;
			HousePos.vScale = Scale;
			HousePos.iIndex = Index;
			HousePos.iTurn = turn;
			m_vecHouse2.push_back(HousePos);
		}

		for (_uint i = 0; i < iPortalSize; ++i)
		{
			if (0 == dwByte)
				break;

			_float3 BackPos, Scale;
			_tchar str3[MAX_PATH];
			_tchar str4[MAX_PATH];
			_uint Index, turn;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, str4, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);
			turn = stoi(str4);

			INDEXPOS PortalPos;

			PortalPos.BackGroundPos = BackPos;
			PortalPos.vScale = Scale;
			PortalPos.iIndex = Index;
			PortalPos.iTurn = turn;
			m_vecPortal.push_back(PortalPos);
		}

		for (_uint i = 0; i < iNPCSize; ++i)
		{
			if (0 == dwByte)
				break;
			_float3 BackPos;
			_tchar str3[MAX_PATH];
			_uint Index;

			ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, str3, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			Index = stoi(str3);
			INDEXPOS NPCPos;

			NPCPos.BackGroundPos = BackPos;
			NPCPos.iIndex = Index;

			m_vecNPC.push_back(NPCPos);
		}

		for (_uint i = 0; i < iWallSize; ++i)
		{
		if (0 == dwByte)
		break;
		_float3 BackPos, Scale;

		ReadFile(hFile, &BackPos, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &Scale, sizeof(_float3), &dwByte, nullptr);

		INDEXPOS WallPos;


		WallPos.BackGroundPos = BackPos;
		WallPos.vScale = Scale;

		m_vecWall.push_back(WallPos);
		}

		if (0 == dwByte)
			break;
	}

	CloseHandle(hFile);
}

void CLevel_FinalBoss::Create_Rain(_float fTimeDelta)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	fRainTime += fTimeDelta;
	if (fRainTime > 3.f)
	{
		fRainTime = 0.f;
		for (int i = 0; i < 30; ++i)
		{
			_float iSour = _float(rand() % 60);
			_float iTemp = _float(rand() % 60);

			_float3 vPos = { 0.f,0.f,0.f };
			tInfo.vPos.x = vPos.x + iSour;
			tInfo.vPos.y = vPos.y;
			tInfo.vPos.z = vPos.z + iTemp;

			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Rain"), LEVEL_FINALBOSS, TEXT("Layer_Effect"), &tInfo);

		}
	}
	Safe_Release(pGameInstance);
}

CLevel_FinalBoss * CLevel_FinalBoss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_FinalBoss*	pInstance = new CLevel_FinalBoss(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_FinalBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_FinalBoss::Free()
{
	__super::Free();
}