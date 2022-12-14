#include "stdafx.h"
#include "..\Public\Town.h"
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

bool g_bCollider = false;
bool g_bTalk = false;

CTown::CTown(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel(pGraphic_Device)
{
	ZeroMemory(&Info, sizeof(CGameObject::INFO));
}

HRESULT CTown::Initialize()
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

	return S_OK;
}

void CTown::Tick(_float fTimeDelta)
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

void CTown::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, TEXT("타운레벨입니다."));

	Open_Level();
}

HRESULT CTown::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO info;
	ZeroMemory(&info, sizeof(CGameObject::INFO));
	info.pstrPath = TEXT("../../Data/Terrain/TownTerrain.dat");
	info.iLevelIndex = LEVEL_TOWN;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_TOWN, pLayerTag, &info)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), LEVEL_TOWN, pLayerTag,&info)))
		return E_FAIL;
	
	for (auto& iter : m_vecTree)
	{
		CBackGroundTree::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CBackGroundTree::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;
		
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGroundTree"), LEVEL_TOWN, pLayerTag, &indexpos)))
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

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House"), LEVEL_TOWN, pLayerTag, &indexpos)))
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


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_House2"), LEVEL_TOWN, pLayerTag, &indexpos)))
			return E_FAIL;
	}


	for (auto& iter : m_vecIndex)
	{
		CBackGroundRect::INDEXPOS indexpos;
		ZeroMemory(&indexpos, sizeof(CBackGroundRect::INDEXPOS));
		indexpos.iIndex = iter.iIndex;
		indexpos.vScale = iter.vScale;
		indexpos.vPos = iter.BackGroundPos;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGroundRect"), LEVEL_TOWN, pLayerTag, &indexpos)))
			return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Transparent_Wall"), LEVEL_TOWN, pLayerTag, nullptr)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CTown::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo")) != nullptr)
	{
		CGameObject::INFO tInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Get_Info();
		memcpy(&Info, &tInfo, sizeof(CGameObject::INFO));
		Info.iLevelIndex = LEVEL_TOWN;
		Info.vPos = m_vBackPos;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_TOWN, pLayerTag, &Info)))
			return E_FAIL;
	}
	else
	{
		CGameObject::INFO tInfo;
		Info.vPos = m_vPlayerPos;
		Info.iLevelIndex = LEVEL_TOWN;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_TOWN, pLayerTag, &Info)))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PlayerInfo"), LEVEL_STATIC, TEXT("Layer_PlayerInfo"), &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}




HRESULT CTown::Ready_Layer_Camera(const _tchar * pLayerTag)
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

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TOWN, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTown::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.iLevelIndex = LEVEL_TOWN;
	tInfo.bHit = false;
	tInfo.bDead = false;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ExpBlank"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ExpLogo"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_QuickSlot"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Inventory"), LEVEL_TOWN, TEXT("Layer_Inventory"), &tInfo)))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_LogRect"), LEVEL_TOWN, TEXT("Layer_Log"), &tInfo)))
		return E_FAIL;*/

	

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_IconBar"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_InventoryIcon"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EquipIcon"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkillIcon"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_StatsIcon"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SpaceUI"), LEVEL_TOWN, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CTown::Ready_Layer_NPC(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// Index 0 = Ang  , Index 1,2,3 = Default, Index 4 = Engineer, Index 5 = Village_Chief, Index 6 = Village_Quest1, Index 7 = Village_Quest2; 

	CGameObject::INFO tInfo;
	tInfo.iLevelIndex = LEVEL_TOWN;
	tInfo.iHp = 1;
	for (auto& iter : m_vecNPC)
	{
		if (iter.iIndex == 0)
		{

		}
		else if (iter.iIndex > 0 && iter.iIndex <= 3)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;						// 인덱스 인데 인포구조체에 인덱스 없어서 MP로 대체 함
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Default_NPC"), LEVEL_TOWN, pLayerTag, &tInfo)))
				return E_FAIL;
		}
		else if (iter.iIndex == 4)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Engineer"), LEVEL_TOWN, pLayerTag, &tInfo)))
				return E_FAIL;
		}
		else if (iter.iIndex == 5)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Village_Chief"), LEVEL_TOWN, pLayerTag, &tInfo)))
				return E_FAIL;
		}
		else if (iter.iIndex == 6)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Village_Quest1"), LEVEL_TOWN, pLayerTag, &tInfo)))
				return E_FAIL;
		}
		else if (iter.iIndex == 7)
		{
			tInfo.vPos = iter.BackGroundPos;
			tInfo.iMp = iter.iIndex;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Village_Quest2"), LEVEL_TOWN, pLayerTag, &tInfo)))
				return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CTown::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	for (auto& iter : m_vecPortal)
	{
		CGameObject::INFO tInfo;
		tInfo.iLevelIndex = LEVEL_TOWN;
		tInfo.vPos = iter.BackGroundPos;
		tInfo.vScale = iter.vScale;
		tInfo.iNextLevel = LEVEL_GAMEPLAY;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TOWN, pLayerTag, &tInfo)))
			return E_FAIL;
		
	}
	
	Safe_Release(pGameInstance);
	return S_OK;
}

void CTown::Open_Level(void)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (nullptr != pGameInstance->Find_Layer(LEVEL_TOWN, TEXT("Layer_Portal")))
	{
		for (auto& iter : pGameInstance->Find_Layer(LEVEL_TOWN, TEXT("Layer_Portal"))->Get_Objects())
		{
			if (dynamic_cast<CPortal*>(iter)->Get_Level())
			{
				pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_PlayerInfo"))->Get_Objects().front()->Set_Info(pGameInstance->Find_Layer(LEVEL_TOWN, TEXT("Layer_Player"))->Get_Objects().front()->Get_Info());
				LEVEL eLevel = (LEVEL)iter->Get_Info().iNextLevel;
				if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eLevel))))
					return;
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CTown::LoadData()
{
	HANDLE hFile = CreateFile(TEXT("../../Data/TownPos.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte = 0;
	
	
	_float3 vPos1,vPos2;
	_uint iMSize, iIndexSize, iTreeSize, iHouseSize, iHouse2Size, iPortalSize, iNPCSize;
	_tchar str1[MAX_PATH];
	_tchar str2[MAX_PATH];
	_tchar str3[MAX_PATH];
	_tchar str4[MAX_PATH];
	_tchar str5[MAX_PATH];
	_tchar str6[MAX_PATH];
	_tchar str7[MAX_PATH];

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



	iMSize = stoi(str1);
	iIndexSize = stoi(str2);
	iTreeSize = stoi(str3);
	iHouseSize = stoi(str4);
	iHouse2Size = stoi(str5);
	iPortalSize = stoi(str6);
	iNPCSize = stoi(str7);



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

		if (0 == dwByte)
			break;
	}

	CloseHandle(hFile);
}
CTown * CTown::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTown*	pInstance = new CTown(pGraphic_Device);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CTown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTown::Free()
{
	__super::Free();
}
