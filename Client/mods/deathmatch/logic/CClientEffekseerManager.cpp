/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerManager.cpp
 *  PURPOSE:     Marker entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#define EFK_MAX_PARTICLES 8000

CClientEffekseerManager::CClientEffekseerManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
    m_pManagerInternal = ::Effekseer::Manager::Create(EFK_MAX_PARTICLES);
}

CClientEffekseerManager::~CClientEffekseerManager()
{
    // Make sure all effects are deleted
    DeleteAll();
}

CClientEffekseerEffect* CClientEffekseerManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTEFFEKSEERFX)
    {
        return static_cast<CClientEffekseerEffect*>(pEntity);
    }

    return NULL;
}

void CClientEffekseerManager::DeleteAll()
{
    // Delete each effect
    m_bCanRemoveFromList = false;
    auto iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    m_bCanRemoveFromList = true;

    // Clear the list
    m_List.clear();
}
