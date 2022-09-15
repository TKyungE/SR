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
	enum EITEM{HPPOTION,MPPOTION,PETEGG,ROBE,STAFF,EITEM_END};
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
		EITEM eItemNum;
		_int iSlotNum;
		_int iCount;
	}ITEM;
private:
	STAT m_tStat;
	ITEM m_tItem[24];
public:
	ITEM Get_Item(_int iNum) { return m_tItem[iNum]; }
	STAT Get_Stat() { return m_tStat; }
	void Set_ItemNum(EITEM _ItemNum, _int iNum) { m_tItem[iNum].eItemNum = _ItemNum; }
	void Set_ItemCount(_int _ItemCount, _int iNum) { m_tItem[iNum].iCount += _ItemCount; }
	void Set_ItemSlot(_int _ItemSlot, _int iNum) { m_tItem[iNum].iSlotNum = _ItemSlot; }
	void Set_STR(_int _iSTR) { m_tStat.iSTR += _iSTR; }
	void Set_DEX(_int _iDEX) { m_tStat.iDEX += _iDEX; }
	void Set_INT(_int _iINT) { m_tStat.iINT += _iINT; }
	void Set_LUK(_int _iLUK) { m_tStat.iLUK += _iLUK; }
	void Set_StatsPoint(_int _Point) { m_tStat.iStatsPoint += _Point; }
};

END