#pragma once
#include "../../Engine/ECS/Component.h"

class GameManagerComponent : public Component
{
public:
    GameManagerComponent() = delete;
    GameManagerComponent(Object* pObject);
    ~GameManagerComponent() override;

    void Update(float fTimeElapsed) override;
};
