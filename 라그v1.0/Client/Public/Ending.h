#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CEnding final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_float3		vScale;
		_uint	iIndex;
	}INDEXPOS;

private:
	CEnding(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CEnding(const CEnding& rhs);
	virtual ~CEnding() = default;
	
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CTransform*	m_pTransformCom2 = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer2 = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
private:
	INDEXPOS	m_IndexPos;
private:
	HRESULT SetUp_Components(void);
	
public:
	static CEnding* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END