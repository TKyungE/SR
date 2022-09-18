#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Component_Manager.h"
#include "Picking.h"
#include "KeyMgr.h"
#include "Frustum.h"
#include "CollisionMgr.h"
#include "QuestManager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, _uint iNumObject, const GRAPHIC_DESC& GraphicDesc, LPDIRECT3DDEVICE9* ppOut);
	void Tick_Engine(_float fTimeDelta);

	void Clear(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	void		Render_Begin(void);
	void		Render_End(HWND hWnd = 0);
	LPD3DXSPRITE	Get_Sprite(void);
	LPD3DXFONT		Get_Font(void);
	LPD3DXFONT		Get_Font2(void);
public: /* For.Input_Device */
	_char Get_DIKState(_uchar eKeyID);
	_char Get_DIMKeyState(DIMK eMouseKeyID);
	_long Get_DIMMoveState(DIMM eMouseMoveID);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_float Get_TimeDelta(const _tchar* pTimerTag);
	void Update(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	//CGameObject* Find_Object(const _tchar* pLayerTag, _uint iIndex);
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public:	/* for. Frustum*/
	_bool	IsInFrustum(_float3 vPos, _float3 vScale);
	
public: /* For.Collision_Manager */
	HRESULT Add_ColiisionGroup(_uint iCollisionGroup, class CGameObject* pGameObject);
	_bool Collision(CGameObject * pGameObject, const _tchar* szDestColliderName, _uint iCollisionGroup, const _tchar* szSourColliderName, CGameObject** pTarget);
	_float3 Get_Collision(void);

public: /* For. Quest_Manager */
	HRESULT Add_Quest(class CQuest* pQuest, _int* pOut);
	HRESULT Release_Quest(_int iIndex);

public:
	static void Release_Engine();

private:
	CGraphic_Device*				m_pGraphic_Device = nullptr;
	CInput_Device*					m_pInput_Device = nullptr;
	CLevel_Manager*					m_pLevel_Manager = nullptr;
	CObject_Manager*				m_pObject_Manager = nullptr;
	CTimer_Manager*					m_pTimer_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CPicking*						m_pPicking = nullptr;
	CKeyMgr*						m_pKeyMgr = nullptr;
	CFrustum*						m_pFrustum = nullptr;
	CCollisionMgr*					m_pCollision_Manager = nullptr;
	CQuestManager*					m_pQuest_Manager = nullptr;

public:
	virtual void Free() override;
};

END