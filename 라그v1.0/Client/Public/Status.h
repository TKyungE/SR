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

class CStatus final : public CGameObject
{
private:
	CStatus(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

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
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CTransform*				m_pTransformCom4 = nullptr;
	CTransform*				m_pTransformCom5 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom4 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom5 = nullptr;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	RECT					m_rcStat[8];
	RECT					m_rcButton[4];
	CGameObject*			m_StatInfo;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CStatus* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END