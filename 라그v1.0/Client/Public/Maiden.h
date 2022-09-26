#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
class CShader;
END

BEGIN(Client)

class CMaiden final : public CGameObject
{
public:
	enum STATE { IDLE, DEAD, SKILL, MOVE, STATE_END };
private:
	CMaiden(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CMaiden(const CMaiden& rhs);
	virtual ~CMaiden() = default;

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
	CTexture* m_pTextureComIDLE_Front = nullptr;
	CTexture* m_pTextureComIDLE_Back = nullptr;
	CTexture* m_pTextureComMove_Front = nullptr;
	CTexture* m_pTextureComMove_Back = nullptr;
	CTexture* m_pTextureComAttack_Front = nullptr;
	CTexture* m_pTextureComAttack_Back = nullptr;
	CTexture* m_pTextureComDead_Front = nullptr;
	CTexture* m_pTextureComDead_Back = nullptr;
	CCollider* m_pColliderCom = nullptr;

	CVIBuffer_Rect* m_pCharVIBufferCom = nullptr;
	CTransform* m_pCharTransformCom = nullptr;
	CTexture* m_pCharTextureCom = nullptr;

	CShader* m_pShaderCom = nullptr;

private:
	void OnTerrain();
	void	OnBillboard();
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	HRESULT Skill_DefaultAttack(const _tchar * pLayerTag);
	void Check_Hit();
	void Chase(_float fTimeDelta);
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	void Check_Front();
	void Use_Skill(_float fTimeDelta);
	void Use_Skill2(_float fTimeDelta);
	HRESULT TextureRender();
	void CheckColl();
	void Create_BlueFire(_float fTimeDelta);
	void	DropItem();
	void Ready_Script(void);

private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_bool				m_bFront = false;
	_bool				m_bRight = false;
	_float				m_fSkillCool = 0.f;
	_float				m_fSkillCool2 = 0.f;
	_float				m_fRespawnTime = 0.f;
	_float				m_fDeadTime = 0.f;
	_float				m_fMove = 0.f;
	_float				m_fY = 0.f;
	_int				m_irand = 0;
	_bool				m_bSkill = false;
	_bool				m_bSkill2 = false;
	_bool				m_bMove = false;
	_bool				m_bDead = false;
	_bool				m_bTotem = false;
	_bool				m_bPowerTotem = false;
	_bool				m_bPowerTotem2 = false;
	_bool				m_bIDLE = false;
	_bool				m_bLastHeal = false;
	_bool				m_bBlueFire = false;
	_float				m_bBlueFireTime = 0.f;
	_bool				m_bStart = false;
	_int				m_iSkillMove = 0;
	_float				m_CollTime = 0.f;
	_float3				m_vTargetLook;

	vector<wstring> m_vScript;

	_float4x4	m_ProjMatrix;
	_float		m_fSizeX = 0.f;
	_float		m_fSizeY = 0.f;
	_float		m_fCharX = 0.f;
	_float		m_fCharY = 0.f;

	_float		m_fAlpha = 0.f;
	_int m_iQuestTex = 0;
	_bool m_bTalk = false;
	_float m_fTalkFrame = 0.f;
	_int m_iTalkFrame = 0;

public:
	static CMaiden* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END