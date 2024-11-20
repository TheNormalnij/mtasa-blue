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
#include "CCameraSA.h"

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
static CSector       (&DEFAULT_SECTORS)[120][120] = *(CSector(*)[120][120])0xB7D0B8;
static CSector*      CURRENT_SECTORS = reinterpret_cast<CSector*>(DEFAULT_SECTORS);
static auto*         SCAN_LIST = (tScanLists*)0xC8E0C8;

static CEntityScanExtenter* instance;

static std::uint32_t CURRENT_SECTORS_X = 120;
static std::uint32_t CURRENT_SECTORS_Y = 120;
static std::int32_t CURRENT_SECTORS_X_MINUS_ONE = 119;
static std::int32_t CURRENT_SECTORS_Y_MINUS_ONE = 119;

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
    SetHooks();
    // Only for tests
    Resize(120, 120);
}

bool CEntityScanExtenter::IsInWorldSector(std::int32_t x, std::int32_t y) const noexcept
{
    return x >= 0 && x <= (std::int32_t)GetSectorsX() && y >= 0 && y <= (std::int32_t)GetSectorsY();
}

CSector* CEntityScanExtenter::GetSector(std::uint32_t x, std::uint32_t y) const noexcept
{
    return &CURRENT_SECTORS[y * CURRENT_SECTORS_X + x];
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

    // TODO
    return GetSector(x, y);
}

std::uint32_t CEntityScanExtenter::GetSectorNumResize(std::int32_t x, std::int32_t y)
{
    x = clamp(x, 0, CURRENT_SECTORS_X);
    y = clamp(y, 0, CURRENT_SECTORS_Y);

    if (IsInWorldSector(x, y))
        return GetSectorNum(x, y);

    // TODO
    return GetSectorNum(x, y);
}

std::uint32_t CEntityScanExtenter::GetSectorNum(std::uint32_t x, std::uint32_t y)
{
    return y * CURRENT_SECTORS_X + x;
}

void CEntityScanExtenter::Resize(std::size_t sectorsX, std::size_t sectorsY)
{
    for (auto x = 0; x < CURRENT_SECTORS_X; x++)
        for (auto y = 0; y < CURRENT_SECTORS_Y; y++)
        {
            CURRENT_SECTORS->m_buildings.RemoveAllItems();
            CURRENT_SECTORS->m_dummies.RemoveAllItems();
        }

    if (CURRENT_SECTORS != reinterpret_cast<CSector*>(DEFAULT_SECTORS))
        delete[] CURRENT_SECTORS;

    CURRENT_SECTORS = new CSector[sectorsY * sectorsX];

    CURRENT_SECTORS_X = sectorsX;
    CURRENT_SECTORS_Y = sectorsY;

    CURRENT_SECTORS_X_MINUS_ONE = sectorsX - 1;
    CURRENT_SECTORS_Y_MINUS_ONE = sectorsY - 1;

    m_SectorsW = sectorsX * 50;
    m_SectorsH = sectorsY * 50;

    m_halfSectorsX = sectorsX / 2.0f;
    m_halfSectorsY = sectorsY / 2.0f;

    m_woldLeft = -(m_SectorsW / 2.0f);
    m_woldRight = m_SectorsW / 2.0f;
    m_woldTop = -(m_SectorsH / 2.0f);
    m_woldBottom = m_SectorsH / 2.0f;

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

    // CStreaming::InstanceLoadedModels
    MemPut(0x40853C + 2, &m_halfSectorsX);
    MemPut(0x408555 + 2, &m_halfSectorsY);
    MemPut(0x40856F + 2, &m_halfSectorsX);
    MemPut(0x4085C5 + 2, &m_halfSectorsY);
    MemPut(0x40860D + 2, &m_halfSectorsX);
    MemPut(0x40865E + 2, &m_halfSectorsY);

    // CTaskSimpleClimb::ScanToGrab
    MemPut(0x67FE38 + 2, &m_halfSectorsX);
    MemPut(0x67FE5F + 2, &m_halfSectorsY);
    MemPut(0x67FE84 + 2, &m_halfSectorsX);
    MemPut(0x67FEAB + 2, &m_halfSectorsY);

    // CWorld::FindObjectsOfTypeInRange
    MemPut(0x564C8A + 2, &m_halfSectorsX);
    MemPut(0x564CB0 + 2, &m_halfSectorsY);
    MemPut(0x564CD1 + 2, &m_halfSectorsX);
    MemPut(0x564CF5 + 2, &m_halfSectorsY);
}

