#pragma once

#include "MyButton.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CTextBox final : public CGameObject
{
public:
	typedef struct tagTextInfo
	{
		wstring* pScript;
		_int iScriptSize;

		_int iQuestIndex;
		_int iLevelIndex;
	}TINFO;
private:
	CTextBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTextBox(const CTextBox& rhs);
	virtual ~CTextBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	RECT					m_rcBox;
	wstring					m_wstr;
	_int					m_iIndex = 0;
	_int					m_iScriptIndex = 0;
	_float					m_fTimeDelta = 0.f;
	_bool					m_bFontRender = false;
	wstring*				m_pScript = nullptr;

	vector<CMyButton*>	m_vButtonArray;
	CMyButton*		m_pButton = nullptr;

	TINFO m_tTInfo;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	HRESULT On_SamplerState();
	HRESULT Off_SamplerState();
	HRESULT Create_Buttons(void);
	void Running_TextBox(void);
	void Print_Text(void);
	void Change_Button(void);

public:
	static CTextBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual _float4x4 Get_World(void) override;
	virtual void Free() override;
};

END