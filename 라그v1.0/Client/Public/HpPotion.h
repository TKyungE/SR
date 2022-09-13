
#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CHpPotion : public CGameObject
{
private:
	CHpPotion(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHpPotion(const CHpPotion& rhs);
	virtual ~CHpPotion() = default;

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
	CGameObject*			m_pTest = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	bool*					m_Click = false;
	_ushort					m_Check = 0;
	CGameObject *			pTarget = nullptr;
	void					Use(void);
	LPD3DXFONT				m_pFont;
	RECT                    m_rcRect;
	_int					m_iCount;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void CheckColl();
public:
	static CHpPotion* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};
END

