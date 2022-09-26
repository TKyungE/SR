#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
class CShader;
END

BEGIN(Client)

class CDarkShield final : public CGameObject
{
private:
	CDarkShield(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDarkShield(const CDarkShield& rhs);
	virtual ~CDarkShield() = default;
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
	CCollider*				m_pColliderCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_float				m_fDeadTime = 0.f;
	_float				m_fCollTime = 0.f;
	_float3				vLook;
	_float				m_fAngle = 0.f;
	_float				m_ShotColl = 0.f;
	_bool				m_bShot = false;
	_int				m_iShot = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	HRESULT TextureRender();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	OnBillboard();
	void CheckColl();
	void Set_Pos();
	void Create_Ball();
public:
	static CDarkShield* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END