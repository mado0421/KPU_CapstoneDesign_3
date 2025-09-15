#pragma once

class Component;

struct CB_OBJECT_INFO
{
	XMFLOAT4X4 xmf4x4World;
};

class Mesh;
class MeshData;

class GameObject
{
public:
	GameObject();
	explicit GameObject(const char* name);
	virtual  ~GameObject();

	virtual void CheckCollision(GameObject* other);
	virtual void SolveConstraint();
	virtual void Input(UCHAR* key_buffer, XMFLOAT2& mouse_movement);
	virtual void Update(float delta_time);
	virtual void Render(ID3D12GraphicsCommandList*);

	virtual void SetParent(GameObject* object) { parent_ = object; }
	void         SetActive(bool is_active);

	void AddComponent(Component* component);

public:
	template <typename T>
	T* GetComponent();

	template <typename T>
	vector<T*> GetComponents();

	template <typename T>
	vector<T*> GetComponentsInChildren();

	GameObject* GetParent() const { return parent_; }
	string GetName() const { return name_; }
	bool IsEnabled() const { return is_enable_; }

protected:
	template <typename T>
	void GetComponentsRecursive(vector<T*>& result);

protected:
	string name_;
	bool   is_enable_;

private:
	GameObject*        parent_;
	vector<GameObject*>    children_;
	vector<Component*> components_;
};


template <typename T>
T* GameObject::GetComponent()
{
	for (Component* c : components_)
	{
		T* as = dynamic_cast<T*>(c);
		if (nullptr != as) return as;
	}
	return nullptr;
}

template <typename T>
vector<T*> GameObject::GetComponents()
{
	vector<T*> result;

	for (Component* c : components_)
	{
		T* as = dynamic_cast<T*>(c);
		if (nullptr != as) result.push_back(as);
	}

	return result;
}

template <typename T>
vector<T*> GameObject::GetComponentsInChildren()
{
	vector<T*> result;

	GetComponentsRecursive(result);

	return result;
}

template <typename T>
void GameObject::GetComponentsRecursive(vector<T*>& result)
{
	for (Component* c : components_)
	{
		T* as = dynamic_cast<T*>(c);
		if (nullptr != as) result.push_back(as);
	}

	for (GameObject* c : children_) c->GetComponentsRecursive<T>(result);
}