#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCriHit final : public CGameObject
{
private:
	CCriHit(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCriHit(const CCriHit& rhs);
	virtual ~CCriHit() = default;
public:
	enum STATE { IDLE, STATE_END };

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
	CShader*				m_pShaderCom = nullptr;
private:

	_float				m_fDeadTime = 0.f;
	_float              m_fMoveX = 0.f;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Move_Frame(_float fTimeDelta);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	OnBillboard();
	void	Jump();
public:
	static CCriHit* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END