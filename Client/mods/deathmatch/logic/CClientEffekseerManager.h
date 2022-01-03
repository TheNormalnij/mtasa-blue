/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPedManager.h
 *  PURPOSE:     Ped entity manager class
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "CClientEffekseerEffect.h"
#include "Effekseer.h"
#include "EffekseerRendererDX9.h"

class CClientManager;
class CEntity;

class CClientEffekseerManager
{
    friend class CClientManager;

public:
    void DeleteAll();

    void                    Render();
    CClientEffekseerEffect* Get(ElementID ID);
    Effekseer::ManagerRef   GetInternalManager() { return m_pManagerInternal; };

protected:
    CClientEffekseerManager(class CClientManager* pManager);
    ~CClientEffekseerManager();

    void AddToList(CClientEffekseerEffect* pEffekseerFX) { m_List.push_back(pEffekseerFX); }
    // void RemoveFromList(CClientEffekseerEffect* pEffekseerFX);

    // void OnCreation(CClientEffekseerEffect* pEffekseerFX);
    // void OnDestruction(CClientEffekseerEffect* pEffekseerFX);

    CClientManager*                      m_pManager;
    std::vector<CClientEffekseerEffect*> m_List;
    EffekseerRendererDX9::RendererRef    m_pRenderer;
    Effekseer::ManagerRef                m_pManagerInternal;
    bool                                 m_bCanRemoveFromList;
};
