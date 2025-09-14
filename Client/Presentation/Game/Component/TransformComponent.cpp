#include "pch.h"
#include "TransformComponent.h"
#include "Presentation/Game/Object.h"


TransformComponent::TransformComponent(Object* pObject) : Component(pObject), m_xmf4x4Local(matrix::Identity()) {}

TransformComponent::~TransformComponent() {}

void TransformComponent::SetLocalTransform(const XMMATRIX& xmmtxTransform) { XMStoreFloat4x4(&m_xmf4x4Local, xmmtxTransform); }

void TransformComponent::SetLocalTransform(const XMFLOAT4X4& xmf4x4Transform) { m_xmf4x4Local = xmf4x4Transform; }

void TransformComponent::SetPosition(const XMFLOAT3& xmf3Position)
{
    m_xmf4x4Local._41 = xmf3Position.x;
    m_xmf4x4Local._42 = xmf3Position.y;
    m_xmf4x4Local._43 = xmf3Position.z;
}

void TransformComponent::SetPosition(float fX, float fY, float fZ)
{
    m_xmf4x4Local._41 = fX;
    m_xmf4x4Local._42 = fY;
    m_xmf4x4Local._43 = fZ;
}

void TransformComponent::Translate(const XMFLOAT3& xmf3Val)
{
    m_xmf4x4Local._41 += xmf3Val.x;
    m_xmf4x4Local._42 += xmf3Val.y;
    m_xmf4x4Local._43 += xmf3Val.z;
}

void TransformComponent::Translate(float fX, float fY, float fZ)
{
    m_xmf4x4Local._41 += fX;
    m_xmf4x4Local._42 += fY;
    m_xmf4x4Local._43 += fZ;
}

void TransformComponent::RotateXYZDegree(const XMFLOAT3& xmf3Val)
{
    XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-xmf3Val.x), XMConvertToRadians(-xmf3Val.y), XMConvertToRadians(-xmf3Val.z)));
    auto     xmf3Right   = XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13);
    auto     xmf3Up      = XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23);
    auto     xmf3Look    = XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);

    xmf3Look  = vector3::TransformNormal(xmf3Look, xmmtxRotate);
    xmf3Right = vector3::TransformNormal(xmf3Right, xmmtxRotate);

    xmf3Look  = vector3::Normalize(xmf3Look);
    xmf3Right = vector3::CrossProduct(xmf3Up, xmf3Look, true);
    xmf3Up    = vector3::CrossProduct(xmf3Look, xmf3Right, true);

    m_xmf4x4Local._11 = xmf3Right.x;
    m_xmf4x4Local._12 = xmf3Right.y;
    m_xmf4x4Local._13 = xmf3Right.z;
    m_xmf4x4Local._21 = xmf3Up.x;
    m_xmf4x4Local._22 = xmf3Up.y;
    m_xmf4x4Local._23 = xmf3Up.z;
    m_xmf4x4Local._31 = xmf3Look.x;
    m_xmf4x4Local._32 = xmf3Look.y;
    m_xmf4x4Local._33 = xmf3Look.z;
}

void TransformComponent::RotateXYZDegree(float fX, float fY, float fZ)
{
    XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-fX), XMConvertToRadians(-fY), XMConvertToRadians(-fZ)));

    auto xmf3Right = XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13);
    auto xmf3Up    = XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23);
    auto xmf3Look  = XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);

    xmf3Look  = vector3::TransformNormal(xmf3Look, xmmtxRotate);
    xmf3Right = vector3::TransformNormal(xmf3Right, xmmtxRotate);

    xmf3Look  = vector3::Normalize(xmf3Look);
    xmf3Right = vector3::CrossProduct(xmf3Up, xmf3Look, true);
    xmf3Up    = vector3::CrossProduct(xmf3Look, xmf3Right, true);

    m_xmf4x4Local._11 = xmf3Right.x;
    m_xmf4x4Local._12 = xmf3Right.y;
    m_xmf4x4Local._13 = xmf3Right.z;
    m_xmf4x4Local._21 = xmf3Up.x;
    m_xmf4x4Local._22 = xmf3Up.y;
    m_xmf4x4Local._23 = xmf3Up.z;
    m_xmf4x4Local._31 = xmf3Look.x;
    m_xmf4x4Local._32 = xmf3Look.y;
    m_xmf4x4Local._33 = xmf3Look.z;
}

