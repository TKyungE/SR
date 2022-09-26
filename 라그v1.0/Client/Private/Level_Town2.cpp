#include "stdafx.h"
#include "..\Public\Level_Town2.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "SoundMgr.h"
#include "BackGroundRect.h" 
#include "Camera_Dynamic.h"
#include "KeyMgr.h"
#include "House.h"
#include "House2.h"
#include "BackGroundTree.h"
#include "Layer.h"
#include "Portal.h"
#include "Transparent_Wall.h"
#include "House3.h"
#include "House6.h"

//bool g_bCollider = false;
//bool g_bTalk = false;
//bool g_bCut = false;

CLevel_Town2::CLevel_Town2(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
	ZeroMemory(&Info, sizeof(CGameObject::INFO));
}

HRESULT CLevel_Town2::Initialize()
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

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Portal(TEXT("Layer_Portal"))))
		return E_FAIL;

	CSoundMgr::Get_Instance()->BGM_Stop();
	CSoundMgr::Get_Instance()->PlayBGM(L"TOWN2.wav", fSOUND+0.1f);

	return S_OK;
}

void CLevel_Town2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	/*CGameObject* Dest;
	CGameObject* Sour;*/

	/*if (CCollisionMgr::Collision_Sphere(pGameInstance->Find_Layer(LEVEL_TOWN, TEXT("Layer_Player"))->Get_Objects(), pGameInstance->Find_Layer(LEVEL_TOWN, TEXT("Layer_Portal"))->Get_Objects(), &Dest, &Sour))
	{
		m_bNextLevel = true;
		pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Set_Info(Dest->Get_Info());
	}*/
	if (GetKeyState('Y') < 0)
	{
		if (!g_bCollider)
			g_bCollider = true;
	}
	if (GetKeyState('U') < 0)
	{
		if (g_bCollider)
		{
			g_bCollider = false;
		}
	}

	Safe_Release(pGameInstance);
}

void CLevel_Town2::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("타운레벨입니다."));

	Open_Level();
}

HRESULT CLevel_Town2::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO info;
	ZeroMemory(&info, sizeof(CGameObject::INFO));
	info.pstrPath = TEXT("../../Data/Terrain/Town2.dat");
	info.iLevelIndex = LEVEL_TOWN2;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_TOWN2, pLayerTag, &info)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_TOWN2, pLayerTag, &info)))
		return E_FAIL;

	for (auto& iter : m_vecTree)
	{
		CBackGroundTree::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CBackGroundTree::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;
		indexpos.iLevelIndex = LEVEL_TOWN2;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGroundTree"), LEVEL_TOWN2, pLayerTag, &indexpos)))
			return E_FAIL;
	}

	for (auto& iter : m_vecHouse)
	{
		CHouse::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CHouse::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;
		indexpos.iTrun = iter.iTrun;



		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_TOWN2, pLayerTag, &indexpos)))
			return E_FAIL;
	}

	for (auto& iter : m_vecHouse2)
	{
		CHouse2::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CHouse2::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;
		indexpos.iTrun = iter.iTrun;


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House2"), LEVEL_TOWN2, pLayerTag, &indexpos)))
			return E_FAIL;
	}


	for (auto& iter : m_vecIndex)
	{
		CBackGroundRect::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CBackGroundRect::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGroundRect"), LEVEL_TOWN2, pLayerTag, &indexpos)))
			return E_FAIL;
	}


	for (auto& iter : m_vecWall)
	{
		Transparent_Wall::WALL Wall;
		ZeroMemory(&Wall, sizeof(Transparent_Wall::WALL));

		Wall.vPos = iter.BackGroundPos;
		Wall.vScale = iter.vScale;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Transparent_Wall"), LEVEL_TOWN2, pLayerTag, &Wall)))
			return E_FAIL;
	}

	CHouse3::INDEXPOS index;
	ZeroMemory(&index, sizeof(CHouse3::INDEXPOS));
	index.vPos = _float3(6.f, 0.f, 17.5f);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House3"), LEVEL_TOWN2, pLayerTag, &index)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House4"), LEVEL_TOWN2, pLayerTag, nullptr)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House5"), LEVEL_TOWN2, pLayerTag, nullptr)))
		return E_FAIL;


	CHouse6::INDEXPOS index2;
	ZeroMemory(&index2, sizeof(CHouse6::INDEXPOS));
	index2.vPos = _float3(13.5f, 0.f, 6.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House6"), LEVEL_TOWN2, pLayerTag, &index2)))
		return E_FAIL;

	CHouse6::INDEXPOS index3;
	ZeroMemory(&index3, sizeof(CHouse6::INDEXPOS));
	index3.vPos = _float3(13.f, 0.f, 17.f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House6"), LEVEL_TOWN2, pLayerTag, &index3)))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House7"), LEVEL_TOWN2, pLayerTag, nullptr)))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ItemBox"), LEVEL_TOWN2, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town2::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo")) != nullptr)
	{
		CGameObject::INFO tInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Get_Info();
		memcpy(&Info, &tInfo, sizeof(CGameObject::INFO));
		
		if (Info.iLevelIndex == LEVEL_TOWN)
		{
			Info.vPos = m_vPlayerPos;
		}
		else
			Info.vPos = m_vBackPos;

		Info.iLevelIndex = LEVEL_TOWN2;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_TOWN2, pLayerTag, &Info)))
			return E_FAIL;
	}
	else
	{
		CGameObject::INFO tInfo;
		Info.vPos = m_vPlayerPos;
		Info.iLevelIndex = LEVEL_TOWN2;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_TOWN2, pLayerTag, &Info)))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerInfo"), LEVEL_STATIC, TEXT("Layer_PlayerInfo"), &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}




