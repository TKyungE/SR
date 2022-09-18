#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "StatInfo.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSkillSlot final : public CGameObject
{
private:
	CSkillSlot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkillSlot(const CSkillSlot& rhs);
	virtual ~CSkillSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pItemTexture = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pSlotTrans[7];
	CVIBuffer_Rect*			m_pSlotBuffer[7];
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	RECT					m_rcSlot[7];
	CGameObject*			m_StatInfo;
	vector<CStatInfo::ITEM>			m_pvecItem;
	_bool					m_bMousePick = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void	Set_Slot();
	void	Check_Slot();
public:
	static CSkillSlot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END