#pragma once
#include "Component.h"

class TempCharacter;

class EnemyDownCounter : public Component
{
public:
    EnemyDownCounter() = delete;
    EnemyDownCounter(Object* pObject);
    ~EnemyDownCounter() override;

    void AddTarget(Object* pObject);
    void SetTextRenderer(TextRendererComponent* pTRC);

    void Update(float fTimeElapsed) override;

private:
    vector<pair<TempCharacter*, bool>> m_vecTargetCharacter;
    TextRendererComponent*         m_pTRC  = nullptr;
    int                            m_count = 0;
};
