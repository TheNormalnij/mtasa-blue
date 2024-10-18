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

struct tScanLists
{
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* buildingsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* objectsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* vehiclesList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* pedsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>* dummiesList;
};

enum eRepeatSectorList : std::int32_t
{
    REPEATSECTOR_VEHICLES = 0,
    REPEATSECTOR_PEDS = 1,
    REPEATSECTOR_OBJECTS = 2
};

class CRepeatSector
{
public:
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*>& GetList(eRepeatSectorList type) noexcept { return m_lists[type]; }

    const CPtrNodeDoubleListSAInterface<CEntitySAInterface*>& GetList(eRepeatSectorList type) const noexcept { return m_lists[type]; }
    // private: Preferrably use the accessor method
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*> m_lists[3];
};

static std::unique_ptr<CEntityScanExtenter> instance;

static CRepeatSector (&DEFAULT_REPEAT_SECTORS)[16][16] = *(CRepeatSector(*)[16][16])0xB992B8;
static CSector       (&DEFAULT_SECTORS)[MAX_SECTORS_Y][MAX_SECTORS_X] = *(CSector(*)[MAX_SECTORS_Y][MAX_SECTORS_X])0xB7D0B8;
static auto*         SCAN_LIST = (tScanLists*)0xC8E0C8;

CEntityScanExtenter::CEntityScanExtenter()
{
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
}

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

#define HOOKPOS_CRenderer__SetupScanLists  0x553540
#define HOOKSIZE_CRenderer__SetupScanLists 0x5
void CEntityScanExtenter::StaticSetHooks()
{
    instance = std::make_unique<CEntityScanExtenter>();

    EZHookInstall(CRenderer__SetupScanLists);
}
