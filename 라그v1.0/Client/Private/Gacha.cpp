#include "stdafx.h"
#include "..\Public\Gacha.h"
#include "GameInstance.h"
#include "Layer.h"

CGacha::CGacha(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CGacha::CGacha(const CGacha & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGacha::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGacha::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	D3DXMatrixOrthoLH(&m_ProjMatrix, (float)g_iWinSizeX, (float)g_iWinSizeY, 0.f, 1.f);

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"));
	m_StatInfo = pLayer->Get_Objects().front();
	Safe_Release(pGameInstance);


	m_fSizeX = 550.f;
	m_fSizeY = 420.f;
	m_fX = 640.f;
	m_fY = 360.f;

	m_ePreState = STATE_END;
	m_eCurState = PAY;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 2;
	m_tFrame.fFrameSpeed = 2.f;
	m_tInfo.bDead = false;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));

	return S_OK;
}

void CGacha::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	Move_Frame(fTimeDelta);
	
	if (m_eCurState == PAY && m_tFrame.iFrameStart == 2)
	{
		iSour = Check_Item();
		if (iSour == 14 || iSour == 15)
		{
			m_fSizeX = 524.f;
			m_fSizeY = 566.f;
			m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
			m_eCurState = GACHA2;
			m_tFrame.iFrameStart = 0;
		}
		else
		{
			m_fSizeX = 510.f;
			m_fSizeY = 598.f;
			m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
			m_eCurState = GACHA;
			m_tFrame.iFrameStart = 0;
		}
	}
	if (m_eCurState == GACHA && m_tFrame.iFrameStart == 182)
	{
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == iSour)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				Set_Dead();
				return;
			}
		}
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == CStatInfo::EITEM_END)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum((CStatInfo::EITEM)iSour, i);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				Set_Dead();
				return;
			}
		}
		Set_Dead();
	}
	if (m_eCurState == GACHA2 && m_tFrame.iFrameStart == 233)
	{
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == iSour)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				Set_Dead();
				return;
			}
		}
		for (int i = 0; i < 24; ++i)
		{
			if (dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Item(i).eItemNum == CStatInfo::EITEM_END)
			{
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_ItemNum((CStatInfo::EITEM)iSour, i);
				dynamic_cast<CStatInfo*>(m_StatInfo)->Set_UseItemCount(1, i);
				Set_Dead();
				return;
			}
		}
		Set_Dead();
	}

	if (m_eCurState == GACHA && m_tFrame.iFrameStart == 128)
	{
		m_fSizeX = 280.f;
		m_fSizeY = 280.f;
		m_pTransformCom2->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, _float3((m_fX+30.f) - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	}
	if (m_eCurState == GACHA2 && m_tFrame.iFrameStart == 210)
	{
		m_fSizeX = 280.f;
		m_fSizeY = 280.f;
		m_pTransformCom2->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom2->Set_State(CTransform::STATE_POSITION, _float3((m_fX+30.f) - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	}

}

void CGacha::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Motion_Change();

	if (nullptr != m_pRendererCom && 0 == g_iCut)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CGacha::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	
	if (m_eCurState == PAY)
	{
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
	}
	if (m_eCurState == GACHA)
	{
		if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
	}
	if (m_eCurState == GACHA2)
	{
		if (FAILED(m_pTextureCom3->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
	}
	m_pVIBufferCom->Render();

	if (m_eCurState == GACHA && m_tFrame.iFrameStart > 127)
	{
		if (FAILED(m_pTextureCom4->Bind_OnGraphicDev(iSour)))
			return E_FAIL;
		if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
			return E_FAIL;
		m_pVIBufferCom2->Render();
	}
	if (m_eCurState == GACHA2 && m_tFrame.iFrameStart > 209)
	{
		if (FAILED(m_pTextureCom4->Bind_OnGraphicDev(iSour)))
			return E_FAIL;
		if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
			return E_FAIL;
		m_pVIBufferCom2->Render();
	}



	if (FAILED(Release_RenderState()))
		return E_FAIL;



	return S_OK;
}

HRESULT CGacha::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pay"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha2"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pTextureCom4)))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CGacha::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	return S_OK;
}

HRESULT CGacha::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CGacha * CGacha::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGacha*	pInstance = new CGacha(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGacha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGacha::Clone(void* pArg)
{
	CGacha*	pInstance = new CGacha(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGacha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CGacha::Get_World(void)
{
	return _float4x4();
}

void CGacha::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTextureCom4);
}
void CGacha::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case PAY:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 2;
			m_tFrame.fFrameSpeed = 2.f;
			break;
		case GACHA:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 182;
			m_tFrame.fFrameSpeed = 0.03f;
			break;
		case GACHA2:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 233;
			m_tFrame.fFrameSpeed = 0.03f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CGacha::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case PAY:
		m_tFrame.iFrameStart = m_pTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case GACHA:
		m_tFrame.iFrameStart = m_pTextureCom2->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case GACHA2:
		m_tFrame.iFrameStart = m_pTextureCom3->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
}

_int CGacha::Check_Item()
{
	_int iDest = rand() % 3;
	_int iWin = rand() % 2;
	_int iDraw = rand() % 10;
	_int iLose = rand() % 14;
	switch (iDest)
	{
	case 0:
		switch (iWin)
		{
		case 0:
			return 14;
			break;
		case 1:
			return 15;
			break;
		default:
			break;
		}
		break;
	case 1:
		switch (iDraw)
		{
		case 0:
			return 4;
			break;
		case 1:
			return 5;
			break;
		case 2:
			return 6;
			break;
		case 3:
			return 7;
			break;
		case 4:
			return 8;
			break;
		case 5:
			return 9;
			break;
		case 6:
			return 10;
			break;
		case 7:
			return 11;
			break;
		case 8:
			return 12;
			break;
		case 9:
			return 13;
			break;
		default:
			break;
		}
		break;
	case 2:
		switch (iLose)
		{
		case 0:
			return 0;
			break;
		case 1:
			return 1;
			break;
		case 2:
			return 17;
			break;
		case 3:
			return 18;
			break;
		case 4:
			return 19;
			break;
		case 5:
			return 20;
			break;
		case 6:
			return 21;
			break;
		case 7:
			return 22;
			break;
		case 8:
			return 23;
			break;
		case 9:
			return 24;
			break;
		case 10:
			return 25;
			break;
		case 11:
			return 26;
			break;
		case 12:
			return 27;
			break;
		case 13:
			return 28;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return 99;
}
