#include "pch.h"
#include "CameraComponent.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Object.h"

CameraComponent::CameraComponent(Object* pObject) : Component(pObject),
                                                    m_xmf4x4View(matrix::Identity()),
                                                    m_xmf4x4Projection(matrix::Identity()),
                                                    m_xmf3Right(1, 0, 0),
                                                    m_xmf3Up(0, 1, 0),
                                                    m_xmf3Look(0, 0, 1),
                                                    m_d3dViewport({0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f}),
                                                    m_d3dScissorRect({0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT}),
                                                    m_pHeadTransform(nullptr),
                                                    m_pLookAtTransform(nullptr)
{
    CalculateProjectionMatrix(0.1f, 1000.0f, ASPECT_RATIO, 60.0f);

    SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
    SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

    t = object->FindComponent<TransformComponent>();
}

CameraComponent::~CameraComponent() {}

void CameraComponent::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
    pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
    pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CameraComponent::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
    m_d3dViewport.TopLeftX = static_cast<float>(xTopLeft);
    m_d3dViewport.TopLeftY = static_cast<float>(yTopLeft);
    m_d3dViewport.Width    = static_cast<float>(nWidth);
    m_d3dViewport.Height   = static_cast<float>(nHeight);
    m_d3dViewport.MinDepth = fMinZ;
    m_d3dViewport.MaxDepth = fMaxZ;
}

void CameraComponent::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
    m_d3dScissorRect.left   = xLeft;
    m_d3dScissorRect.top    = yTop;
    m_d3dScissorRect.right  = xRight;
    m_d3dScissorRect.bottom = yBottom;
}

void CameraComponent::SetLookAtWorldPos(const XMFLOAT3& xmf3LookAt)
{
    XMFLOAT3 xmf3Pos = t->GetPosition(Space::world);

    XMFLOAT4X4 xmf4x4New = matrix::LookAtLH(xmf3Pos, xmf3LookAt, XMFLOAT3(0, 1, 0));
    //XMFLOAT4X4 xmf4x4New = Matrix4x4::LookAtLH(xmf3Pos, xmf3LookAt, m_xmf3Up);

    m_xmf3Right = XMFLOAT3(xmf4x4New._11, xmf4x4New._21, xmf4x4New._31);
    m_xmf3Up    = XMFLOAT3(xmf4x4New._12, xmf4x4New._22, xmf4x4New._32);
    m_xmf3Look  = XMFLOAT3(xmf4x4New._13, xmf4x4New._23, xmf4x4New._33);
}

void CameraComponent::SetLookAtWorldPos(const float& x, const float& y, const float& z) { SetLookAtWorldPos(XMFLOAT3(x, y, z)); }

//void CameraComponent::SetFocusObject(Object* pObject)
//{
//	m_pLookAtTransform = pObject->FindComponent<TransformComponent>();
//}

void CameraComponent::SetHeadAndLookAt(Object* pHead, Object* pLookAt, XMFLOAT3 distance)
{
    m_pHeadTransform   = pHead->FindComponent<TransformComponent>();
    m_pLookAtTransform = pLookAt->FindComponent<TransformComponent>();
    m_xmf3Direction    = vector3::Normalize(distance);
    m_fDistance        = vector3::Length(distance);
}

void CameraComponent::SetFocusDisable() { m_pLookAtTransform = nullptr; }

const XMFLOAT4X4 CameraComponent::GetViewMatrix() { return m_xmf4x4View; }

const XMFLOAT4X4 CameraComponent::GetProjectionMatrix() { return m_xmf4x4Projection; }

void CameraComponent::CheckCollision(Component* other)
{
    if (m_pHeadTransform)
    {
        XMFLOAT3 xmf3Origin, xmf3Direction;
        float    length = 0;


        xmf3Origin    = m_pHeadTransform->GetPosition(Space::world);
        xmf3Direction = m_xmf3Direction;

        XMVECTOR origin    = XMLoadFloat3(&xmf3Origin);
        XMVECTOR direction = XMLoadFloat3(&xmf3Direction);

        direction = XMVector3Rotate(direction, XMQuaternionRotationMatrix(t->GetWorldTransform()));

        auto otherBoxCollider = dynamic_cast<BoxColliderComponent*>(other);
        if (otherBoxCollider)
        {
            otherBoxCollider->m_box.Intersects(origin, direction, length);
            if (0 < length)
            {
                if (m_fDistance > length && m_fMinLength > length)
                {
                    m_fMinLength = length;
                    m_bIfCollide = true;
                }
            }
        }
        auto otherSphereCollider = dynamic_cast<SphereColliderComponent*>(other);
        if (otherSphereCollider)
        {
            otherSphereCollider->m_sphere.Intersects(origin, direction, length);
            if (0 < length)
            {
                if (m_fDistance > length && m_fMinLength > length)
                {
                    m_fMinLength = length;
                    m_bIfCollide = true;
                }
            }
        }
    }
}

void CameraComponent::SolveConstraint()
{
    XMFLOAT3 position = m_pHeadTransform->GetPosition(Space::local);
    if (m_bIfCollide) position = vector3::Add(position, vector3::Multiply(m_fMinLength * 0.8f, m_xmf3Direction));
    else position              = vector3::Add(position, vector3::Multiply(m_fDistance, m_xmf3Direction));
    t->SetPosition(position);

    m_fMinLength = FLT_MAX;
    m_bIfCollide = false;
}


void CameraComponent::Update(float fTimeElapsed)
{
    if (!is_enable) return;


    if (m_pLookAtTransform) SetLookAtWorldPos(m_pLookAtTransform->GetPosition(Space::world));
    CalculateViewMatrix();
}

void CameraComponent::CalculateViewMatrix()
{
    TransformComponent* transform    = t;
    XMFLOAT3            xmf3Position = transform->GetPosition(Space::world);

    m_xmf3Look  = vector3::Normalize(m_xmf3Look);
    m_xmf3Right = vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
    m_xmf3Up    = vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

    m_xmf4x4View._11 = m_xmf3Right.x;
    m_xmf4x4View._12 = m_xmf3Up.x;
    m_xmf4x4View._13 = m_xmf3Look.x;
    m_xmf4x4View._21 = m_xmf3Right.y;
    m_xmf4x4View._22 = m_xmf3Up.y;
    m_xmf4x4View._23 = m_xmf3Look.y;
    m_xmf4x4View._31 = m_xmf3Right.z;
    m_xmf4x4View._32 = m_xmf3Up.z;
    m_xmf4x4View._33 = m_xmf3Look.z;
    m_xmf4x4View._41 = -vector3::DotProduct(xmf3Position, m_xmf3Right);
    m_xmf4x4View._42 = -vector3::DotProduct(xmf3Position, m_xmf3Up);
    m_xmf4x4View._43 = -vector3::DotProduct(xmf3Position, m_xmf3Look);
}

void CameraComponent::CalculateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
    m_xmf4x4Projection = matrix::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}
