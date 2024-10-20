/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEntitySA.h"
#include "CPtrNodeDoubleListSA.h"

enum eRepeatSectorList : std::int32_t
{
    REPEATSECTOR_VEHICLES = 0,
    REPEATSECTOR_PEDS = 1,
    REPEATSECTOR_OBJECTS = 2
};

class CRepeatSector
{
public:
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>& GetList(eRepeatSectorList type) noexcept { return m_lists[type]; }

    const CPtrNodeDoubleListSAInterface<CEntitySAInterface>& GetList(eRepeatSectorList type) const noexcept { return m_lists[type]; }
    // private: Preferrably use the accessor method
    CPtrNodeDoubleListSAInterface<CEntitySAInterface> m_lists[3];
};
