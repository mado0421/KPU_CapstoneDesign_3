#include "pch.h"
#include "Component.h"

#include "Presentation/Game/Object.h"


Component::Component(Object* object) : object(object) { object->AddComponent(this); }

Component::~Component() = default;

void Component::CheckCollision(Component*)
{
}

void Component::SolveConstraint()
{
}

void Component::Update(float)
{
}

void Component::Render(ID3D12GraphicsCommandList*)
{
}

void Component::SetActive(const bool is_active) { is_enable = is_active; }
