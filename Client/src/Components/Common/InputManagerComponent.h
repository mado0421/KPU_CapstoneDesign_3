#pragma once
#include "pch.h"
#include "../../Engine/ECS/Component.h"

#include "src/Game/Input/KeyCode.h"

class InputManagerComponent : public Component
{
public:
    InputManagerComponent() = delete;
    InputManagerComponent(Object* pObject);
    ~InputManagerComponent() override;

    virtual void InputEvent(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement);
    void         Update(float fTimeElapsed) override;

    bool            IsKeyDown(KeyCode key);
    bool            IsKeyUp(KeyCode key);
    const XMFLOAT2& GetMouseMovement();

protected:
    UCHAR    m_pKeysBuffer[256];
    XMFLOAT2 m_xmf2MouseMovement;
};
