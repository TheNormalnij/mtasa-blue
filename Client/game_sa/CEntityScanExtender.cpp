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
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* buildingsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* objectsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* vehiclesList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* pedsList;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface>* dummiesList;
};

const auto MAX_REPEAT_SECTORS_X = 16;
const auto MAX_REPEAT_SECTORS_Y = 16;

static CRepeatSector (&DEFAULT_REPEAT_SECTORS)[MAX_REPEAT_SECTORS_Y][MAX_REPEAT_SECTORS_X] = *(CRepeatSector(*)[16][16])0xB992B8;
static CSector       (&DEFAULT_SECTORS)[MAX_SECTORS_Y][MAX_SECTORS_X] = *(CSector(*)[MAX_SECTORS_Y][MAX_SECTORS_X])0xB7D0B8;
static auto*         SCAN_LIST = (tScanLists*)0xC8E0C8;

static std::unique_ptr<CEntityScanExtenter> instance;

static std::uint32_t CURRENT_SECTORS_X = 120;
static std::uint32_t CURRENT_SECTORS_Y = 120;
static std::uint32_t CURRENT_SECTORS_X_MINUS_ONE = 119;
static std::uint32_t CURRENT_SECTORS_Y_MINUS_ONE = 119;

CEntityScanExtenter::CEntityScanExtenter()
{
    PatchOnce();
    // Only for tests
    PatchDynamic();
}

bool CEntityScanExtenter::IsInWorldSector(std::int32_t x, std::int32_t y) const noexcept
{
    return x >= 0 && x <= (std::int32_t)GetSectorsX() && y >= 0 && y <= (std::int32_t)GetSectorsY();
}

CSector* CEntityScanExtenter::GetSector(std::uint32_t x, std::uint32_t y) const noexcept
{
    return &DEFAULT_SECTORS[y][x];
}

std::uint32_t CEntityScanExtenter::GetSectorsX() const noexcept
{
    return CURRENT_SECTORS_X;
}

