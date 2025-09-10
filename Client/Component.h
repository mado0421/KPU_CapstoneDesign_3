#pragma once

class Object;

class Component
{
public:
	Component() = delete;
	Component(Object* pObject);
	virtual ~Component();

	virtual Component* GetInstance();

	virtual void CheckCollision(Component* c)
	{
	}

	virtual void SolveConstraint()
	{
	}

	virtual void Update(float fTimeElapsed)
	{
	}

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList)
	{
	}

	void SetActive(bool bActive);

	Object* m_pObject = nullptr;
	bool m_bEnabled = true;
};
