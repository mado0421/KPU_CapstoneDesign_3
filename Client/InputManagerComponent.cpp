#include "stdafx.h"
#include "Components.h"

InputManagerComponent::InputManagerComponent(Object* pObject)
	:Component(pObject)
	, m_xmf2MouseMovement(0, 0)
{
}

InputManagerComponent::~InputManagerComponent()
{
}

void InputManagerComponent::InputEvent(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement)
{
	memcpy(m_pKeysBuffer, pKeyBuffer, sizeof(UCHAR) * 256);
	m_xmf2MouseMovement = xmf2MouseMovement;
}

void InputManagerComponent::Update(float fTimeElapsed)
{
}

bool InputManagerComponent::IsKeyDown(KeyCode key)
{
	return (m_pKeysBuffer[key] & 0xF0);
}

bool InputManagerComponent::IsKeyUp(KeyCode key)
{
	return !(m_pKeysBuffer[key] & 0xF0);
}

const XMFLOAT2& InputManagerComponent::GetMouseMovement()
{
	return m_xmf2MouseMovement;
}
