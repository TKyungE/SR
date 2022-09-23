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

class CSkyNPC final : public CGameObject
{
private:
	CSkyNPC(LPDIRECT3DDEVICE9 pGraphic_Device);
	CSkyNPC(const CSkyNPC& rhs);
	virtual ~CSkyNPC() = default;

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
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
	CTransform*				m_pMouseTransformCom = nullptr;
	CVIBuffer_Rect*			m_pMouseBuffer = nullptr;
	CTexture*				m_pMouseTextureCom = nullptr;
private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float					m_fCool = 0.f;
	vector<wstring>			m_vNormalScript;
	_bool					m_bTrue = false;
	_bool					m_bTrue2 = false;
	_int					m_iNPC = 0;
	_int					m_iNPC2 = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	void Ready_Script(void);
	void Check_Index();
	void Move_Frame(_float fTimeDelta);
public:
	static CSkyNPC* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END