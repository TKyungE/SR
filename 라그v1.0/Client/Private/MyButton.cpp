#include "stdafx.h"
#include "..\Public\MyButton.h"

CMyButton::CMyButton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMyButton::CMyButton(const CMyButton & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMyButton::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::Initialize(void * pArg)
{
	return E_NOTIMPL;
}

void CMyButton::Tick(_float fTimeDelta)
{
}

void CMyButton::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMyButton::Render()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::SetUp_Components()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::SetUp_RenderState()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::Release_RenderState()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::On_SamplerState()
{
	return E_NOTIMPL;
}

HRESULT CMyButton::Off_SamplerState()
{
	return E_NOTIMPL;
}

CMyButton * CMyButton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

CGameObject * CMyButton::Clone(void * pArg)
{
	return nullptr;
}

_float4x4 CMyButton::Get_World(void)
{
	return _float4x4();
}

void CMyButton::Free()
{
}
