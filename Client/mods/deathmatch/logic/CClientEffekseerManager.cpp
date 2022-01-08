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

    // Init render
    auto pDevice = g_pCore->GetGraphics()->GetDevice();

    m_pRenderer = ::EffekseerRendererDX9::Renderer::Create(pDevice, EFK_MAX_PARTICLES);
    m_pRenderer->SetRestorationOfStatesFlag(false);

    m_pManagerInternal->SetSpriteRenderer(m_pRenderer->CreateSpriteRenderer());
    m_pManagerInternal->SetRibbonRenderer(m_pRenderer->CreateRibbonRenderer());
    m_pManagerInternal->SetRingRenderer(m_pRenderer->CreateRingRenderer());
    m_pManagerInternal->SetTrackRenderer(m_pRenderer->CreateTrackRenderer());
    m_pManagerInternal->SetModelRenderer(m_pRenderer->CreateModelRenderer());

    // Create loaders
    m_pManagerInternal->SetTextureLoader(m_pRenderer->CreateTextureLoader());
    m_pManagerInternal->SetModelLoader(m_pRenderer->CreateModelLoader());
    m_pManagerInternal->SetMaterialLoader(m_pRenderer->CreateMaterialLoader());
    m_pManagerInternal->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
    // We can init sound player for effects
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

void CClientEffekseerManager::Render()
{
    g_pCore->GetGraphics()->EnteringMTARenderZone();

    D3DMATRIX matrixProj, matrixView;
    auto pDevice = g_pCore->GetGraphics()->GetDevice();

    pDevice->GetTransform(D3DTS_PROJECTION, &matrixProj);
    pDevice->GetTransform(D3DTS_VIEW, &matrixView);

    ::Effekseer::Matrix44 effProjection;
    ::Effekseer::Matrix44 effView;

    for (int a = 0; a < 4; a++)
    {
        for (int b = 0; b < 4; b++)
        {
            effProjection.Values[a][b] = matrixProj.m[a][b];
            effView.Values[a][b] = matrixView.m[a][b];
        }
    }

    m_pRenderer->SetProjectionMatrix(effProjection);
    m_pRenderer->SetCameraMatrix(effView);

    m_pManagerInternal->Update();

    m_pRenderer->BeginRendering();

    pDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pManagerInternal->Draw();
    m_pRenderer->EndRendering();

    g_pCore->GetGraphics()->LeavingMTARenderZone();
}
