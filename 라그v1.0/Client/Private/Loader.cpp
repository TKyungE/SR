#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Sky.h"
#include "UI.h"
#include "ThunderCloud.h"
#include "ThunderSword.h"
#include "Tornado.h"
#include "Hit.h"
#include "UseSkill.h"
#include "HpBar.h"
#include "MpBar.h"
#include "IconBar.h"
#include "InventoryIcon.h"
#include "Inventory.h"
#include "XBox.h"
#include "LevelUp.h"
#include "DmgFont.h"
#include "FireSpear.h"
#include "Fire.h"
#include "Meteor.h"
#include "MeteorEff.h"
#include "Rain.h"
#include"EquipIcon.h"
#include"SkillIcon.h"
#include"StatsIcon.h"
#include"QuickSlot.h"
#include"SkillWnd.h"
#include"StatsWnd.h"
#include"PlusBox.h"
#include"WorldHpBar.h"
#include"PlayerMpBar.h"
#include "FireDragon.h"
#include "Wind.h"
#include "Shadow.h"
#include"ExpBar.h"
#include"ExpLogo.h"
#include"ExpBlank.h"
#include "BackGroundRect.h"
#include "Poring.h"
#include "BackGroundTree.h"
#include "House.h"
#include "Stone.h"
#include "Portal.h"
#include "PlayerInfo.h"
#include "Wing.h"
#include "House2.h"
#include"SkillSlot.h"
#include"HpPotion.h"
#include "Default_NPC.h"
#include "Village_Chief.h"
#include "Village_Quest1.h"
#include "Village_Quest2.h"
#include "Engineer.h"
#include "PoisonArrow.h"
#include "Alligator.h"
#include "TerrainRect.h"
#include "ElderWilow.h"
#include "Bigfoot.h"
#include "DefaultAttack.h"
#include "Help.h"
#include "Angry.h"
#include "Byorgue.h"
#include "Bloodymurderer.h"
#include "Dandelion.h"
#include "Sword.h"
#include "SwordSlice.h"
#include "DandelionAttack.h"
#include "Maiden.h"
#include"InventorySlot.h"
#include"LogRect.h"
#include "HuntQuest1.h"
#include "Drain.h"
#include "PowerTotem.h"
#include "BossSkillTarget.h"
#include "BlueFire.h"
#include "AuraBlue.h"
#include "Space.h"
#include "FireBall.h"
#include "Transparent_Wall.h"
#include "LetterBox.h"
#include "Atros.h"
#include "Baphomet.h"
#include "Minorous.h"
#include "Skeleton.h"
#include "Wraith.h"
#include "Zombie.h"
#include "House3.h"