void TransformComponent::Rotate(const XMFLOAT4& xmf4Quaternion)
{
    XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(&xmf4Quaternion));

    auto xmf3Right = XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13);
    auto xmf3Up    = XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23);
    auto xmf3Look  = XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33);

    xmf3Look  = vector3::TransformNormal(xmf3Look, xmmtxRotate);
    xmf3Right = vector3::TransformNormal(xmf3Right, xmmtxRotate);

    xmf3Look  = vector3::Normalize(xmf3Look);
    xmf3Right = vector3::CrossProduct(xmf3Up, xmf3Look, true);
    xmf3Up    = vector3::CrossProduct(xmf3Look, xmf3Right, true);

    m_xmf4x4Local._11 = xmf3Right.x;
    m_xmf4x4Local._12 = xmf3Right.y;
    m_xmf4x4Local._13 = xmf3Right.z;
    m_xmf4x4Local._21 = xmf3Up.x;
    m_xmf4x4Local._22 = xmf3Up.y;
    m_xmf4x4Local._23 = xmf3Up.z;
    m_xmf4x4Local._31 = xmf3Look.x;
    m_xmf4x4Local._32 = xmf3Look.y;
    m_xmf4x4Local._33 = xmf3Look.z;
}

XMMATRIX TransformComponent::GetLocalTransform() { return XMLoadFloat4x4(&m_xmf4x4Local); }

XMMATRIX TransformComponent::GetWorldTransform()
{
    if (nullptr != object->GetParent())
    {
        TransformComponent* l_pParentTransform = object->GetParent()->GetComponent<TransformComponent>();
        return XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4Local), l_pParentTransform->GetWorldTransform());
    }
    return XMLoadFloat4x4(&m_xmf4x4Local);
}

const XMFLOAT3 TransformComponent::GetLookVector(Space space)
{
    switch (space)
    {
    case Space::local: return vector3::Normalize(XMFLOAT3(m_xmf4x4Local._31, m_xmf4x4Local._32, m_xmf4x4Local._33));
    case Space::world: XMFLOAT4X4 worldTransform;
        XMStoreFloat4x4(&worldTransform, GetWorldTransform());
        return vector3::Normalize(XMFLOAT3(worldTransform._31, worldTransform._32, worldTransform._33));
    }
}

const XMFLOAT3 TransformComponent::GetUpVector(Space space)
{
    switch (space)
    {
    case Space::local: return vector3::Normalize(XMFLOAT3(m_xmf4x4Local._21, m_xmf4x4Local._22, m_xmf4x4Local._23));
    case Space::world: XMFLOAT4X4 worldTransform;
        XMStoreFloat4x4(&worldTransform, GetWorldTransform());
        return vector3::Normalize(XMFLOAT3(worldTransform._21, worldTransform._22, worldTransform._23));
    }
}

const XMFLOAT3 TransformComponent::GetRightVector(Space space)
{
    switch (space)
    {
    case Space::local: return vector3::Normalize(XMFLOAT3(m_xmf4x4Local._11, m_xmf4x4Local._12, m_xmf4x4Local._13));
    case Space::world: XMFLOAT4X4 worldTransform;
        XMStoreFloat4x4(&worldTransform, GetWorldTransform());
        return vector3::Normalize(XMFLOAT3(worldTransform._11, worldTransform._12, worldTransform._13));
    }
}

const XMFLOAT3 TransformComponent::GetPosition(Space space)
{
    switch (space)
    {
    case Space::local: return XMFLOAT3(m_xmf4x4Local._41, m_xmf4x4Local._42, m_xmf4x4Local._43);
    case Space::world: XMFLOAT4X4 worldTransform;
        XMStoreFloat4x4(&worldTransform, GetWorldTransform());
        return XMFLOAT3(worldTransform._41, worldTransform._42, worldTransform._43);
    }
}

const XMFLOAT4 TransformComponent::GetRotationQuaternion(Space space)
{
    XMFLOAT4 result;

    switch (space)
    {
    case Space::local: XMStoreFloat4(&result, XMQuaternionRotationMatrix(GetLocalTransform()));
        return result;
    case Space::world: XMStoreFloat4(&result, XMQuaternionRotationMatrix(GetWorldTransform()));
        return result;
    }
}
