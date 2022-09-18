#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pKeyMgr(CKeyMgr::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pCollision_Manager(CCollisionMgr::Get_Instance())
	, m_pQuest_Manager(CQuestManager::Get_Instance())
{
	Safe_AddRef(m_pQuest_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pKeyMgr);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, _uint iNumObject, const GRAPHIC_DESC& GraphicDesc, LPDIRECT3DDEVICE9* ppOut)
{
	if (nullptr == m_pGraphic_Device || nullptr == m_pObject_Manager)
		return E_FAIL;

	/* 그래픽 디바이스를 초기화한다. */
	if (FAILED(m_pGraphic_Device->InitDevice(GraphicDesc, ppOut)))
		return E_FAIL;

	/* 입력 디바이스를 초기화한다. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, *ppOut)))
		return E_FAIL;

	/* 사운드 디바이스를 초기화한다. */


	/* 컨테이너의 공간을 확보해둔다. */
	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize(*ppOut)))
		return E_FAIL;

	if (FAILED(m_pCollision_Manager->Reserve_Container(iNumObject)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || nullptr == m_pObject_Manager)
		return;

	m_pInput_Device->Update();

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);

	m_pFrustum->Tick();
	m_pPicking->Tick();
	m_pQuest_Manager->Tick();

	m_pLevel_Manager->Late_Tick(fTimeDelta);
	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pCollision_Manager->Release_Objects();
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || nullptr == m_pComponent_Manager)
		return;

	m_pComponent_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
}

void CGameInstance::Render_Begin(void)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_Begin();
}

void CGameInstance::Render_End(HWND hWnd)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Render_End(hWnd);
}

LPD3DXSPRITE CGameInstance::Get_Sprite(void)
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_Sprite();
}

LPD3DXFONT CGameInstance::Get_Font(void)
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_Font();
}
LPD3DXFONT CGameInstance::Get_Font2(void)
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_Font2();
}
_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);	
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);	
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

HRESULT CGameInstance::Open_Level(unsigned int iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

CLayer * CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

//CGameObject * CGameInstance::Find_Object(const _tchar * pLayerTag, _uint iIndex)
//{
//	if (nullptr == m_pObject_Manager)
//		return nullptr;
//
//	return m_pObject_Manager->Find_Object(pLayerTag, iIndex);
//}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}


_bool CGameInstance::IsInFrustum(_float3 vPos, _float3 vScale)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->IsinFrustum(vPos, vScale);
}
HRESULT CGameInstance::Add_ColiisionGroup(_uint iCollisionGroup, CGameObject * pGameObject)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;
	
	return m_pCollision_Manager->Add_ColiisionGroup(iCollisionGroup, pGameObject);
}

_bool CGameInstance::Collision(CGameObject * pGameObject, const _tchar* szDestColliderName, _uint iCollisionGroup, const _tchar* szSourColliderName, CGameObject** pTarget)
{
	if (nullptr == m_pCollision_Manager)
		return false;
	
	return m_pCollision_Manager->Collision(pGameObject, szDestColliderName, iCollisionGroup, szSourColliderName, pTarget);
}

_float3 CGameInstance::Get_Collision(void)
{
	if (nullptr == m_pCollision_Manager)
		return _float3();

	return m_pCollision_Manager->Get_Collision();
}

HRESULT CGameInstance::Add_Quest(CQuest * pQuest, _int * pOut)
{
	if (nullptr == m_pQuest_Manager)
		return E_FAIL;
	
	return m_pQuest_Manager->Add_Quest(pQuest, pOut);
}

HRESULT CGameInstance::Release_Quest(_int iIndex)
{
	if (nullptr == m_pQuest_Manager)
		return E_FAIL;
	
	return m_pQuest_Manager->Release_Quest(iIndex);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CCollisionMgr::Get_Instance()->Destroy_Instance();

	CQuestManager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CFrustum::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();
	
	CKeyMgr::Get_Instance()->Destroy_Instance();

	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pQuest_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pKeyMgr);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pGraphic_Device);
}
