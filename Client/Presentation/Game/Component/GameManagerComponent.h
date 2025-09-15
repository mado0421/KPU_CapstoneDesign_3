#pragma once
#include "Component.h"

class GameManagerComponent : public Component
{
public:
    GameManagerComponent() = delete;
    GameManagerComponent(GameObject* pObject);
    ~GameManagerComponent() override;

    void Update(float fTimeElapsed) override;
};
