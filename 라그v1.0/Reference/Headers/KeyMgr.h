#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CKeyMgr final : public CBase
{
	DECLARE_SINGLETON(CKeyMgr)
private:
	CKeyMgr();
	~CKeyMgr();

public:
	bool		Key_Pressing(int _Key);
	bool		Key_Up(int _Key);
	bool		Key_Down(int _Key);

private:
	
	bool						m_bKeyState[VK_MAX];
public:
	virtual void Free();
};

END