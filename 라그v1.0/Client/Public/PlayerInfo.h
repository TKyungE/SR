#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerInfo final : public CGameObject
{
private:
	CPlayerInfo(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CPlayerInfo(const CPlayerInfo& rhs);
	virtual ~CPlayerInfo() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;
public:
	static CPlayerInfo* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;

};

END