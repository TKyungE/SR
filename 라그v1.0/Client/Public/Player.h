#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE { IDLE,MOVE,SKILL,MOVE_LEFT, STATE_END };
private:
	CPlayer(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CCollider* m_pColliderCom = nullptr;

	CTexture* m_pTextureComIDLE_Front = nullptr;
	CTexture* m_pTextureComIDLE_Back = nullptr;
	CTexture* m_pTextureComMove_Front = nullptr;
	CTexture* m_pTextureComMove_Back = nullptr;
	CTexture* m_pTextureComMove_Left_Front = nullptr;
	CTexture* m_pTextureComMove_Left_Back = nullptr;
	CTexture* m_pTextureComSkill_Front = nullptr;
	CTexture* m_pTextureComSkill_Back = nullptr;
	
	CTexture* m_pTextureComRide_IDLE_Front = nullptr;
	CTexture* m_pTextureComRide_IDLE_Back = nullptr;
	CTexture* m_pTextureComRide_Move_Front = nullptr;
	CTexture* m_pTextureComRide_Move_Back = nullptr;
	CTexture* m_pTextureComRide_Move_Left_Front = nullptr;
	CTexture* m_pTextureComRide_Move_Left_Back = nullptr;

private:
	void CheckColl();
	void OnTerrain();
	void	OnBillboard();
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void Use_Skill();
	void Key_Input(_float fTimeDelta);
	HRESULT Skill_Thunder(const _tchar * pLayerTag, _float3 _vPos);
	HRESULT Skill_Tornado(const _tchar * pLayerTag,_float3 _vPos);
	HRESULT Skill_FireBall(const _tchar * pLayerTag, _float3 _vPos);
	HRESULT Skill_FireSpear(const _tchar * pLayerTag, _float3 _vPos);
	HRESULT Skill_Meteor(const _tchar * pLayerTag, _float3 _vPos);
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
private:
	_float3 m_vTargetLook;
	_float3 m_vTarget;
	_bool	m_bUseSkill = false;
	_bool	m_bThunder = false;
	_bool	m_bTornado = false;
	_bool	m_bFireBall = false;
	_bool	m_bFireSpear = false;
	_bool	m_bMeteor = false;
	_bool	m_bFront = false;
	_bool	m_bRight = false;
	_bool	m_bLeft = false;
	_bool	m_bCamera = false;
	_bool   m_bRide = false;
	_bool	m_bFly = false;
	_float	m_fFly_fY = 0.f;
	STATE				m_ePreState;
	STATE				m_eCurState;

public:
	_float3 Get_Pos();
	_bool Get_Front() { return m_bFront; }
	void Set_Front(_bool bfront) { m_bFront = bfront; }
	void Set_Camera(_bool bCamera) { m_bCamera = bCamera; }
	_bool Get_Fly() { return m_bFly; }
	void Set_Fly(_bool _bFly) {	m_bFly = _bFly;}
private:
	void Player_Move(_float fTimeDelta);
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	void Check_Front();
	HRESULT TextureRender();
	void Check_Hit();
	void Get_PickingPoint(void);

private:
	_float3 m_fPickPoint = { 0.f, 0.f, 0.f };
};

END