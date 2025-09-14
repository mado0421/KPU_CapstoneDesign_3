#pragma once
#include "Component.h"

class DoorComponent : public Component
{
public:
    DoorComponent() = delete;
    DoorComponent(Object* pObject, bool bOpen = false);
    ~DoorComponent() override;

    void Open();
    void Close();

    void Update(float fTimeElapsed) override;

private:
    bool                m_bOpen;
    float               m_fProgress;
    XMFLOAT3            m_xmf3OrigPosition;
    TransformComponent* l_transform;
};
