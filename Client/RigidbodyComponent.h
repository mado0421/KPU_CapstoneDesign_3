#pragma once
#include "Component.h"

class RigidbodyComponent : public Component
{
public:
	RigidbodyComponent() = delete;
	RigidbodyComponent(Object* pObject);
	~RigidbodyComponent() override;

	void SolveConstraint() override;
	void Update(float fTimeElapsed) override;

protected:
	XMFLOAT3 m_xmf3PrevPosition;
};
