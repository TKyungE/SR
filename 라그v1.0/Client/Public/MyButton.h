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

class CMyButton final : public CGameObject
{
public:
	enum BUTTONTYPE { BUTTON_CLOSE, BUTTON_BACK, BUTTON_NEXT, BUTTON_RECEIVE, BUTTON_END };
public:
	typedef struct tagButtonInfo
	{
		CMyButton* pOut;
		_float3 vPos;
		_int iType;
	}BINFO;

private:
	CMyButton(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMyButton(const CMyButton& rhs);
	virtual ~CMyButton() = default;

public:
	_bool Get_Clicked(void) { return m_bClicked; }
	BUTTONTYPE Get_Type(void) { return m_eType; }
	void TurnOff_Clicked(void) { m_bClicked = false; }

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render(void) override;

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool m_bClicked = false;
	_bool m_bMouseCheck = false;
	RECT	m_rcBtn;

	_int m_iTex = 0;

	BINFO m_tBInfo;
	BUTTONTYPE m_eType = BUTTON_END;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();

public:
	static CMyButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END
