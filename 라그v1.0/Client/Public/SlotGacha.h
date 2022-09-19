#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ClientQuest.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CSlotGacha final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_uint	iIndex;
	}INDEXPOS;

private:
	CSlotGacha(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CSlotGacha(const CSlotGacha& rhs);
	virtual ~CSlotGacha() = default;

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
	CTexture* m_pTextureCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CCollider* m_pQuestColliderCom = nullptr;



private:
	INDEXPOS	m_IndexPos;

	_bool m_bTalk = false;
	_bool m_bOnce = false;
	_bool m_bQuestRender = true;

	vector<wstring> m_vNormalScript;


	_float4x4	m_ProjMatrix;
	_float		m_fSizeX = 0.f;
	_float		m_fSizeY = 0.f;
	_float		m_fX = 0.f;
	_float		m_fY = 0.f;

	_float		m_fAlpha = 0.f;
	_int m_iQuestTex = 0;

private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	OnBillboard();
	void Ready_Script(void);

public:
	static CSlotGacha* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END