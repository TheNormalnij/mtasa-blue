/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CEffekseerManagerInterface.h
 *  PURPOSE:     Exception effekseer manager interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "effekseer/src/Effekseer/Effekseer.h"

class CEffekseerManagerInterface;

#pragma once

class CEffekseerManagerInterface
{
public:
    virtual Effekseer::ManagerRef GetInterface() = 0;
};
