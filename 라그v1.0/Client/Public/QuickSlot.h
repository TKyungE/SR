#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "StatInfo.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_WingRect;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQuickSlot final : public CGameObject
{
private:
	CQuickSlot(LPDIRECT3DDEVICE9 pGraphic_Device);
	CQuickSlot(const CQuickSlot& rhs);
	virtual ~CQuickSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pItemTexture = nullptr;
	CTexture*				m_pMouseTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pMouseTransformCom = nullptr;
	CVIBuffer_WingRect*			m_pVIBufferCom = nullptr;
	CTransform*				m_pSlotTrans[10];
	CVIBuffer_Rect*			m_pSlotBuffer[10];
	CVIBuffer_Rect*			m_pMouseBuffer;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	RECT					m_rcSlot[10];
	RECT					m_rcCount[10];
	CGameObject*			m_StatInfo;
	vector<CStatInfo::ITEM>			m_pvecItem;
	_bool					m_bMousePick = false;
	_float					m_ThunderCool = 0.f;
	_float					m_TornadoCool = 0.f;
	_float					m_FireBall = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void	Set_Slot();
	void	Check_Slot(_float fTimeDelta);
	void	Use_Slot();
	void Move_Frame(_float fTimeDelta);
public:
	static CQuickSlot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END