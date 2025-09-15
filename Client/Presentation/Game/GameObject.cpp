#include "pch.h"
#include "GameObject.h"
#include "Presentation/Game/Component/Components.h"

GameObject::GameObject() : is_enable_(true), parent_(nullptr)
{
}

GameObject::GameObject(const char* name) : name_(name), is_enable_(true), parent_(nullptr)
{
}

GameObject::~GameObject()
{
	ranges::for_each(components_, [](const Component* c) { delete c; });
}

void GameObject::CheckCollision(GameObject* other)
{
	ranges::for_each(components_, [&](Component* c)
	{
		vector<ColliderComponent*> colliders = other->GetComponents<ColliderComponent>();
		ranges::for_each(colliders, [&](ColliderComponent* collider) { c->CheckCollision(collider); });
	});
}

void GameObject::SolveConstraint() { ranges::for_each(components_, [](Component* c) { c->SolveConstraint(); }); }

void GameObject::Input(UCHAR* key_buffer, XMFLOAT2& mouse_movement)
{
	if (InputManagerComponent* input_manager = GetComponent<InputManagerComponent>(); nullptr != input_manager)
		input_manager->InputEvent(key_buffer, mouse_movement);
}

void GameObject::Update(const float delta_time)
{
	ranges::for_each(components_, [&](Component* c) { c->Update(delta_time); });
}

void GameObject::Render(ID3D12GraphicsCommandList* command_list)
{
	ranges::for_each(components_, [&](Component* c) { c->Render(command_list); });
}

void GameObject::SetActive(const bool is_active)
{
	is_enable_ = is_active;
	ranges::for_each(components_, [&](Component* c) { c->SetActive(is_active); });
}


void GameObject::AddComponent(Component* component) { components_.push_back(component); }