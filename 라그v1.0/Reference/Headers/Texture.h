#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TYPE { TYPE_DEFAULT, TYPE_CUBEMAP, TYPE_END };
private:
	CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;
public:
	LPDIRECT3DBASETEXTURE9 Get_Texture(_uint iIndex) {
		return m_Textures[iIndex];
	}
public:
	HRESULT Initialize_Prototype(TYPE eType, const _tchar* pTextureFilePath, _uint iNumTexture);
	HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_OnGraphicDev(_uint iIndex = 0);
	_int MoveFrame(_float fTimeDelta, _float MoveSpeed, _int MaxFrame);
private:
	_uint										m_iNumTextures = 0;
	vector<IDirect3DBaseTexture9*>				m_Textures;
	_float										m_fFrameTime = 0.f;
	_int										m_iFrame = 0;
	//LPDIRECT3DCUBETEXTURE9
	//LPDIRECT3DTEXTURE9

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TYPE eType, const _tchar* pTextureFilePath, _uint iNumTexture = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END