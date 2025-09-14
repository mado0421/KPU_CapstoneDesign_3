#include "pch.h"
#include "Presentation/Game/Component/Components.h"

EnemyDownCounter::EnemyDownCounter(Object* pObject) : Component(pObject)
{
    TextRendererComponent* temp = pObject->FindComponent<TextRendererComponent>();
    if (temp) m_pTRC = temp;
}

EnemyDownCounter::~EnemyDownCounter() {}

void EnemyDownCounter::AddTarget(Object* pObject)
{
    TargetBoardControllerComponent* temp = pObject->FindComponent<TargetBoardControllerComponent>();

    if (temp)
    {
        pair<TempCharacter*, bool> tempPair;
        tempPair.first  = temp;
        tempPair.second = false;
        m_vecTargetCharacter.push_back(tempPair);
    }
}

void EnemyDownCounter::SetTextRenderer(TextRendererComponent* pTRC) { m_pTRC = pTRC; }

void EnemyDownCounter::Update(float fTimeElapsed)
{
    if (!is_enable) return;

    for (int i = 0; i < m_vecTargetCharacter.size(); i++)
    {
        if (m_vecTargetCharacter[i].second == false && !m_vecTargetCharacter[i].first->IsAlive())
        {
            m_count++;
            m_vecTargetCharacter[i].second = true;
        }
        else if (m_vecTargetCharacter[i].second == true && m_vecTargetCharacter[i].first->IsAlive()) m_vecTargetCharacter[i].second = false;
    }

    if (m_pTRC)
    {
        string temp = "CurrentCount: ";
        temp += to_string(m_count);
        m_pTRC->SetText(temp.c_str());
    }
}
