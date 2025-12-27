#pragma once
#include "../../Engine/ECS/Component.h"

class BoxColliderComponent;
class SphereColliderComponent;
class Object;
class AnimatorComponent;

using BoxColliders    = vector<BoxColliderComponent*>;
using SphereColliders = vector<SphereColliderComponent*>;

class ColliderComponent : public Component
{
public:
    ColliderComponent() = delete;
    ColliderComponent(Object* pObject, AnimatorComponent* pAnimator, bool bTrigger = false, int boneIdx = 0);
    ~ColliderComponent() override;

    void Update(float fTimeElapsed) override;

    bool isTrigger();

    vector<ColliderComponent*> m_vecpCollided;
    XMFLOAT4X4                 m_xmf4x4Local;

protected:
    AnimatorComponent* m_pAnimator;
    int                m_boneIdx;
    bool               m_bTrigger;
};

class BoxColliderComponent : public ColliderComponent
{
public:
    BoxColliderComponent() = delete;
    BoxColliderComponent(Object* pObject, const XMFLOAT3& xmf3Extents, bool bTrigger = false, AnimatorComponent* pAnimator = nullptr, int boneIdx = 0);
    BoxColliderComponent(Object* pObject, const XMFLOAT3& xmf3Center, const XMFLOAT3& xmf3Extents, const XMFLOAT4& xmf4Orientation, bool bTrigger = false, AnimatorComponent* pAnimator = nullptr, int boneIdx = 0);
    ~BoxColliderComponent() override;

    void Update(float fTimeElapsed) override;

    void CheckCollision(Component* other) override;

    BoundingOrientedBox m_box;
};

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent() = delete;
    SphereColliderComponent(Object* pObject, const float& fRadius, bool bTrigger = false, AnimatorComponent* pAnimator = nullptr, int boneIdx = 0);
    SphereColliderComponent(Object* pObject, const XMFLOAT3& xmf3Center, const float& fRadius, bool bTrigger = false, AnimatorComponent* pAnimator = nullptr, int boneIdx = 0);
    ~SphereColliderComponent() override;

    void Update(float fTimeElapsed) override;

    void CheckCollision(Component* other) override;

    BoundingSphere m_sphere;
};
