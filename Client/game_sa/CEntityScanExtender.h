/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "CPtrNodeDoubleListSA.h"
#include "CBuildingSA.h"
#include <memory>

struct CSector
{
    CPtrNodeDoubleListSAInterface<CEntitySAInterface> m_buildings;
    CPtrNodeDoubleListSAInterface<CEntitySAInterface> m_dummies;

    void Swap(CSector& another)
    {
        auto buildins = m_buildings;
        auto dummies = m_dummies;
        m_buildings = another.m_buildings;
        m_dummies = another.m_dummies;

        another.m_buildings = buildins;
        another.m_dummies = dummies;
    }
};

class CEntityScanExtenter
{
public:
    CEntityScanExtenter();

    bool     IsInWorldSector(std::int32_t x, std::int32_t y) const noexcept;
    bool     IsInWorldPosition(const CVector& pos) const noexcept;
    CSector* GetSector(std::int32_t x, std::int32_t y) const noexcept;

    void ResizeForPosition(const CVector& pos);

    std::uint32_t GetSectorNum(std::uint32_t x, std::uint32_t y) const noexcept;


    std::int32_t GetSectorX(float x) const noexcept;
    std::int32_t GetSectorY(float x) const noexcept;

    float GetSectorPosX(std::int32_t) const noexcept;

    std::uint32_t GetSectorsX() const noexcept;
    std::uint32_t GetSectorsY() const noexcept;

private:
    std::int32_t RoundSectorForResize(std::int32_t sector) const noexcept;
    void         Resize(std::size_t sectorsX, std::size_t sectorsY);

    void PatchOnce();
    void PatchDynamic();
    void SetHooks();
    void Reset();
    void CalculateWorldValiables(std::size_t sectorsX, std::size_t sectorsY);

private:
    std::size_t m_SectorsW;
    std::size_t m_SectorsH;

    float m_halfSectorsX;
    float m_halfSectorsY;

    float m_woldLeft;
    float m_woldRight;
    float m_woldTop;
    float m_woldBottom;
};