std::uint32_t CEntityScanExtenter::GetSectorsY() const noexcept
{
    return CURRENT_SECTORS_Y;
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

std::int32_t CEntityScanExtenter::GetSectorX(float x) const noexcept
{
    x = x / static_cast<float>(m_SectorsW / CURRENT_SECTORS_X) + static_cast<float>(CURRENT_SECTORS_X / 2);
    return std::floor(x);
}

std::int32_t CEntityScanExtenter::GetSectorY(float y) const noexcept
{
    y = y / static_cast<float>(m_SectorsH / CURRENT_SECTORS_Y) + static_cast<float>(CURRENT_SECTORS_Y / 2);
    return std::floor(y);
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

	// CStreaming::DeleteAllRwObjects

	MemPut(0x4090D7 + 2, &m_halfSectorsX);
    MemPut(0x4090F1 + 2, &m_halfSectorsY);

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

	// CStreaming::DeleteAllRwObjects


    int i = 10;
}

// Missing in C++14
static std::int32_t clamp(std::int32_t v, std::int32_t min, std::int32_t max)
{
    if (v < min)
        return min;
    if (v > max)
        return max;

    return v;
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

    SCAN_LIST->objectsList = &repeatSector->GetList(REPEATSECTOR_OBJECTS);
    SCAN_LIST->vehiclesList = &repeatSector->GetList(REPEATSECTOR_VEHICLES);
    SCAN_LIST->pedsList = &repeatSector->GetList(REPEATSECTOR_PEDS);

    if (instance->IsInWorldSector(sectorX, sectorY))
    {
        CSector* sector = instance->GetSector(sectorX, sectorY);
        SCAN_LIST->buildingsList = &sector->m_buildings;
        SCAN_LIST->dummiesList = &sector->m_dummies;
    }
    else
    {
        // sector x and y are out of bounds
        SCAN_LIST->buildingsList = nullptr;
        SCAN_LIST->dummiesList = nullptr;
    }
}

#define HOOKPOS_CStreaming__AddModelsToRequestLis�1  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLis�1 0x5
static std::uint32_t CStreaming__AddModelsToRequestList1_CONTINUE = 0x40D578;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLis�1()
{
    _asm {
			mov ebp, CURRENT_SECTORS_Y_MINUS_ONE
			fld dword ptr [esp+0x14] ; original code
			sub esp, 8
			jmp CStreaming__AddModelsToRequestList1_CONTINUE
    }
}

#define HOOKPOS_CStreaming__AddModelsToRequestLis�2  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLis�2 0x5
static std::uint32_t CStreaming__AddModelsToRequestList2_CONTINUE = 0x40D686;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLis�2()
{
    _asm {
			mov edi, CURRENT_SECTORS_X_MINUS_ONE
			cmp ecx, edi
			jl sectorY
			mov ecx, edi
        sectorY:
			mov edi, CURRENT_SECTORS_Y_MINUS_ONE
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

#define HOOKPOS_CStreaming__DeleteAllRwObjects1  0x409125
#define HOOKSIZE_CStreaming__DeleteAllRwObjects1 0x5
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects1_CONTINUE = 0x409133;
void __declspec(naked) HOOK_CStreaming__DeleteAllRwObjects1()
{
    _asm {
        mov eax, CURRENT_SECTORS_X
        dec eax
        cmp ecx, eax
        jl exitHook
        mov ecx, eax
    exitHook:
        jmp CStreaming__CStreaming__DeleteAllRwObjects1_CONTINUE
    }
}

#define HOOKPOS_CStreaming__DeleteAllRwObjects2  0x40913F
#define HOOKSIZE_CStreaming__DeleteAllRwObjects2 0x5
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects2_CONTINUE = 0x40914C;
void __declspec(naked) HOOK_CStreaming__DeleteAllRwObjects2()
{
    _asm {
        mov esi, CURRENT_SECTORS_Y_MINUS_ONE
        cmp eax, esi
        jl tableOffsetCalc
        mov eax, esi
    tableOffsetCalc:
        inc esi
        imul eax, esi
        jmp CStreaming__CStreaming__DeleteAllRwObjects2_CONTINUE
    }
}

#define HOOKPOS_CStreaming__DeleteAllRwObjects3  0x4091AA
#define HOOKSIZE_CStreaming__DeleteAllRwObjects3 0x5
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects3_CONTINUE = 0x4091C3;
void __declspec(naked) HOOK_CStreaming__DeleteAllRwObjects3()
{
    _asm {
        mov esi, CURRENT_SECTORS_X_MINUS_ONE
        cmp ecx, esi
        jl nextCheck
        mov ecx, esi
    nextCheck:
        mov eax, ebx
        mov esi, CURRENT_SECTORS_Y_MINUS_ONE
        cmp eax, esi
        jl calcTableOffset
        mov eax, esi
    calcTableOffset:
        inc esi
        imul eax, esi
        jmp CStreaming__CStreaming__DeleteAllRwObjects3_CONTINUE
    }
}

#define HOOKPOS_CStreaming__DeleteAllRwObjects4  0x4091E7
#define HOOKSIZE_CStreaming__DeleteAllRwObjects4 0x5
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects4_LOOP_Y = 0x409125;
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects4_LOOP_X = 0x409110;
static std::uint32_t CStreaming__CStreaming__DeleteAllRwObjects4_CONTINUE = 0x4091F8;
void __declspec(naked) HOOK_CStreaming__DeleteAllRwObjects4()
{
    _asm {
        mov esi, CURRENT_SECTORS_Y
        cmp edi, esi
        jl toYLoop
        mov eax, [esp+0x14]
        inc eax
        mov esi, CURRENT_SECTORS_X
        cmp eax, esi
        jmp CStreaming__CStreaming__DeleteAllRwObjects4_CONTINUE

    toYLoop:
        jmp CStreaming__CStreaming__DeleteAllRwObjects4_LOOP_Y
    }
}

static void DeleteRwObjectsInSectorList(CPtrNodeDoubleListSAInterface<CEntitySAInterface> &list)
{
    auto nextNode = list.GetNode();
    while (nextNode)
    {
        if (!nextNode->pItem->bImBeingRendered && !nextNode->pItem->bStreamingDontDelete)
            nextNode->pItem->DeleteRwObject();
        nextNode = nextNode->pNext;
    }
}

static CRepeatSector* GetRepeatSector(int32 x, int32 y)
{
    return &DEFAULT_REPEAT_SECTORS[y % MAX_REPEAT_SECTORS_Y][x % MAX_REPEAT_SECTORS_X];
}

// Is this code ever used?
#define HOOKPOS_CStreaming__DeleteRwObjectsAfterDeath 0x409210
#define HOOKSIZE_CStreaming__DeleteRwObjectsAfterDeath 0x5

// Deletes all RW objects more than 3 sectors (on each axis) away from the given point's sector
static void HOOK_CStreaming__DeleteRwObjectsAfterDeath(const CVector2D& point)
{
    const std::int32_t pointSecX = instance->GetSectorX(point.fX);
    const std::int32_t pointSecY = instance->GetSectorY(point.fY);
    for (int32 sx = 0; sx < instance->GetSectorsX(); ++sx)
    {
        if (std::abs(pointSecX - sx) > 3)
        {
            for (int32 sy = 0; sy < instance->GetSectorsY(); ++sy)
            {
                if (std::abs(pointSecY - sy) > 3)
                {
                    CRepeatSector* repeatSector = GetRepeatSector(sx, sy);
                    CSector*       sector = instance->GetSector(sx, sy);
                    DeleteRwObjectsInSectorList(sector->m_buildings);
                    DeleteRwObjectsInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS));
                    DeleteRwObjectsInSectorList(sector->m_dummies);
                }
            }
        }
    }
}

// 0x40D7C0
// TODO: Decode this, no clue whats going on here..
void HOOK__CStreaming__DeleteRwObjectsBehindCamera(size_t memoryToCleanInBytes)
{
    uint32& ms_memoryUsedBytes = *reinterpret_cast<uint32*>(0x8E4CB4);
    auto    DeleteRwObjectsBehindCameraInSectorList = (bool(__cdecl*)(CPtrNodeDoubleListSAInterface<CEntitySAInterface>&, size_t))0x409940;
    auto    DeleteRwObjectsNotInFrustumInSectorList = (bool(__cdecl*)(CPtrNodeDoubleListSAInterface<CEntitySAInterface>&, size_t))0x4099E0;
    auto    CWorld__IncrementCurrentScanCode = (void (*)())0x4072E0;

    if (ms_memoryUsedBytes < memoryToCleanInBytes)
        return;

    const auto START_OFFSET_XY = 10;
    const auto END_OFFSET_XY = 2;

    const CVector&   cameraPos = TheCamera.GetPosition();
    const int32      pointSecX = instance->GetSectorX(cameraPos.x), pointSecY = instance->GetSectorY(cameraPos.y);
    const CVector2D& camFwd = TheCamera.GetForward();
    if (std::fabs(camFwd.y) < std::fabs(camFwd.x))
    {
        int32 sectorStartY = std::max(pointSecY - START_OFFSET_XY, 0);
        int32 sectorEndY = std::min(pointSecY + START_OFFSET_XY, MAX_SECTORS_Y - 1);
        int32 sectorStartX = 0;
        int32 sectorEndX = 0;
        int32 factorX = 0;

        if (camFwd.x <= 0.0f)
        {
            sectorStartX = std::min(pointSecX + START_OFFSET_XY, MAX_SECTORS_X - 1);
            sectorEndX = std::min(pointSecX + END_OFFSET_XY, MAX_SECTORS_X - 1);
            factorX = -1;
        }
        else
        {
            sectorStartX = std::max(pointSecX - START_OFFSET_XY, 0);
            sectorEndX = std::max(pointSecX - END_OFFSET_XY, 0);
            factorX = +1;
        }

        CWorld__IncrementCurrentScanCode();
        for (int32 sectorX = sectorStartX; sectorX != sectorEndX; sectorX += factorX)
        {
            for (int32 sectorY = sectorStartY; sectorY <= sectorEndY; sectorY++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsBehindCameraInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }

        if (camFwd.x <= 0.0f)
        {
            sectorEndX = std::min(pointSecX + END_OFFSET_XY, MAX_SECTORS_X - 1);
            sectorStartX = std::max(pointSecX - START_OFFSET_XY, 0);
            factorX = -1;
        }
        else
        {
            sectorEndX = std::max(pointSecX - END_OFFSET_XY, 0);
            sectorStartX = std::min(pointSecX + START_OFFSET_XY, MAX_SECTORS_X - 1);
            factorX = +1;
        }

        CWorld__IncrementCurrentScanCode();
        for (int32 sectorX = sectorStartX; sectorX != sectorEndX; sectorX -= factorX)
        {
            for (int32 sectorY = sectorStartY; sectorY <= sectorEndY; sectorY++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsNotInFrustumInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsNotInFrustumInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsNotInFrustumInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }

        CWorld__IncrementCurrentScanCode();
        for (int32 sectorX = sectorStartX; sectorX != sectorEndX; sectorX -= factorX)
        {
            for (int32 sectorY = sectorStartY; sectorY <= sectorEndY; sectorY++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsBehindCameraInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }
    }
    else
    {
        int32 sectorStartX = std::max(pointSecX - START_OFFSET_XY, 0);
        int32 sectorEndX = std::min(pointSecX + START_OFFSET_XY, MAX_SECTORS_X - 1);
        int32 sectorStartY = 0;
        int32 sectorEndY = 0;
        int32 factorY = 0;
        if (camFwd.y <= 0.0f)
        {
            sectorEndY = std::min(pointSecY + END_OFFSET_XY, MAX_SECTORS_Y - 1);
            sectorStartY = std::min(pointSecY + START_OFFSET_XY, MAX_SECTORS_Y - 1);
            factorY = -1;
        }
        else
        {
            sectorStartY = std::max(pointSecY - START_OFFSET_XY, 0);
            sectorEndY = std::max(pointSecY - END_OFFSET_XY, 0);
            factorY = +1;
        }
        CWorld__IncrementCurrentScanCode();
        for (int32 sectorY = sectorStartY; sectorY != sectorEndY; sectorY += factorY)
        {
            for (int32 sectorX = sectorStartX; sectorX <= sectorEndX; sectorX++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsBehindCameraInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }
        if (camFwd.y <= 0.0f)
        {
            sectorEndY = std::min(pointSecY + END_OFFSET_XY, MAX_SECTORS_Y - 1);
            sectorStartY = std::max(pointSecY - START_OFFSET_XY, 0);
            factorY = -1;
        }
        else
        {
            sectorEndY = std::max(pointSecY - END_OFFSET_XY, 0);
            sectorStartY = std::min(pointSecY + START_OFFSET_XY, MAX_SECTORS_Y - 1);
            factorY = +1;
        }
        CWorld__IncrementCurrentScanCode();
        for (int32 sectorY = sectorStartY; sectorY != sectorEndY; sectorY -= factorY)
        {
            for (int32 sectorX = sectorStartX; sectorX <= sectorEndX; sectorX++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsNotInFrustumInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsNotInFrustumInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsNotInFrustumInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }

        auto RemoveReferencedTxds = (bool(__cdecl*)(int32_t))0x40D2F0;
        if (RemoveReferencedTxds(memoryToCleanInBytes))
            return;

        // BUG: possibly missing CWorld::IncrementCurrentScanCode() here?
        for (int32 sectorY = sectorStartY; sectorY != sectorEndY; sectorY -= factorY)
        {
            for (int32 sectorX = sectorStartX; sectorX <= sectorEndX; sectorX++)
            {
                CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
                CSector*       sector = instance->GetSector(sectorX, sectorY);
                if (DeleteRwObjectsBehindCameraInSectorList(sector->m_buildings, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(sector->m_dummies, memoryToCleanInBytes) ||
                    DeleteRwObjectsBehindCameraInSectorList(repeatSector->GetList(REPEATSECTOR_OBJECTS), memoryToCleanInBytes))
                {
                    return;
                }
            }
        }
    }

    while (ms_memoryUsedBytes >= memoryToCleanInBytes)
    {
        auto RemoveLeastUsedModel = (bool(__cdecl*)(int32_t))0x40CFD0;
        if (!RemoveLeastUsedModel(0))
        {
            break;
        }
    }
}


void CEntityScanExtenter::Initialize()
{
    instance = std::make_unique<CEntityScanExtenter>();
    StaticSetHooks();
}

void CEntityScanExtenter::StaticSetHooks()
{
    EZHookInstall(CRenderer__SetupScanLists);
    EZHookInstall(CStreaming__AddModelsToRequestLis�1);
    EZHookInstall(CStreaming__AddModelsToRequestLis�2);
    EZHookInstall(CStreaming__DeleteAllRwObjects1);
    EZHookInstall(CStreaming__DeleteAllRwObjects2);
    EZHookInstall(CStreaming__DeleteAllRwObjects3);
    EZHookInstall(CStreaming__DeleteAllRwObjects4);
    EZHookInstall(CStreaming__DeleteRwObjectsAfterDeath);
    EZHookInstall(GetSector);
}