#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CCollider;
class CQuest;
END

BEGIN(Client)

class CVillage_Quest1 final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_uint	iIndex;
	}INDEXPOS;

private:
	CVillage_Quest1(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CVillage_Quest1(const CVillage_Quest1& rhs);
	virtual ~CVillage_Quest1() = default;

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
	CTransform* m_pQuestTransformCom = nullptr;
	CVIBuffer_Rect* m_pQuestVIBufferCom = nullptr;
	CCollider* m_pQuestColliderCom = nullptr;
	CTexture* m_pQuestTextureCom = nullptr;

	CVIBuffer_Rect* m_pCharVIBufferCom = nullptr;
	CTransform* m_pCharTransformCom = nullptr;
	CTexture* m_pCharTextureCom = nullptr;


private:
	INDEXPOS	m_IndexPos;

	_bool m_bTalk = false;
	CQuest* m_pQuest = nullptr;


	_float4x4	m_ProjMatrix;
	_float		m_fSizeX = 0.f;
	_float		m_fSizeY = 0.f;
	_float		m_fX = 0.f;
	_float		m_fY = 0.f;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void	OnBillboard();

public:
	static CVillage_Quest1* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END