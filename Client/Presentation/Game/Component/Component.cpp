#include "pch.h"
#include "Component.h"

#include "Presentation/Game/Object.h"


Component::Component(Object* pObject) : object(pObject) { pObject->AddComponent(this); }

Component::~Component() {}

Component* Component::GetInstance() { return this; }

void Component::SetActive(bool bActive) { is_enable = bActive; }
