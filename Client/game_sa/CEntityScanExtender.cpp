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


void CEntityScanExtenter::PatchOnce()
{
    // CEntity::Add(CRect) without lods (yet)

    MemPut(0x5347EB + 2, &m_woldLeft);
    MemPut(0x534819 + 2, &m_woldRight);
    MemPut(0x534832 + 2, &m_woldTop);
    MemPut(0x53484B + 2, &m_woldBottom);

    MemPut(0x534927 + 2, &m_halfSectorsX);
    MemPut(0x534948 + 2, &m_halfSectorsY);
    MemPut(0x534967 + 2, &m_halfSectorsX);
    MemPut(0x534988 + 2, &m_halfSectorsY);

    // CEntity::Remove without lods (yet)

    MemPut(0x534AFD + 2, &m_woldLeft);
    MemPut(0x534B2A + 2, &m_woldLeft);
    MemPut(0x534B3A + 2, &m_woldRight);
    MemPut(0x534B53 + 2, &m_woldTop);
    MemPut(0x534B6C + 2, &m_woldBottom);

    // 0x71CB70 CGlass::HasGlassBeenShatteredAtCoors ???

	// CStreaming::AddModelsToRequestList
	MemPut(0x40D45E + 2, &m_halfSectorsX);
    MemPut(0x40D47E + 2, &m_halfSectorsY);
    MemPut(0x40D49D + 2, &m_halfSectorsX);
    MemPut(0x40D4E9 + 2, &m_halfSectorsY);

    MemPut(0x40D52D + 2, &m_halfSectorsX);
    MemPut(0x40D57E + 2, &m_halfSectorsY);
}

void CEntityScanExtenter::PatchDynamic()
{
    // CEntity::Add(CRect)
    MemPut(0x53480D + 4, m_woldLeft);
    MemPut(0x534826 + 4, m_woldRight - 1.f);
    MemPut(0x53483F + 4, m_woldTop);
    MemPut(0x534858 + 4, m_woldBottom - 1.f);

    // CEntity::Remove
    MemPut(0x534B47 + 4, m_woldRight - 1.f);
    MemPut(0x534B60 + 4, m_woldTop);
    MemPut(0x534B79 + 4, m_woldBottom - 1.f);

    // CStreaming::AddModelsToRequestList

	MemPut(0x40D547 + 1, std::uint32_t(m_halfSectorsX - 1.f));

    MemPut(0x40D68C + 3, DEFAULT_SECTORS);

    int i = 10;
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

#define HOOKPOS_CStreaming__AddModelsToRequestLiså1  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLiså1 0x5
static std::size_t CStreaming__AddModelsToRequestList1_CONTINUE = 0x40D578;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLiså1()
{
    _asm {
			mov edi, instance
			mov ebp, [edi+0x8]       ; m_sectorsYMinusOne
			fld dword ptr [esp+0x14] ; original code
			sub esp, 8
			jmp CStreaming__AddModelsToRequestList1_CONTINUE
    }
}

#define HOOKPOS_CStreaming__AddModelsToRequestLiså2  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLiså2 0x5
static std::size_t CStreaming__AddModelsToRequestList2_CONTINUE = 0x40D686;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLiså2()
{
    _asm {
			mov edi, instance
			mov edi, [edi] ; m_sectorsX
			dec edi
			cmp ecx, edi
			jl sectorY
			mov ecx, edi
		sectorY:
			mov edi, instance
			mov edi, [edi+0x4] ; m_sectorsY
			dec edi
			mov ecx, [esp+0x5C]
			cmp eax, edi
			jl tableAccess
			mov eax, edi
		tableAccess:
			inc edi
			imul eax, edi
			jmp CStreaming__AddModelsToRequestList2_CONTINUE
    }
}

void CEntityScanExtenter::StaticSetHooks()
{
    instance = std::make_unique<CEntityScanExtenter>();

    EZHookInstall(CRenderer__SetupScanLists);
    EZHookInstall(CStreaming__AddModelsToRequestLiså1);
    EZHookInstall(CStreaming__AddModelsToRequestLiså2);
    EZHookInstall(GetSector);
}
