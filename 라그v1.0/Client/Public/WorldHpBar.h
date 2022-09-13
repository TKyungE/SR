#pragma once

#include"GameObject.h"
#include"Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_WingRect;
END

BEGIN(Client)
class CWorldHpBar : public CGameObject
{
private:
	CWorldHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWorldHpBar(const CWorldHpBar& rhs);
	virtual ~CWorldHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void	Tick(_float fTimeDelta)override;
	virtual void	Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;
	CVIBuffer_WingRect*			m_pVIBufferCom = nullptr;
	CVIBuffer_WingRect*			m_pVIBufferCom2 = nullptr;
private:

	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	bool*					m_Click;
	_ushort					m_Check = 0;

	short					m_Temp = 0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
public:
	void OnBillboard();

public:
	static CWorldHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END


