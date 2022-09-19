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

class CInven final : public CGameObject
{
private:
	CInven(LPDIRECT3DDEVICE9 pGraphic_Device);
	CInven(const CInven& rhs);
	virtual ~CInven() = default;

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
	CTransform*				m_pSlotTrans[24];
	CVIBuffer_Rect*			m_pSlotBuffer[24];
	CTexture*				m_pTextTexture = nullptr;
	CTransform*				m_pTextTrans = nullptr;
	CVIBuffer_Rect*			m_pTextBuffer = nullptr;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	RECT					m_rcSlot[24];
	RECT					m_rcCount[24];
	RECT					m_rcMoneyBox;
	RECT					m_rcTextBox;
	vector<CStatInfo::ITEM>			m_vecItem;
	CGameObject*			m_StatInfo;
	_bool					m_bMousePick = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void	Set_Slot();
	void	Check_Slot();
	void	UseItem(CStatInfo::EITEM _eItem, _int Index);
public:
	static CInven* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END