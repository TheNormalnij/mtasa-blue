/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include <vector>
#include "CPtrNodeDoubleListSA.h"
#include "CBuildingSA.h"
#include <memory>

constexpr auto MAX_SECTORS_Y = 120;
constexpr auto MAX_SECTORS_X = 120;

struct CSector
{
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*> m_buildings;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface*> m_dummies;
};

class CEntityScanExtenter
{
public:
    CEntityScanExtenter();

    bool           IsInWorldSector(std::int32_t x, std::int32_t y) const noexcept;
    CSector*       GetSector(std::uint32_t x, std::uint32_t y) const noexcept;

    CSector* GetSectorResize(std::uint32_t x, std::uint32_t y);
    void           Resize(std::size_t count);

    static void StaticSetHooks();

    std::int32_t GetSectorsX() const noexcept { return m_sectorsX; };
    std::int32_t GetSectorsY() const noexcept { return m_sectorsY; };

private:
    std::int32_t m_sectorsX{120};
    std::int32_t m_sectorsY{120};
    std::size_t  m_SectorsHalfW{3000};
    std::size_t  m_SectorsHalfH{3000};
};
