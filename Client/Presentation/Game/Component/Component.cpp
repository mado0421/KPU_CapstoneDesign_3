#include "pch.h"
#include "Component.h"

#include "Presentation/Game/GameObject.h"


Component::Component(GameObject* game_object) : game_object_(game_object) { game_object->AddComponent(this); }

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

void Component::SetActive(const bool is_active) { is_enable_ = is_active; }
