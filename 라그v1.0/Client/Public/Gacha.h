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

class CGacha final : public CGameObject
{
private:
	CGacha(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGacha(const CGacha& rhs);
	virtual ~CGacha() = default;
public:
	enum STATE { PAY,GACHA,GACHA2,STATE_END };
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom2 = nullptr;
	CTexture*				m_pTextureCom3 = nullptr;
	CTexture*				m_pTextureCom4 = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	STATE				m_ePreState;
	STATE				m_eCurState;
	CGameObject*			m_StatInfo = nullptr;
	_int				iSour = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	_int Check_Item();
public:
	static CGacha* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END