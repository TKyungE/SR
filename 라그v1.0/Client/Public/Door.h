#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect3;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CDoor final : public CGameObject
{
public:
	typedef struct tagIndexPos
	{
		_float3		vPos;
		_float3		vScale;
		_uint	iIndex;
		_uint	iLevelIndex;
	}INDEXPOS;

private:
	CDoor(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;
	
public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render(void) override;

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Rect3* m_pVIBuffer = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CCollider* m_pColliderCom = nullptr;
	CShader*	m_pShaderCom = nullptr;
	CCollider* m_pQuestColliderCom = nullptr;
private:
	INDEXPOS	m_IndexPos;
	_uint		m_iCheck = 0;
	_float		m_fTime = 0.f;
	_bool		m_bTalk = false;
	vector<wstring>			m_vNormalScript;
	vector<wstring>			m_vFindScript;
	_bool		m_bCheck = false;
	_uint		m_iCount = 0.f;
	_bool		m_bLine = false;
private:
	HRESULT SetUp_Components(void);
	void Ready_Script(void);
	
public:
	static CDoor* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END