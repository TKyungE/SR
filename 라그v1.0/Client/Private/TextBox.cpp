#include "stdafx.h"
#include "..\Public\TextBox.h"
#include "GameInstance.h"
#include "MyButton.h"

CTextBox::CTextBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(&m_tTInfo, sizeof(TINFO));
}

CTextBox::CTextBox(const CTextBox & rhs)
	: CGameObject(rhs)
	, m_tTInfo(rhs.m_tTInfo)
{
}

HRESULT CTextBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_tTInfo, pArg, sizeof(TINFO));

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = (_float)g_iWinSizeX * 0.85f;
	m_fSizeY = (_float)g_iWinSizeY * 0.3f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.8f;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	m_rcBox = { 180, 500, 1120, 650 };

	m_pScript = m_tTInfo.pScript;

	return S_OK;
}

void CTextBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (0 == m_vButtonArray.size())
	{
		if (FAILED(Create_Buttons()))
		{
			ERR_MSG(TEXT("Failed to Create Buttons"));
			return;
		}
	}
	else if ((m_iScriptIndex == m_tTInfo.iQuestIndex) && (CMyButton::BUTTON_RECEIVE != m_vButtonArray[2]->Get_Type()))
		Change_Button();

	Running_TextBox();

	Print_Text();

	m_fTimeDelta += fTimeDelta;
}

void CTextBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTextBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	_float4x4		ViewMatrix;
	D3DXMatrixIdentity(&ViewMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return E_FAIL;

	Safe_AddRef(pInstance);

	pInstance->Get_Font()->DrawText(nullptr, m_wstr.c_str(), (_int)m_wstr.length(), &m_rcBox, DT_LEFT | DT_WORDBREAK, D3DCOLOR_ARGB(255, 0, 0, 0));

	Safe_Release(pInstance);

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}

void CTextBox::TextBoxDead()
{
	/*for (auto& iterator : m_vButtonArray)
		iterator->Set_Dead();

	m_vButtonArray.clear();*/

	Set_Dead();

	if (g_bCut)
		g_bCut = false;

}

HRESULT CTextBox::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TextBox"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextBox::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CTextBox::Release_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

HRESULT CTextBox::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CTextBox::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CTextBox::Create_Buttons(void)
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return E_FAIL;

	Safe_AddRef(pInstance);

	CMyButton::BINFO tBInfo;
	
	for (_int i = 0; i < 3; ++i)
	{
		tBInfo.vPos = _float3(g_iWinSizeX * (0.75f + (0.05f * i)), g_iWinSizeY * 0.92f, 0.f);
		tBInfo.iType = i;
		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_MyButton"), m_tTInfo.iLevelIndex, TEXT("Layer_UI"), &tBInfo)))
		{
			ERR_MSG(TEXT("Failed to Add GameObject : MyButton"));
			return E_FAIL;
		}

		m_pButton = tBInfo.pOut;
		m_vButtonArray.push_back(m_pButton);
	}
	
	Safe_Release(pInstance);
	
	return S_OK;
}

void CTextBox::Running_TextBox(void)
{
	for (auto& iter : m_vButtonArray)
	{
		if (iter->Get_Clicked())
		{
			switch (iter->Get_Type())
			{
			case CMyButton::BUTTON_CLOSE:
			{
				for (auto& iterator : m_vButtonArray)
					iterator->Set_Dead();

				m_vButtonArray.clear();
				
				Set_Dead();
				
				if (g_bCut)
					g_bCut = false;
				
				break;
			}
			case CMyButton::BUTTON_BACK:
			{
				if (m_iScriptIndex > 0)
				{
					--m_iScriptIndex;
					m_wstr = TEXT("");
					m_iIndex = 0;
				}
				iter->TurnOff_Clicked();
				break;
			}
			case CMyButton::BUTTON_NEXT:
			{
				if (m_iScriptIndex < m_tTInfo.iScriptSize - 1)
				{
					++m_iScriptIndex;
					m_wstr = TEXT("");
					m_iIndex = 0;
				}

				iter->TurnOff_Clicked();
				break;
			}
			case CMyButton::BUTTON_RECEIVE:
			{
				g_bQuest = true;
				g_bReward = true;

				for (auto& iterator : m_vButtonArray)
					iterator->Set_Dead();

				m_vButtonArray.clear();

				Set_Dead();

				if (g_bCut)
					g_bCut = false;

				break;
			}
			}

			break;
		}
	}
}

void CTextBox::Print_Text(void)
{
	wstring wstr = m_pScript[m_iScriptIndex];

	if (m_iIndex < wstr.length() && m_fTimeDelta > 0.03f)
	{
		m_wstr += wstr[m_iIndex];
		++m_iIndex;
		m_fTimeDelta = 0.f;
		m_bFontRender = true;
	}
}

void CTextBox::Change_Button(void)
{
	m_vButtonArray[2]->Set_Dead();
	m_vButtonArray.erase(m_vButtonArray.begin() + 2);
	
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	CMyButton::BINFO tBInfo;

	tBInfo.vPos = _float3(g_iWinSizeX * 0.85f, g_iWinSizeY * 0.92f, 0.f);
	tBInfo.iType = 3;

	if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_MyButton"), m_tTInfo.iLevelIndex, TEXT("Layer_UI"), &tBInfo)))
	{
		ERR_MSG(TEXT("Failed to Add GameObject : MyButton"));
		return;
	}

	m_pButton = tBInfo.pOut;
	m_vButtonArray.push_back(m_pButton);
	
	Safe_Release(pInstance);
}

CTextBox * CTextBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTextBox*	pInstance = new CTextBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTextBox::Clone(void * pArg)
{
	CTextBox*	pInstance = new CTextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CTextBox::Get_World(void)
{
	return _float4x4();
}

void CTextBox::Free()
{
	for (auto& iterator : m_vButtonArray)
		iterator->Set_Dead();

	m_vButtonArray.clear();


	if (nullptr != m_pScript)
	{
		for (_int i = 0; i < m_tTInfo.iScriptSize; ++i)
			m_pScript[i].clear();
		Safe_Delete_Array(m_pScript);
	}

	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
