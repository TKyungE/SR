#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Client)

class CStatInfo final : public CGameObject
{
private:
	CStatInfo(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CStatInfo(const CStatInfo& rhs);
	virtual ~CStatInfo() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;
public:
	static CStatInfo* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
public:
	enum EITEM{HPPOTION,MPPOTION,GOLD,ENGINE,TIARA,BOBY,SHOES,ROBE,PANDANT,EARRING,BRACELET,RING,STAFF,ORB,RIDEEGG,PETEGG,WING,
		MON1,MON2, MON3, MON4, MON5, MON6, MON7, MON8, MON9, MON10, MON11, MON12,SKILL_THUNDER,SKILL_TORNADO,SKILL_FIREBALL,RIDE_ALPACA,SKILL_SKYFIRE,SKILL_SKYTHUNDER,EITEM_END};
	typedef struct tagStat
	{
		_int iSTR;
		_int iDEX;
		_int iINT;
		_int iLUK;
		_int iStatsPoint;
	}STAT;
	typedef struct tagItem
	{
		EITEM eItemNum;		//아이템 종류 확인 가능
		_int iSlotNum;
		_int iCount;		//아이템 갯수 확인 가능
	}ITEM;
private:
	STAT m_tStat;
	STAT m_tMaxStat;
	ITEM m_tItem[24];		//인벤토리 내의 아이템
	ITEM m_pQuickSlot[10];
	ITEM m_pSkyQuickSlot[2];
	ITEM m_SkillSlot[7];
	ITEM m_EquipSlot[10];
	_bool	m_bMousePick = false;
	_bool	m_bInven = false;
	_bool	m_bPoring = false;
	_bool	m_bRide = false;
	_bool	m_bShop = false;
	ITEM m_MouseItem;
	_int m_iEquipStr = 0;
	_int m_iEquipDex = 0;
	_int m_iEquipInt = 0;
	_int m_iEquipLuk = 0;
public:
	ITEM Get_SkyQuickSlot(_int iNum) { return m_pSkyQuickSlot[iNum]; }
	ITEM Get_EquipSlot(_int iNum) { return m_EquipSlot[iNum]; }
	ITEM Get_SkillSlot(_int iNum) { return m_SkillSlot[iNum]; }
	ITEM Get_QuickSlot(_int iNum) { return m_pQuickSlot[iNum]; }
	ITEM Get_Item(_int iNum) { return m_tItem[iNum]; }		//이걸로 인벤토리 내부 아이템을 확인해야 한다.
	STAT Get_Stat() { return m_tMaxStat; }
	_bool Get_MousePick() { return m_bMousePick; }
	_bool Get_InvenMouse() { return m_bInven; }
	ITEM Get_MouseItem() { return m_MouseItem; }
	void Set_EquipSlot(ITEM _Item, _int iNum) { m_EquipSlot[iNum] = _Item; }
	void Set_QuickSlot(ITEM _Item, _int iNum) { m_pQuickSlot[iNum] = _Item; }
	void Set_InvenItem(ITEM _Item, _int iNum) { m_tItem[iNum] = _Item; }
	void Set_MouseItem(ITEM _MouseItem) { m_MouseItem = _MouseItem; }
	void Set_MousePick(_bool _Pick) {m_bMousePick = _Pick;}
	void Set_InvenMouse(_bool _Pick) { m_bInven = _Pick; }
	void Set_ItemNum(EITEM _ItemNum, _int iNum) { m_tItem[iNum].eItemNum = _ItemNum; }
	void Set_ItemCount(_int _ItemCount, _int iNum) { m_tItem[iNum].iCount = _ItemCount; }
	void Set_UseItemCount(_int _ItemCount, _int iNum) { m_tItem[iNum].iCount += _ItemCount; }
	void Set_ItemSlot(_int _ItemSlot, _int iNum) { m_tItem[iNum].iSlotNum = _ItemSlot; }

	void Set_QuickItemNum(EITEM _ItemNum, _int iNum) { m_pQuickSlot[iNum].eItemNum = _ItemNum; }
	void Set_QuickItemCount(_int _ItemCount, _int iNum) { m_pQuickSlot[iNum].iCount = _ItemCount; }
	void Set_QuickUseItemCount(_int _ItemCount, _int iNum) { m_pQuickSlot[iNum].iCount += _ItemCount; }
	void Set_QuickItemSlot(_int _ItemSlot, _int iNum) { m_pQuickSlot[iNum].iSlotNum = _ItemSlot; }
	void Set_SkillSlot(ITEM _Item, _int iNum){ m_SkillSlot[iNum] = _Item; }

	void Set_SkyQuickItemCount(_int _ItemCount, _int iNum) { m_pSkyQuickSlot[iNum].iCount = _ItemCount; }
	void Set_SkyQuickUseItemCount(_int _ItemCount, _int iNum) { m_pSkyQuickSlot[iNum].iCount += _ItemCount; }

	void Set_STR(_int _iSTR) { m_tStat.iSTR += _iSTR; }
	void Set_DEX(_int _iDEX) { m_tStat.iDEX += _iDEX; }
	void Set_INT(_int _iINT) { m_tStat.iINT += _iINT; }
	void Set_LUK(_int _iLUK) { m_tStat.iLUK += _iLUK; }
	void Set_StatsPoint(_int _Point) { m_tMaxStat.iStatsPoint += _Point; }

	_bool Get_Poring() { return m_bPoring; }
	_bool Get_Ride() { return m_bRide; }
	void Set_Poring() { m_bPoring = true; }
	void Set_Ride() { m_bRide = true; }

	_bool Get_Shop() { return m_bShop; }
	void Set_Shop(_bool _Shop) { m_bShop = _Shop; }
private:
	void Check_Equip();
};

END