/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientEffekseerEffectHandler.cpp
 *  PURPOSE:     Effekseer handle class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientEffekseerEffectHandler::CClientEffekseerEffectHandler(CClientManager* pManager, ElementID ID, Effekseer::Handle handle) : ClassInit(this), CClientEntity(ID)
{
    SetTypeName("effekseer-handler");

    m_Handle = handle;
    m_pManager = pManager;
    m_pInternalManager = pManager->GetEffekseerManager()->GetInternalManager();
}

CClientEffekseerEffectHandler::~CClientEffekseerEffectHandler()
{
    Stop();
}

void CClientEffekseerEffectHandler::SetPosition(const CVector& vecPosition)
{
    m_vecPosition = vecPosition;
    m_pInternalManager->SetLocation(m_Handle, ::Effekseer::Vector3D(vecPosition.fX, vecPosition.fY, vecPosition.fZ));
}

void CClientEffekseerEffectHandler::SetRotation(const CVector& vecRotation)
{
    m_pInternalManager->SetRotation(m_Handle, vecRotation.fX, vecRotation.fY, vecRotation.fZ);
}

void CClientEffekseerEffectHandler::SetScale(const CVector& vecScale)
{
    m_pInternalManager->SetScale(m_Handle, vecScale.fX, vecScale.fY, vecScale.fZ);
}

void CClientEffekseerEffectHandler::Stop()
{
    m_pInternalManager->StopEffect(m_Handle);
}

void CClientEffekseerEffectHandler::StopRoot()
{
    m_pInternalManager->StopRoot(m_Handle);
}

void CClientEffekseerEffectHandler::SetSpeed(float fSpeed)
{
    m_pInternalManager->SetSpeed(m_Handle, fSpeed);
}

float CClientEffekseerEffectHandler::GetSpeed()
{
    return m_pInternalManager->GetSpeed(m_Handle);
}

void CClientEffekseerEffectHandler::SetDynamicInput(int32_t index, float fValue)
{
    m_pInternalManager->SetDynamicInput(m_Handle, index, fValue);
}

float CClientEffekseerEffectHandler::GetDynamicInput(int32_t index)
{
    return m_pInternalManager->GetDynamicInput(m_Handle, index);
}
