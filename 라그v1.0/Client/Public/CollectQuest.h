#pragma once

#include "Client_Defines.h"
#include "ClientQuest.h"

BEGIN(Client)

class CCollectQuest final : public CClientQuest
{
public:
	typedef struct tagQuestInfo_Derived
	{
		QINFO tQInfo;
	}QINFO_DERIVED;
private:
	CCollectQuest(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCollectQuest(const CCollectQuest& rhs);
	virtual ~CCollectQuest() = default;

public:
	QINFO_DERIVED Get_QInfoDerived(void) { return m_tQInfo; }

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(void) override;
	virtual QINFO Get_QInfo(void) override { return m_tQInfo.tQInfo; }

private:
	QINFO_DERIVED m_tQInfo;

public:
	static CCollectQuest* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CCollectQuest* Clone(void* pArg = nullptr) override;
	virtual void Free(void) override;
};

END