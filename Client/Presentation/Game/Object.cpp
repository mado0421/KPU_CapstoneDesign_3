#include "pch.h"
#include "Object.h"
#include "Presentation/Game/Component/Components.h"

Object::Object() : is_enable_(true), parent_(nullptr)
{
}

Object::Object(const char* name) : name_(name), is_enable_(true), parent_(nullptr)
{
}

Object::~Object()
{
	ranges::for_each(components_, [](const Component* c) { delete c; });
}

void Object::CheckCollision(Object* other)
{
	ranges::for_each(components_, [&](Component* c)
	{
		vector<ColliderComponent*> colliders = other->GetComponents<ColliderComponent>();
		ranges::for_each(colliders, [&](ColliderComponent* collider) { c->CheckCollision(collider); });
	});
}

void Object::SolveConstraint() { ranges::for_each(components_, [](Component* c) { c->SolveConstraint(); }); }

void Object::Input(UCHAR* key_buffer, XMFLOAT2& mouse_movement)
{
	if (InputManagerComponent* input_manager = GetComponent<InputManagerComponent>(); nullptr != input_manager)
		input_manager->InputEvent(key_buffer, mouse_movement);
}

void Object::Update(const float delta_time)
{
	ranges::for_each(components_, [&](Component* c) { c->Update(delta_time); });
}

void Object::Render(ID3D12GraphicsCommandList* command_list)
{
	ranges::for_each(components_, [&](Component* c) { c->Render(command_list); });
}

void Object::SetActive(const bool is_active)
{
	is_enable_ = is_active;
	ranges::for_each(components_, [&](Component* c) { c->SetActive(is_active); });
}


void Object::AddComponent(Component* component) { components_.push_back(component); }
