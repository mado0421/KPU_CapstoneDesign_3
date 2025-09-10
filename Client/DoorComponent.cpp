#include "stdafx.h"
#include "Components.h"

DoorComponent::DoorComponent(Object* pObject, bool bOpen)
	: Component(pObject)
	  , m_bOpen(bOpen)
	  , m_fProgress(0)
	  , m_xmf3OrigPosition(pObject->FindComponent<TransformComponent>()->GetPosition())
{
	if (m_bOpen) m_fProgress = 1;
	else m_fProgress = 0;

	l_transform = m_pObject->FindComponent<TransformComponent>();
}

DoorComponent::~DoorComponent()
{
}

void DoorComponent::Open()
{
	m_bOpen = true;
}

void DoorComponent::Close()
{
	m_bOpen = false;
}

void DoorComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	if (m_bOpen)
	{
		if (m_fProgress < 1) m_fProgress += fTimeElapsed;
		else m_fProgress = 1;
	}
	else
	{
		if (m_fProgress > 0) m_fProgress -= fTimeElapsed;
		else m_fProgress = 0;
	}

	XMFLOAT3 xmf3Temp = l_transform->GetRightVector();
	xmf3Temp = Vector3::Add(m_xmf3OrigPosition, Vector3::Multiply(m_fProgress * 3/*m*/, xmf3Temp));
	l_transform->SetPosition(xmf3Temp);
}
