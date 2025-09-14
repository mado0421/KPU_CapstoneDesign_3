#pragma once
#include "Presentation/Game/Component/Component.h"

class TransformComponent;

class CameraComponent : public Component
{
public:
    CameraComponent() = delete;
    CameraComponent(Object* pObject);
    ~CameraComponent() override;

    void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);
    void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
    void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
    void SetLookAtWorldPos(const XMFLOAT3& xmf3LookAtWorld);
    void SetLookAtWorldPos(const float& x, const float& y, const float& z);
    //void SetFocusObject(Object* pObject);
    void SetHeadAndLookAt(Object* pHead, Object* pLookAt, XMFLOAT3 distance);
    void SetFocusDisable();

    const XMFLOAT4X4 GetViewMatrix();
    const XMFLOAT4X4 GetProjectionMatrix();

    const XMFLOAT3 GetLookVector() { return Vector3::Normalize(m_xmf3Look); }

    void CheckCollision(Component* other) override;
    void SolveConstraint() override;
    void Update(float fTimeElapsed) override;

protected:
    void CalculateViewMatrix();
    void CalculateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

    XMFLOAT4X4 m_xmf4x4View;
    XMFLOAT4X4 m_xmf4x4Projection;

    XMFLOAT3 m_xmf3Right;
    XMFLOAT3 m_xmf3Up;
    XMFLOAT3 m_xmf3Look;

    D3D12_VIEWPORT m_d3dViewport;
    D3D12_RECT     m_d3dScissorRect;

    TransformComponent* t;

    //=========================For Cam Collide=======
    TransformComponent* m_pHeadTransform;
    TransformComponent* m_pLookAtTransform;
    XMFLOAT3            m_xmf3Direction;
    float               m_fMinLength = FLT_MAX;
    float               m_fDistance;
    bool                m_bIfCollide = false;
};
