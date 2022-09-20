#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_TOWN, LEVEL_CHOBOFIELD, LEVEL_MIDBOSS,LEVEL_TOWN2,LEVEL_DESERT1, LEVEL_END };
	enum CHANNELID { SOUND_EFFECT, SOUND_BGM, MAXCHANNEL };
	enum MONSTERTYPE { MON_ALLIGATOR, MON_ELDERWILOW, MON_BIGFOOT, MON_BYORGUE, MON_BLOODYMURDERER, MON_DANDELION, MON_END };
	enum COLLISIONGROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_BOSS, COLLISION_OBJECT, COLLISION_ITEM, COLLISION_PLAYERSKILL, COLLISION_MONSTERSKILL, COLLISION_PORTAL, COLLISION_PET, COLLISION_TOTEM, COLLISION_NPC, COLLISION_END };
}
#include <time.h>
#include <string>
#include <io.h>
#include <process.h>
#include "../inc/fmod.h"
#include "../inc/fmod.hpp"
#pragma comment (lib, "../lib/x64/fmod_vc.lib")
using namespace FMOD;


// Client_Defines.h

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

extern bool g_bCollider;
extern bool g_bTalk;
extern bool g_bCut;
extern bool g_bFirst;
extern bool g_bQuest;
extern bool g_bReward;


//extern int g_iFrame;

#define     fSOUND  0.1f
typedef struct UiPos
{
	float fPosX;
	float fPosY;
	bool  bNext;

}Pos;

using namespace Client;