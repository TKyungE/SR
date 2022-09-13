#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDmgFont final : public CGameObject
{
private:
	CDmgFont(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDmgFont(const CDmgFont& rhs);
	virtual ~CDmgFont() = default;
public:
	enum STATE { ONE,TWO,THREE,FOUR,STATE_END };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private: /* For.Components */
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CTransform*				m_pTransformCom4 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom1 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom4 = nullptr;
private:
	STATE				m_eCurState;
	_int				m_iDraw[4];
	_float				m_fDeadTime = 0.f;
	_float3				m_vPos;
	_float              m_fMoveX = 0.f;
	_float				m_fTime = 0.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	HRESULT TextureRender();
	void	OnBillboard();
	void	Set_State();
	void	Set_vPos();
	void	Jump();
public:
	static CDmgFont* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END