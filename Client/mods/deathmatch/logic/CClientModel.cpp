/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModel.h
 *  PURPOSE:     Model handling class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientModel::CClientModel(CClientManager* pManager, int iModelID, eClientModelType eModelType)
{
    // Init
    m_pManager = pManager;
    m_pModelManager = pManager->GetModelManager();
    m_iModelID = iModelID;
    m_eModelType = eModelType;

    m_pModelManager->Add(this);
}

CClientModel::~CClientModel(void)
{
    Deallocate();

    m_pModelManager->Remove(this);
}

bool CClientModel::Allocate(void)
{
    m_bAllocatedByUs = true;

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);

    // Allocate only on free IDs
    if (!pModelInfo->IsValid())
    {
        switch (m_eModelType)
        {
            case eClientModelType::PED:
                pModelInfo->MakePedModel("PSYCHO");
                break;
            case eClientModelType::OBJECT:
                pModelInfo->MakeObjectModel(1337);
                break;
            case eClientModelType::VEHICLE_AUTOMOBILE:
                pModelInfo->MakeVehicleAutomobile(400);
                break;
            default:
                return false;
        }
        return true;
    }
    return false;
}

bool CClientModel::Deallocate(void)
{
    if (!m_bAllocatedByUs)
        return false;

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);

    // ModelInfo must be valid
    if (pModelInfo->IsValid())
    {
        if (m_eModelType == eClientModelType::PED)
        {
            // If some ped is using this ID, change him to CJ
            CClientPedManager*                       pPedManager = g_pClientGame->GetManager()->GetPedManager();
            for (auto iter = pPedManager->IterBegin(); iter != pPedManager->IterEnd(); iter++)
            {
                CClientPed* pPed = *iter;
                if (pPed->GetModel() == m_iModelID)
                {
                    if (pPed->IsStreamedIn())
                    {
                        pPed->StreamOutForABit();
                    }
                    pPed->SetModel(0);
                }
            }
        }
        else if (m_eModelType == eClientModelType::OBJECT)
        {
            CClientObjectManager* pObjectManager = g_pClientGame->GetManager()->GetObjectManager();
            for (auto iter = pObjectManager->IterBegin(); iter != pObjectManager->IterEnd(); iter++)
            {
                CClientObject* pObject = *iter;
                if (pObject->GetModel() == m_iModelID)
                {
                    if (pObject->IsStreamedIn())
                    {
                        pObject->StreamOutForABit();
                    }
                    pObject->SetModel(1337);
                }
            }
        }

        // Restore DFF/TXD
        g_pClientGame->GetManager()->GetDFFManager()->RestoreModel(m_iModelID);

        // Restore COL (for non ped models)
        if (m_eModelType != eClientModelType::PED)
        {
            g_pClientGame->GetManager()->GetColModelManager()->RestoreModel(m_iModelID);
        }

        pModelInfo->DeallocateModel();

        this->SetParentResource(nullptr);
        return true;
    }
    return false;
}
