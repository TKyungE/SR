#include "stdafx.h"
#include "..\Public\BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 5;
	m_tFrame.fFrameSpeed = 0.3f;


	return S_OK;
}

void CBackGround::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 카메라의 트랜스폼을 찾아(CCamera::m_pTransformTag);

	if (GetKeyState(VK_UP) < 0)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (GetKeyState(VK_DOWN) < 0)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState(VK_LEFT) < 0)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta * -1.f);
		// m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (GetKeyState(VK_RIGHT) < 0)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
		// m_pTransformCom->Go_Right(fTimeDelta);
	}

	//상태에따라 스프라이트 돌리는 함수
	Move_Frame(fTimeDelta);
	
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//상태변경 확인 함수
	Motion_Change();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CBackGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	
	
	//상태에 따라 바인드하는 함수
	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	return S_OK;
}


void CBackGround::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{//필요한 상태들 추가하고 값 지정해주면됨
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.fFrameSpeed = 0.3f;
			break;
		case WALK:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.fFrameSpeed = 0.3f;
			break;
		case ATTACK:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.fFrameSpeed = 0.3f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBackGround::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		m_tFrame.iFrameStart = m_pTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case WALK:
	//	m_tFrame.iFrameStart = m_pTextureCom2->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case ATTACK:
	//	m_tFrame.iFrameStart = m_pTextureCom3->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
	
}

HRESULT CBackGround::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	case WALK:
	//	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
	//		return E_FAIL;
		break;
	case ATTACK:
	//	if (FAILED(m_pTextureCom3->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
	//		return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}


HRESULT CBackGround::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"), (CComponent**)&m_pTextureCom)))
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

HRESULT CBackGround::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	

	return S_OK;
}

HRESULT CBackGround::Release_RenderState()
{
	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround*	pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackGround::Clone(void* pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CBackGround::Get_World(void)
{
	return _float4x4();
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
