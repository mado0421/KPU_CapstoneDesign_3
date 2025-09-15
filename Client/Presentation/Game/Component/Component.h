#pragma once

#include "pch.h"

class GameObject;

class Component
{
public:
	Component() = delete;
	Component(GameObject*);
	virtual ~Component();

	virtual void CheckCollision(Component*);
	virtual void SolveConstraint();

	virtual void Update(float delta_time);
	virtual void Render(ID3D12GraphicsCommandList*);

	void SetActive(bool);

	// Getter/Setter methods
	GameObject* GetGameObject() const { return game_object_; }
	void SetGameObject(GameObject* game_object) { game_object_ = game_object; }

	bool IsEnabled() const { return is_enable_; }
	void SetEnabled(bool enabled) { is_enable_ = enabled; }

private:
	GameObject* game_object_ = nullptr;
	bool        is_enable_   = true;
};
