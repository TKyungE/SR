#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Cube;
class CTexture;
END

BEGIN(Client)

class CVillage_Quest2 final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_uint	iIndex;
	}INDEXPOS;

private:
	CVillage_Quest2(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CVillage_Quest2(const CVillage_Quest2& rhs);
	virtual ~CVillage_Quest2() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Cube* m_pVIBuffer = nullptr;
	CTexture* m_pTextureCom = nullptr;

private:
	INDEXPOS	m_IndexPos;

private:
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void OnTerrain(void);
	void	OnBillboard();
public:
	static CVillage_Quest2* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END