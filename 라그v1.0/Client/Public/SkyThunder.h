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

class CSkyThunder final : public CGameObject
{
private:
	CSkyThunder(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkyThunder(const CSkyThunder& rhs);
	virtual ~CSkyThunder() = default;
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
	CTransform*				m_pTransformCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_float				m_fDeadTime = 0.f;
	_float3				vLook;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT TextureRender();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void CheckColl();
	void	Set_vPos();
public:
	static CSkyThunder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END