HRESULT CLevel_Town2::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TOWN2, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town2::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.iLevelIndex = LEVEL_TOWN2;
	tInfo.bHit = false;
	tInfo.bDead = false;
	tInfo.pTarget = Info.pTarget;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_TOWN2, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SpaceUI"), LEVEL_TOWN2, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LetterBox"), LEVEL_TOWN2, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_QuestUI"), LEVEL_TOWN2, pLayerTag, &tInfo)))
		return E_FAIL;

	tInfo.iMp = 1;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Fade"), LEVEL_TOWN2, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Town2::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// Index 0 = Ang  , Index 1,2,3 = Default, Index 4 = Engineer, Index 5 = Village_Chief, Index 6 = Village_Quest1, Index 7 = Village_Quest2; 

	CGameObject::INFO tInfo;
	tInfo.iLevelIndex = LEVEL_TOWN2;
	tInfo.iHp = 1;
	for (auto& iter : m_vecNPC)
	{
		if (iter.iIndex == 0)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Village_Chief"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 8)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;						// 인덱스 인데 인포구조체에 인덱스 없어서 MP로 대체 함
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Default_NPC"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 9)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;						// 인덱스 인데 인포구조체에 인덱스 없어서 MP로 대체 함
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Default_NPC"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 10)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShopNPC"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 11)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			tInfo.pTarget = Info.pTarget;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Town_Quest1"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 12)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			tInfo.pTarget = Info.pTarget;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Town_Quest2"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}

		else if (iter.iIndex == 13)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			tInfo.pTarget = Info.pTarget;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Town_Quest3"), LEVEL_TOWN2, pLayerTag, &tInfo)))
				return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Town2::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	auto iter = m_vecPortal.begin();

	_uint iCount = 0;

	for (; iter != m_vecPortal.end(); ++iter)
	{
		if (iCount == 1)
		{
			iCount = 0;
			break;
		}
		CGameObject::INFO tInfo;
		tInfo.iLevelIndex = LEVEL_TOWN2;
		tInfo.vPos = iter->BackGroundPos;
		tInfo.vScale = iter->vScale;
		tInfo.iNextLevel = LEVEL_GAMEPLAY;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TOWN2, pLayerTag, &tInfo)))
			return E_FAIL;

		++iCount;
	}

	for (; iter != m_vecPortal.end(); ++iter)
	{
		if (iCount == 1)
		{
			iCount = 0;
			break;
		}

		CGameObject::INFO tInfo;
		tInfo.iLevelIndex = LEVEL_TOWN2;
		tInfo.vPos = iter->BackGroundPos;
		tInfo.vScale = iter->vScale;
		tInfo.iNextLevel = LEVEL_DESERT1;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TOWN2, pLayerTag, &tInfo)))
			return E_FAIL;

		++iCount;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CLevel_Town2::Open_Level(void)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Find_Layer(LEVEL_TOWN2, TEXT("Layer_Portal")))
	{
		for (auto& iter : pGameInstance->Find_Layer(LEVEL_TOWN2, TEXT("Layer_Portal"))->Get_Objects())
		{
			if (dynamic_cast<CPortal*>(iter)->Get_Level())
			{
				if (!m_bFade)
				{
					CGameObject::INFO	tInfo;
					ZeroMemory(&tInfo, sizeof(CGameObject::INFO));
					tInfo.iLevelIndex = LEVEL_TOWN2;
					tInfo.bHit = false;
					tInfo.bDead = false;
					tInfo.pTarget = Info.pTarget;
					tInfo.iMp = 0;

					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Fade"), LEVEL_TOWN2, TEXT("Layer_Fade"), &tInfo)))
						return;
					m_bFade = true;
				}

				if (pGameInstance->Find_Layer(LEVEL_TOWN2, TEXT("Layer_Fade"))->Get_Objects().front()->Get_Info().iMaxMp == 1)
				{
					pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Set_Info(pGameInstance->Find_Layer(LEVEL_TOWN2, TEXT("Layer_Player"))->Get_Objects().front()->Get_Info());
					LEVEL eLevel = (LEVEL)iter->Get_Info().iNextLevel;
					if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevel))))
						return;
				}
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CLevel_Town2::LoadData()
{
	HANDLE hFile = CreateFile(TEXT("../../Data/Town2Pos.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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
			HousePos.iTrun = turn;
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
			HousePos.iTrun = turn;
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
			PortalPos.iTrun = turn;
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
CLevel_Town2 * CLevel_Town2::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLevel_Town2*	pInstance = new CLevel_Town2(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Town2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Town2::Free()
{
	__super::Free();
}
