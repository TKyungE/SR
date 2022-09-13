#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CPoring final : public CGameObject
{
public:
	enum STATE { IDLE, MOVE, STATE_END };
private:
	CPoring(LPDIRECT3DDEVICE9 _pGraphic_Device);
	CPoring(const CPoring& rhs);
	virtual ~CPoring() = default;

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


private:
	void OnTerrain();
	void	OnBillboard();
	HRESULT SetUp_Components(void);
	HRESULT SetUp_RenderState(void);
	HRESULT Release_RenderState(void);
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	void Chase(_float fTimeDelta);
	void Motion_Change();
	void Move_Frame(_float fTimeDelta);
	void Check_Front();
	HRESULT TextureRender();
private:
	STATE				m_ePreState;
	STATE				m_eCurState;
	_bool	m_bFront = false;

public:
	static CPoring* Create(LPDIRECT3DDEVICE9 _pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free(void) override;
};

END