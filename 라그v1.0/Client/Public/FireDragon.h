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

class CFireDragon final : public CGameObject
{
public:
	enum STATE { IDLE, DEAD, SKILL, STATE_END };
private:
	CFireDragon(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CFireDragon(const CFireDragon& rhs);
	virtual ~CFireDragon() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CTexture* m_pTextureComIDLE_Front = nullptr;
	CTexture* m_pTextureComIDLE_Back = nullptr;
	CTexture* m_pTextureComAttack = nullptr;
	CTexture* m_pTextureComDead = nullptr;


private:
	void OnTerrain();
	void	OnBillboard();
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	HRESULT Skill_FireSpear(const _tchar * pLayerTag);
	HRESULT Skill_Meteor(const _tchar * pLayerTag);
	void Check_Hit();
	void Chase(_float fTimeDelta);
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	void Check_Front();
	void Use_Skill(_float fTimeDelta);
	HRESULT Create_Wind();
	HRESULT TextureRender();
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_bool	m_bFront = false;
	_float				m_fMeteorCool = 0.f;
	_float				m_fFireSpearCool = 0.f;
	_float				m_fDeadTime = 0.f;
	_bool	m_bSkill = false;
	_bool	m_bDead = false;
public:
	static CFireDragon* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END