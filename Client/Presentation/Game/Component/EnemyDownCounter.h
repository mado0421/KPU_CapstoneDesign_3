#pragma once
#include "Component.h"

class TempCharacter;

class EnemyDownCounter : public Component
{
public:
    EnemyDownCounter() = delete;
    EnemyDownCounter(GameObject* pObject);
    ~EnemyDownCounter() override;

    void AddTarget(GameObject* pObject);
    void SetTextRenderer(TextRendererComponent* pTRC);

    void Update(float fTimeElapsed) override;

private:
    vector<pair<TempCharacter*, bool>> m_vecTargetCharacter;
    TextRendererComponent*         m_pTRC  = nullptr;
    int                            m_count = 0;
};
