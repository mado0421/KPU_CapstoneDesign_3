#pragma once
#include "Presentation/Game/Component/Component.h"

/*
	TransformComponent Component is Component for Object Coordnation.
*/

enum class Space
{
    local = false,
    world = true
};

class TransformComponent : public Component
{
public:
    TransformComponent() = delete;
    TransformComponent(Object* pObject);
    ~TransformComponent() override;

    void SetLocalTransform(const XMMATRIX& xmmtxTransform);
    void SetLocalTransform(const XMFLOAT4X4& xmf4x4Transform);
    void SetPosition(const XMFLOAT3& xmf3Position);
    void SetPosition(float fX, float fY, float fZ);
    void Translate(const XMFLOAT3& xmf3Val);
    void Translate(float fX, float fY, float fZ);
    void RotateXYZDegree(const XMFLOAT3& xmf3Val);
    void RotateXYZDegree(float fX, float fY, float fZ);
    void Rotate(const XMFLOAT4& xmf4Quaternion);

    XMMATRIX       GetLocalTransform();
    XMMATRIX       GetWorldTransform();
    const XMFLOAT3 GetLookVector(Space space = Space::local);
    const XMFLOAT3 GetUpVector(Space space = Space::local);
    const XMFLOAT3 GetRightVector(Space space = Space::local);
    const XMFLOAT3 GetPosition(Space space = Space::local);
    const XMFLOAT4 GetRotationQuaternion(Space space = Space::local);

protected:
    XMFLOAT4X4 m_xmf4x4Local;
};
