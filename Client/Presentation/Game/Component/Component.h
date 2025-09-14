#pragma once

#include "pch.h"

class Object;

class Component
{
public:
	Component() = delete;
	Component(Object*);
	virtual ~Component();

	virtual void CheckCollision(Component*);
	virtual void SolveConstraint();

	virtual void Update(float delta_time);
	virtual void Render(ID3D12GraphicsCommandList*);

	void SetActive(bool);

public:
	Object* object    = nullptr;
	bool    is_enable = true;
};
