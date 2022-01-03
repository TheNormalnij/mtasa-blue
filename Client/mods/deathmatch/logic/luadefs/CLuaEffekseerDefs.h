/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaEffekseerDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CLuaDefs.h"

class CLuaEffekseerDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static CClientEffekseerEffect*        loadEffekseerFx(lua_State* const luaVM, std::string strPath);
    static CClientEffekseerEffectHandler* playEffekseerFx(CClientEffekseerEffect* pEffect, CVector vecPos);
};
