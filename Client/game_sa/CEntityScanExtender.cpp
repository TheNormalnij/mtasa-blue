/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEntityScanExtender.h"
#include "CRepeatSectorSAInterface.h"

struct tScanLists
{
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* buildingsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* objectsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* vehiclesList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* pedsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* dummiesList;
};

static CRepeatSector (&DEFAULT_REPEAT_SECTORS)[16][16] = *(CRepeatSector(*)[16][16])0xB992B8;
static CSector       (&DEFAULT_SECTORS)[MAX_SECTORS_Y][MAX_SECTORS_X] = *(CSector(*)[MAX_SECTORS_Y][MAX_SECTORS_X])0xB7D0B8;
static auto*         SCAN_LIST = (tScanLists*)0xC8E0C8;

static std::unique_ptr<CEntityScanExtenter> instance;

// Missing in C++14
static std::int32_t clamp(std::int32_t v, std::int32_t min, std::int32_t max)
{
    if (v < min)
        return min;
    if (v > max)
        return max;

    return v;
}

CEntityScanExtenter::CEntityScanExtenter()
{
    PatchOnce();
    // Only for tests
    PatchDynamic();
}

bool CEntityScanExtenter::IsInWorldSector(std::int32_t x, std::int32_t y) const noexcept
{
    return x >= 0 && x <= m_sectorsX && y >= 0 && y <= m_sectorsY;
}

CSector* CEntityScanExtenter::GetSector(std::uint32_t x, std::uint32_t y) const noexcept
{
    return &DEFAULT_SECTORS[y][x];
}

CSector* CEntityScanExtenter::GetSectorResize(std::uint32_t x, std::uint32_t y)
{
    if (IsInWorldSector(x, y))
        return GetSector(x, y);

    return GetSector(x, y);
}

void CEntityScanExtenter::Resize(std::size_t count)
{
    PatchDynamic();
}

#define HOOKPOS_GetSector  0x407260
#define HOOKSIZE_GetSector 0x5
CSector* __cdecl HOOK_GetSector(std::int32_t x, std::int32_t y)
{
    x = clamp(x, 0, instance->GetSectorsX() - 1);
    y = clamp(y, 0, instance->GetSectorsY() - 1);
    return instance->GetSector(x, y);
}

#define HOOKPOS_CRenderer__SetupScanLists  0x553540
#define HOOKSIZE_CRenderer__SetupScanLists 0x5
static void __cdecl HOOK_CRenderer__SetupScanLists(std::int32_t sectorX, std::int32_t sectorY)
{
    CRepeatSector* repeatSector = &DEFAULT_REPEAT_SECTORS[sectorY & 0xF][sectorX & 0xF];
    if (instance->IsInWorldSector(sectorX, sectorY))
    {
        CSector* sector = instance->GetSector(sectorX, sectorY);
        SCAN_LIST->buildingsList = &sector->m_buildings;
        SCAN_LIST->objectsList = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
        SCAN_LIST->vehiclesList = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
        SCAN_LIST->pedsList = &repeatSector->GetList(REPEATSECTOR_PEDS);
        SCAN_LIST->dummiesList = &sector->m_dummies;
    }
    else
    {
        // sector x and y are out of bounds
        SCAN_LIST->buildingsList = nullptr;
        SCAN_LIST->objectsList = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
        SCAN_LIST->vehiclesList = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
        SCAN_LIST->pedsList = &repeatSector->GetList(REPEATSECTOR_PEDS);
        SCAN_LIST->dummiesList = nullptr;
    }
}

void CEntityScanExtenter::StaticSetHooks()
{
    instance = std::make_unique<CEntityScanExtenter>();

    EZHookInstall(CRenderer__SetupScanLists);
    EZHookInstall(GetSector);
}

void CEntityScanExtenter::PatchOnce()
{
    // CEntity::Add(CRect) without lods

    MemPut(0x5347EB + 2, &m_woldLeft);
    MemPut(0x534819 + 2, &m_woldRight);
    MemPut(0x534832 + 2, &m_woldTop);
    MemPut(0x53484B + 2, &m_woldBottom);

    MemPut(0x534927 + 2, &m_halfSectorsX);
    MemPut(0x534948 + 2, &m_halfSectorsY);
    MemPut(0x534967 + 2, &m_halfSectorsX);
    MemPut(0x534988 + 2, &m_halfSectorsY);
}

void CEntityScanExtenter::PatchDynamic()
{
    MemPut(0x53480D + 4, m_woldLeft);
    MemPut(0x534826 + 4, m_woldRight - 1.f);
    MemPut(0x53483F + 4, m_woldTop);
    MemPut(0x534858 + 4, m_woldBottom - 1.f);
}
