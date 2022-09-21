#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_WingRect;
class CVIBuffer_RectLeft;
END

BEGIN(Client)

class CWing final : public CGameObject
{
private:
	CWing(LPDIRECT3DDEVICE9 pGraphic_Device);
	CWing(const CWing& rhs);
	virtual ~CWing() = default;
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
	CVIBuffer_WingRect*			m_pVIBufferCom = nullptr;
	CVIBuffer_RectLeft*			m_pVIBufferCom2 = nullptr;
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_float				m_TurnTime = 0.f;
	_bool				m_bTurn = false;
	_bool				m_bFront = false;
	_float				vfTurn = 1.f;
	_float3				m_OriginvRight;
	_float3				m_OriginvLook;
	_float				m_fAngle = 0.f;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	Set_TargetPos();
	void	Set_TargetPos2();
	void	WingTurn(_float fTimeDelta);
	void	WingTurn2(_float fTimeDelta);
public:
	static CWing* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END