CLoader::CLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_TOWN:
		pLoader->Loading_ForTownLevel();
		break;
	case LEVEL_CHOBOFIELD:
		pLoader->Loading_ForChoboLevel();
		break;
	case LEVEL_MIDBOSS:
		pLoader->Loading_ForMidBoss();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
		
	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	
	Loading_Prototype();
	Loading_Static(LEVEL_STATIC);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{


	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForTownLevel()
{


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForChoboLevel()
{
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForMidBoss()
{

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Static(LEVEL Level)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);
	//NPC
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_NPC"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/NPC/NPC%d.png"),8))))
		return E_FAIL;
	//Pet 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Poring_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Poring/IDLE_Back/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Poring_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Poring/IDLE_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Poring_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Poring/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Poring_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Poring/Move_Back/%d.png"), 8))))
		return E_FAIL;

	//UI 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/INTERFACE/Info%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Temp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/QuickSlot/QuickSlot%d.png"), 9))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_StatsWnd"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/StatWnd/StatWnd%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_PlusBox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/StatWnd/StatWnd%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_XBox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/XBox/XBox%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Item"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/StatWnd/StatWnd%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_HpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/HpBar/HpBar%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ExpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/ExpBar/ExpBar%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ExpLogo"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/ExpBar/ExpLogo%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ExpBlank"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/ExpBar/ExpBar%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_TePrototype_Component_Texture_QuickSlotxture_PlayerMpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/HpBar/HpBar%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_WorldHpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/HpBar/HpBar%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_MpBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/HpBar/HpBar%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Inventory"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/Inventory/Inventory%d.jpg"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_QuickSlot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/QuickSlot/QuickSlot%d.jpg"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_SkillWnd"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/SkillWnd/SkillWnd%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_IconBar"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/IconBar%d.jpg"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_QuestUI"), 
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/Quest/QuestUI0%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_SpaceUI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/Space/Space0%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_LetterBox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/LetterBox/Black0%d.png"), 1))))
		return E_FAIL;

	//Icon 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_EquipIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/UIWnd/UIWnd%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_InventoryIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/UIWnd/UIWnd%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_SkillIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/UIWnd/UIWnd%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_StatsIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/UI/UIWnd/UIWnd%d.png"), 8))))
		return E_FAIL;

	// Terrain 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.tga"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_TerrainRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MAP/Map/Map%d.png"), 114))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Portal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/Portal/Portal%d.png"), 24))))
		return E_FAIL;
	// sky 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	//Effect 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Help"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Help/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Angry"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Angry/%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wind"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Wind/%d.bmp"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Hit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Skill/Hit/Effect%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ThunderEff"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/ThunderEff/%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_DmgFont"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/DmgFont/%d.png"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_UseSkill"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/UseSkill/Effect%d.bmp"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_LevelUp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Level/ItemLevelUp_%d.bmp"), 22))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_MeteorEff"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/MeteorEff/Effect%d.bmp"), 20))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Rain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/snow/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Drain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Drain/%d.bmp"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_PowerTotem"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/PowerTotem/%d.bmp"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_AuraBlue"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/AURA/AuraBlue/%d.bmp"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BossSkillTarget"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/BossSkillTarget/%d.bmp"), 1))))
		return E_FAIL;

	//Skill 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ThunderCloud"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Skill/Thunder/ThunderCloud/Cloud%d.bmp"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ThunderSword"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Skill/Thunder/ThunderSword/%d.bmp"), 17))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Tornado"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Skill/Tornado/Effect%d.bmp"), 25))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireSpear"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/FireSpear/%d.bmp"), 11))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Fire"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Fire/%d.bmp"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Meteor"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/Meteor/%d.png"), 12))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_PoisonArrow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/PoisonArrow/Effect%d.bmp"), 11))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Sword"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Sword/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_SwordSlice"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/SwordSlice/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_DandelionAttack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/DandelionAttack/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BlueFire"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Boss_Skill/BlueFire/%d.bmp"), 10))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireBall"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Skill/FireBall/%d.bmp"), 1))))
		return E_FAIL;
	//Player 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/IDLE_Back/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/IDLE_Front/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Move_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Move_Left_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Move_Left_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Move_Left_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Move_Left_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skill_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Skill_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skill_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Skill_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_IDLE_Back/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_IDLE_Front/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_Move_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_Move_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_Move_Left_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_Move_Left_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Ride_Move_Left_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Ride_Move_Left_Back/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Shadow"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Effect/Shadow/%d.bmp"), 1))))
		return E_FAIL;

	//Wing
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wing"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Player/Wing/%d.png"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_House_Body"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/House/Body/%d.dds"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_House_Head"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/House/Head/%d.png"), 1))))
		return E_FAIL;
	
	//Boss
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireDragon_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/FireDragon/IDLE_Front/%d.png"), 7))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireDragon_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/FireDragon/IDLE_Back/%d.png"), 7))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireDragon_Dead"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/FireDragon/Dead/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_FireDragon_Attack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/FireDragon/Attack/%d.png"), 9))))
		return E_FAIL;
	//Maiden
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/IDLE_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/IDLE_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Hit_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Hit_Front/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Hit_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Hit_Back/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Attack_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Attack_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Dead_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Maiden_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/BOSS/Maiden/Dead_Back/%d.png"), 5))))
		return E_FAIL;
	//Monster
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Monster"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Test/Alpha/agav00%d.png"), 10))))
		return E_FAIL;
	//Alligator
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/IDLE_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/IDLE_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Move_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Move_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Attack_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Attack_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Dead_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Alligator_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Alligator/Dead_Back/%d.png"), 4))))
		return E_FAIL;
	//ElderWilow
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/IDLE_Front/%d.png"), 5))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_IDLE_Back"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/IDLE_Back/%d.png"), 5))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Move_Front"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Move_Front/%d.png"), 4))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Move_Back"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Move_Back/%d.png"), 4))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Attack_Front"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Attack_Front/%d.png"), 5))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Attack_Back"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Attack_Back/%d.png"), 5))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Dead_Front"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Dead_Front/%d.png"), 3))))
		return E_FAIL;																								
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_ElderWilow_Dead_Back"),		
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/ElderWilow/Dead_Back/%d.png"), 3))))
		return E_FAIL;
	//Bigfoot
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/IDLE_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/IDLE_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Move_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Move_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Attack_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Attack_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Attack2_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Attack2_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Attack2_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Attack2_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Dead_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bigfoot_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bigfoot/Dead_Back/%d.png"), 4))))
		return E_FAIL;
	//Byorgue
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/IDLE_Front/%d.png"), 9))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/IDLE_Back/%d.png"), 9))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Move_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Move_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Attack_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Attack_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Dead_Front/%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Byorgue_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Byorgue/Dead_Back/%d.png"), 3))))
		return E_FAIL;
	//Bloodymurderer
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/IDLE_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/IDLE_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Move_Front/%d.png"), 9))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Move_Back/%d.png"), 9))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Attack_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Attack_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Dead_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Bloodymurderer_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Bloodymurderer/Dead_Back/%d.png"), 4))))
		return E_FAIL;
	//Dandelion
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/IDLE_Front/%d.png"), 7))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/IDLE_Back/%d.png"), 7))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Move_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Attack_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Attack_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Dead_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Dandelion_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Dandelion/Dead_Back/%d.png"), 5))))
		return E_FAIL;
	//Atros
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/IDLE_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/IDLE_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Move_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Attack_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Attack_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Dead_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Atros_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Atros/Dead_Back/%d.png"), 5))))
		return E_FAIL;
	//Baphomet
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/IDLE_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/IDLE_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Move_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Move_Back/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Attack_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Attack_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Dead_Front/%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Baphomet_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Baphomet/Dead_Back/%d.png"), 3))))
		return E_FAIL;
	//Minorous
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/IDLE_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/IDLE_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Move_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Attack_Front/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Attack_Back/%d.png"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Dead_Front/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Minorous_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Minorous/Dead_Back/%d.png"), 2))))
		return E_FAIL;
	//Skeleton
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/IDLE_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/IDLE_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/Attack_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/Attack_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/Dead_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Skeleton_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Skeleton/Dead_Back/%d.png"), 5))))
		return E_FAIL;
	//Wraith
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/IDLE_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/IDLE_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/Attack_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/Attack_Back/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/Dead_Front/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Wraith_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Wraith/Dead_Back/%d.png"), 2))))
		return E_FAIL;
	//Zombie
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_IDLE_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/IDLE_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_IDLE_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/IDLE_Back/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Move_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Move_Front/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Move_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Move_Back/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Attack_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Attack_Front/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Attack_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Attack_Back/%d.png"), 5))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Dead_Front"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Dead_Front/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Zombie_Dead_Back"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/OBJ/OBJ/MONSTER/Zombie/Dead_Back/%d.png"), 4))))
		return E_FAIL;
	//for GamePlay
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Tree"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Terrain/grass_%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_Stone"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/Stone/%d.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BackGroundTree"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/BackGround/Tree/%d.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BackGroundTreeRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/Tree/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BackGroundRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/etc/BackGround%d.png"), 13))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_BackGroundFly"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/etc/Fly/%d.png"), 12))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_HouseRect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/House/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_HouseCube"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/BackGround/House/%d.dds"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(Level, TEXT("Prototype_Component_Texture_House3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/BackGround/House/1/%d.png"), 6))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Prototype()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);
	//NPC
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Default_NPC"),
		CDefault_NPC::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Village_Chief"),
		CVillage_Chief::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Village_Quest1"),
		CVillage_Quest1::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Village_Quest2"),
		CVillage_Quest2::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Engineer"),
		CEngineer::Create(m_pGraphic_Device))))
		return E_FAIL;
	//UI 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StatsWnd"),
		CStatsWnd::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpPotion"),
		CHpPotion::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenSlot"),
		CInventorySlot::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillSlot"),
		CSkillSlot::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WorldHpBar"),
		CWorldHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerMpBar"),
		CPlayerMpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogRect"),
		CLogRect::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlusBox"),
		CPlusBox::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpBar"),
		CHpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpBar"),
		CExpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpLogo"),
		CExpLogo::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpBlank"),
		CExpBlank::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MpBar"),
		CMpBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IconBar"),
		CIconBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InventoryIcon"),
		CInventoryIcon::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EquipIcon"),
		CEquipIcon::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_XBox"),
		CXBox::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillIcon"),
		CSkillIcon::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StatsIcon"),
		CStatsIcon::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuickSlot"),
		CQuickSlot::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkillWnd"),
		CSkillWnd::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpaceUI"),
		CSpace::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LetterBox"),
		CLetterBox::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Player 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerInfo"),
		CPlayerInfo::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Pet 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Poring"),
		CPoring::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Wing 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wing"),
		CWing::Create(m_pGraphic_Device))))
		return E_FAIL;


	//Terrain , BackGround 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TerrainRect"),
		CTerrainRect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGroundTree"),
		CBackGroundTree::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGroundRect"),
		CBackGroundRect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Portal"),
		CPortal::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House2"),
		CHouse2::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Transparent_Wall"),
		Transparent_Wall::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House3"),
		CHouse3::Create(m_pGraphic_Device))))
		return E_FAIL;
	

	//sky 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	//camera 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pGraphic_Device))))
		return E_FAIL;

	//Skill 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderCloud"),
		CThunderCloud::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderSword"),
		CThunderSword::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tornado"),
		CTornado::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireSpear"),
		CFireSpear::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire"),
		CFire::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Meteor"),
		CMeteor::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PoisonArrow"),
		CPoisonArrow::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DefaultAttack"),
		CDefaultAttack::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SwordSlice"),
		CSwordSlice::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DandelionAttack"),
		CDandelionAttack::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BlueFire"),
		CBlueFire::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBall"),
		CFireBall::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Effect 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Angry"),
		CAngry::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Help"),
		CHelp::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wind"),
		CWind::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hit"),
		CHit::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DmgFont"),
		CDmgFont::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UseSkill"),
		CUseSkill::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LevelUp"),
		CLevelUp::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeteorEff"),
		CMeteorEff::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rain"),
		CRain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shadow"),
		CShadow::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Drain"),
		CDrain::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossSkillTarget"),
		CBossSkillTarget::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AuraBlue"),
		CAuraBlue::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_House"),
		CHouse::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stone"),
		CStone::Create(m_pGraphic_Device))))
		return E_FAIL;
	//Monster 객체
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ElderWilow"),
		CElderWilow::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Alligator"),
		CAlligator::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bigfoot"),
		CBigfoot::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Byorgue"),
		CByorgue::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bloodymurderer"),
		CBloodymurderer::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dandelion"),
		CDandelion::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PowerTotem"),
		CPowerTotem::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Atros"),
		CAtros::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Baphomet"),
		CBaphomet::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Minorous"),
		CMinorous::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wraith"),
		CWraith::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pGraphic_Device))))
		return E_FAIL;
	//BOSS
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireDragon"),
		CFireDragon::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Maiden"),
		CMaiden::Create(m_pGraphic_Device))))
		return E_FAIL;

	


	Safe_Release(pGameInstance);

	return S_OK;
}



CLoader * CLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGraphic_Device);
}
