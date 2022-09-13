#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQuickSlot : public CGameObject
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
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float					m_fPrePosX = 0, m_fPrePosY = 0;
	Pos						m_Pass;
	vector < CGameObject* > m_vSlot;
	CGameObject*			m_iTemp = nullptr;
	bool					m_bCheck = false;
	int						m_iSour = 0;
	int						Temp = 0;
	void					QuickUse();
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
public:
	void Change(void);

public:
	static CQuickSlot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END
