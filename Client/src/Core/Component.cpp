#include "pch.h"
#include "Component.h"
#include "Object.h"


Component::Component(Object* pObject) : m_pObject(pObject) { pObject->AddComponent(this); }

Component::~Component() {}

Component* Component::GetInstance() { return this; }

void Component::SetActive(bool bActive) { m_bEnabled = bActive; }
