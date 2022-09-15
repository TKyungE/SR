#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMyButton abstract : public CGameObject
{
public:
	enum BUTTONTYPE { BUTTON_BACK, BUTTON_CLOSE, BUTTON_NEXT, BUTTON_RECEIVE, BUTTON_END };
public:
	typedef struct tagButtonInfo
	{
		CMyButton* pOut;
		_float3 vPos;
	}BINFO;

protected:
	CMyButton(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMyButton(const CMyButton& rhs);
	virtual ~CMyButton() = default;

public:
	_bool Get_Clicked(void) { return m_bClicked; }
	BUTTONTYPE Get_Type(void) { return m_eType; }
	void TurnOff_Clicked(void) { m_bClicked = false; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool m_bClicked = false;
	_bool m_bMouseCheck = false;
	RECT	m_rcBtn;

	_int m_iTex = 0;

	BINFO m_tBInfo;
	BUTTONTYPE m_eType = BUTTON_END;

protected:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual _float4x4 Get_World(void) = 0;
	virtual void Free() override;
};

END