void CEntityScanExtenter::PatchDynamic()
{
    // CEntity::Add(CRect)
    MemPut(0x53480D + 4, m_woldLeft);
    MemPut(0x534826 + 4, m_woldRight - 1.f);
    MemPut(0x53483F + 4, m_woldTop);
    MemPut(0x534858 + 4, m_woldBottom - 1.f);
    MemPut(0x534A09 + 3, CURRENT_SECTORS);
    MemPut(0x534A98 + 3, reinterpret_cast<std::uint32_t>(CURRENT_SECTORS) + 4);

    // CEntity::Remove
    MemPut(0x534B47 + 4, m_woldRight - 1.f);
    MemPut(0x534B60 + 4, m_woldTop);
    MemPut(0x534B79 + 4, m_woldBottom - 1.f);

    // CStreaming::AddModelsToRequestList

	MemPut(0x40D547 + 1, std::uint32_t(m_halfSectorsX - 1.f));

    MemPut(0x40D68C + 3, CURRENT_SECTORS);

    // CStreaming::DeleteAllRwObjects

    MemPut(0x40914E + 3, CURRENT_SECTORS);
    MemPut(0x4091C5 + 3, CURRENT_SECTORS);

    // CStreaming::InstanceLoadedModels
    MemPut(0x408627 + 1, CURRENT_SECTORS_X_MINUS_ONE);
    MemPut(0x4086FF + 3, CURRENT_SECTORS);
    MemPut(0x408706 + 3, CURRENT_SECTORS);

    // CTaskSimpleClimb::ScanToGrab
    MemPut(0x67FF5D + 3, CURRENT_SECTORS);

    // CWorld::FindObjectsOfTypeInRange
    MemPut(0x564DA9 + 3, CURRENT_SECTORS);

    // CCollision::CheckCameraCollisionBuildings
    MemPut(0x41A85A + 3, CURRENT_SECTORS);
    MemPut(0x41A861 + 3, CURRENT_SECTORS);

    // CWorld::ClearScanCodes
    MemPut(0x408258 + 1, CURRENT_SECTORS);

    // CWorld::RemoveStaticObjects
    MemPut(0x563844 + 1, CURRENT_SECTORS);

    // CWorld::ShutDown
    MemPut(0x563844 + 1, CURRENT_SECTORS);

    // CPhysical::ProcessCollisionSectorList
    MemPut(0x54BB23 + 3, CURRENT_SECTORS);
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

#define HOOKPOS_CStreaming__AddModelsToRequestLiså1  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLiså1 0x5
static std::uint32_t CStreaming__AddModelsToRequestList1_CONTINUE = 0x40D578;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLiså1()
{
    _asm {
			mov ebp, CURRENT_SECTORS_Y_MINUS_ONE
			fld dword ptr [esp+0x14] ; original code
			sub esp, 8
			jmp CStreaming__AddModelsToRequestList1_CONTINUE
    }
}

#define HOOKPOS_CStreaming__AddModelsToRequestLiså2  0x40D66B
#define HOOKSIZE_CStreaming__AddModelsToRequestLiså2 0x5
static std::uint32_t CStreaming__AddModelsToRequestList2_CONTINUE = 0x40D686;
void __declspec(naked) HOOK_CStreaming__AddModelsToRequestLiså2()
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


// Untested need a way to trigger this function
#define HOOKPOS_CStreaming__DeleteRwObjectsBehindCamera  0x40D7C0
#define HOOKSIZE_CStreaming__DeleteRwObjectsBehindCamera 0x5

// TODO: Decode this, no clue whats going on here..
void HOOK_CStreaming__DeleteRwObjectsBehindCamera(size_t memoryToCleanInBytes)
{
    // our defenitions
    uint32& ms_memoryUsedBytes = *reinterpret_cast<uint32*>(0x8E4CB4);
    auto    DeleteRwObjectsBehindCameraInSectorList = (bool(__cdecl*)(CPtrNodeDoubleListSAInterface<CEntitySAInterface>&, size_t))0x409940;
    auto    DeleteRwObjectsNotInFrustumInSectorList = (bool(__cdecl*)(CPtrNodeDoubleListSAInterface<CEntitySAInterface>&, size_t))0x4099E0;
    auto    CWorld__IncrementCurrentScanCode = (void (*)())0x4072E0;
    auto    RemoveLeastUsedModel = (bool(__cdecl*)(int32_t))0x40CFD0;
    auto    RemoveReferencedTxds = (bool(__cdecl*)(int32_t))0x40D2F0;
    auto & TheCamera = *reinterpret_cast<CCameraSAInterface*>(0xB6F028);
    // mta-reverse-modern code

    if (ms_memoryUsedBytes < memoryToCleanInBytes)
        return;

    const auto START_OFFSET_XY = 10;
    const auto END_OFFSET_XY = 2;

    const CVector&   cameraPos = TheCamera.GetPosition();
    const int32      pointSecX = instance->GetSectorX(cameraPos.fX), pointSecY = instance->GetSectorY(cameraPos.fY);
    const CVector2D& camFwd = TheCamera.GetForward();
    if (std::fabs(camFwd.fY) < std::fabs(camFwd.fX))
    {
        int32 sectorStartY = std::max(pointSecY - START_OFFSET_XY, 0);
        int32 sectorEndY = std::min(pointSecY + START_OFFSET_XY, CURRENT_SECTORS_Y_MINUS_ONE);
        int32 sectorStartX = 0;
        int32 sectorEndX = 0;
        int32 factorX = 0;

        if (camFwd.fX <= 0.0f)
        {
            sectorStartX = std::min(pointSecX + START_OFFSET_XY, CURRENT_SECTORS_X_MINUS_ONE);
            sectorEndX = std::min(pointSecX + END_OFFSET_XY, CURRENT_SECTORS_X_MINUS_ONE);
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

        if (camFwd.fX <= 0.0f)
        {
            sectorEndX = std::min(pointSecX + END_OFFSET_XY, CURRENT_SECTORS_X_MINUS_ONE);
            sectorStartX = std::max(pointSecX - START_OFFSET_XY, 0);
            factorX = -1;
        }
        else
        {
            sectorEndX = std::max(pointSecX - END_OFFSET_XY, 0);
            sectorStartX = std::min(pointSecX + START_OFFSET_XY, CURRENT_SECTORS_X_MINUS_ONE);
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
        int32 sectorEndX = std::min(pointSecX + START_OFFSET_XY, CURRENT_SECTORS_X_MINUS_ONE);
        int32 sectorStartY = 0;
        int32 sectorEndY = 0;
        int32 factorY = 0;
        if (camFwd.fY <= 0.0f)
        {
            sectorEndY = std::min(pointSecY + END_OFFSET_XY, CURRENT_SECTORS_Y_MINUS_ONE);
            sectorStartY = std::min(pointSecY + START_OFFSET_XY, CURRENT_SECTORS_Y_MINUS_ONE);
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
        if (camFwd.fY <= 0.0f)
        {
            sectorEndY = std::min(pointSecY + END_OFFSET_XY, CURRENT_SECTORS_Y_MINUS_ONE);
            sectorStartY = std::max(pointSecY - START_OFFSET_XY, 0);
            factorY = -1;
        }
        else
        {
            sectorEndY = std::max(pointSecY - END_OFFSET_XY, 0);
            sectorStartY = std::min(pointSecY + START_OFFSET_XY, CURRENT_SECTORS_Y_MINUS_ONE);
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
        if (!RemoveLeastUsedModel(0))
        {
            break;
        }
    }
}

// untested

#define HOOKPOS_CStreaming__InstanceLoadedModels1  0x408651
#define HOOKSIZE_CStreaming__InstanceLoadedModels1 0x7
static std::uint32_t CStreaming__InstanceLoadedModels1_CONTINUE = 0x408658;
void __declspec(naked) HOOK_CStreaming__InstanceLoadedModels1()
{
    _asm {
        mov edi, CURRENT_SECTORS_Y_MINUS_ONE
        fld [esp+0x18]
        sub esp, 8
        jmp CStreaming__InstanceLoadedModels1_CONTINUE
    }
}

#define HOOKPOS_CStreaming__InstanceLoadedModels2  0x4086E2
#define HOOKSIZE_CStreaming__InstanceLoadedModels2 0x5
static std::uint32_t CStreaming__InstanceLoadedModels2_CONTINUE = 0x4086FD;
void __declspec(naked) HOOK_CStreaming__InstanceLoadedModels2()
{
    _asm {
            mov esi, CURRENT_SECTORS_X_MINUS_ONE
            cmp ecx, esi
            jl nextCheck
            mov ecx, esi
        nextCheck:
            mov esi, CURRENT_SECTORS_Y_MINUS_ONE
            mov eax, [esp+0x20]
            cmp eax, esi
            jl calcTable
            mov eax, esi
        calcTable:
            inc esi
            imul eax, esi
            jmp CStreaming__InstanceLoadedModels2_CONTINUE
    }
}

#define HOOKPOS_CTaskSimpleClimb__ScanToGrab  0x67FF40
#define HOOKSIZE_CTaskSimpleClimb__ScanToGrab 0x5
static std::uint32_t CTaskSimpleClimb__ScanToGrab_CONTINUE = 0x67FF5B;
void __declspec(naked) HOOK_CTaskSimpleClimb__ScanToGrab()
{
    _asm {
            mov edx, CURRENT_SECTORS_X_MINUS_ONE
            cmp ecx, edx
            jl nextCheck
            mov ecx, edx
        nextCheck:
            mov edx, CURRENT_SECTORS_Y_MINUS_ONE
            mov eax, [esp+0x38]
            cmp eax, edx
            jl calcTable
            mov eax, edx
        calcTable:
            inc edx
            imul eax, edx
            jmp CTaskSimpleClimb__ScanToGrab_CONTINUE
    }
}

// untested (unused?)
#define HOOKPOS_CWorld__FindObjectsOfTypeInRange  0x564D8C
#define HOOKSIZE_CWorld__FindObjectsOfTypeInRange 0x5
static std::uint32_t CWorld__FindObjectsOfTypeInRange_CONTINUE = 0x564DA7;
void __declspec(naked) HOOK_CWorld__FindObjectsOfTypeInRange()
{
    _asm {
            mov esi, CURRENT_SECTORS_X_MINUS_ONE
            cmp ecx, esi
            jl nextCheck
            mov ecx, esi
        nextCheck:
            mov esi, CURRENT_SECTORS_Y_MINUS_ONE
            mov eax, [esp+0x18]
            cmp eax, esi
            jl calcTable
            mov eax, esi
        calcTable:
            inc esi
            imul eax, esi
            jmp CWorld__FindObjectsOfTypeInRange_CONTINUE
    }
}

#define HOOKPOS_CCollision__CheckCameraCollisionBuildings  0x41A831
#define HOOKSIZE_CCollision__CheckCameraCollisionBuildings 0x5
void __declspec(naked) HOOK_CCollision__CheckCameraCollisionBuildings()
{
    _asm {
            mov edx, CURRENT_SECTORS_X_MINUS_ONE
            cmp ecx, edx
            jl next
            mov ecx, edx
        next:
            mov eax, [esp+0x28] ; original code
            xor edx, edx
            test eax, eax
            setle dl
            dec edx
            and eax, edx ; original code end

            mov edx, CURRENT_SECTORS_Y_MINUS_ONE
            cmp eax, edx
            jl calcTable
            mov eax, edx
        calcTable:
            inc edx
            imul eax, edx
            JMP_ABSOLUTE_ASM(0x41A856)
    }
}

static std::uint32_t __cdecl GetArrayPositionResize(std::int32_t x, std::int32_t y)
{
    return instance->GetSectorNumResize(x, y);
}

#define HOOKPOS_CEntity__Add1  0x5349DB
#define HOOKSIZE_CEntity__Add1 0x7
void __declspec(naked) HOOK_CEntity__Add1()
{
    _asm {
        push edi ; y
        push esi ; x
        call GetArrayPositionResize
        add  esp, 4*2
        JMP_ABSOLUTE_ASM(0x534A08)
    }
}

#define HOOKPOS_CEntity__Add2  0x534A6B
#define HOOKSIZE_CEntity__Add2 0x7
void __declspec(naked) HOOK_CEntity__Add2()
{
    _asm {
        push eax
        push ecx
        call GetArrayPositionResize
        add  esp, 4*2
        JMP_ABSOLUTE_ASM(0x534A98)
    }
}

#define HOOKPOS_CPhysical__ProcessCollisionSectorLists  0x54BAF6
#define HOOKSIZE_CPhysical__ProcessCollisionSectorLists 0x5
void __declspec(naked) HOOK_CPhysical__ProcessCollisionSectorLists()
{
    _asm {
            mov eax, CURRENT_SECTORS_X_MINUS_ONE
            cmp edi, eax
            jl next
            mov edi, eax
        next:
            mov eax, [esp+0x1F8]
            xor ecx, ecx
            cmp eax, ebx
            setle cl
            dec ecx
            and ecx, eax
            mov ebx, CURRENT_SECTORS_Y_MINUS_ONE
            cmp ecx, ebx
            jl calcTable
            mov ecx, ebx
        calcTable:
            inc ebx
            imul ecx, ebx
            xor ebx, ebx
            JMP_ABSOLUTE_ASM(0x54BB1E)
    }
}

// CPhysical::ProcessShiftSectorList
// CEntity__Remove

void CEntityScanExtenter::SetHooks()
{
    instance = this;
    EZHookInstall(CEntity__Add1);
    EZHookInstall(CEntity__Add2);
    EZHookInstall(CRenderer__SetupScanLists);
    EZHookInstall(CStreaming__AddModelsToRequestLiså1);
    EZHookInstall(CStreaming__AddModelsToRequestLiså2);
    EZHookInstall(CStreaming__DeleteAllRwObjects1);
    EZHookInstall(CStreaming__DeleteAllRwObjects2);
    EZHookInstall(CStreaming__DeleteAllRwObjects3);
    EZHookInstall(CStreaming__DeleteAllRwObjects4);
    EZHookInstall(CStreaming__DeleteRwObjectsAfterDeath);
    EZHookInstall(CStreaming__DeleteRwObjectsBehindCamera);
    EZHookInstall(CStreaming__InstanceLoadedModels1);
    EZHookInstall(CStreaming__InstanceLoadedModels2);
    EZHookInstall(CTaskSimpleClimb__ScanToGrab);
    EZHookInstall(CWorld__FindObjectsOfTypeInRange);
    EZHookInstall(CCollision__CheckCameraCollisionBuildings);
    EZHookInstall(CPhysical__ProcessCollisionSectorLists);
    EZHookInstall(GetSector);
}
