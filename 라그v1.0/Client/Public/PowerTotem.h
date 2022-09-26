#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CPowerTotem final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_uint	iIndex;
	}INDEXPOS;

private:
	CPowerTotem(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CPowerTotem(const CPowerTotem& rhs);
	virtual ~CPowerTotem() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTransform* m_pTransformCom2 = nullptr;
	CVIBuffer_Cube* m_pVIBufferCube = nullptr;
	CVIBuffer_Rect* m_pVIBufferRect = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CTexture* m_pTextureCom2 = nullptr;
	CCollider*				m_pColliderCom = nullptr;
private:
	_float		m_fHitTime = 0.f;
	_float		m_fDrainTime = 0.f;
	_float		m_fCollTime = 0.f;
	CGameObject*			m_StatInfo;
private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void OnTerrain(void);
	//void OnBillBoard(void);
	void Set_vPos();
	HRESULT House_Render();
	void CheckColl();
	void Check_Hit();
	HRESULT CreateDrain(_float fTimeDelta);
public:
	static CPowerTotem* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END
