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

class CLetterBox final : public CGameObject
{
private:
	CLetterBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLetterBox(const CLetterBox& rhs);
	virtual ~CLetterBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pUpTransformCom = nullptr;
	CTransform*				m_pDownTransformCom = nullptr;
	CVIBuffer_Rect*			m_pUpVIBufferCom = nullptr;
	CVIBuffer_Rect*			m_pDownVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fUpY, m_fDownY, m_fSizeX, m_fSizeY;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

public:
	static CLetterBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END