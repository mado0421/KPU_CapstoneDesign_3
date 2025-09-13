#include "pch.h"
#include "../../Core/Components.h"
#include "../../Core/Object.h"
#include "../../Core/Scene.h"

EventComponent::EventComponent(Object* pObject) : Component(pObject), targetCount(-1) {}

EventComponent::EventComponent(Object* pObject, int count) : Component(pObject), targetCount(count) {}

EventComponent::~EventComponent() {}

void EventComponent::AddEvent(EventInfo& info)
{
    vector<Object*>* temp;
    switch (info.type)
    {
    case EVENT::TRIGGER: isTriggerReady = true;
        break;
    case EVENT::EDIED: temp = static_cast<vector<Object*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecEnemyToWatch.push_back(temp->at(i));
        isWatchingEnemy = true;
        break;

    case EVENT::DOPEN: temp = static_cast<vector<Object*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecDoorToOpen.push_back(temp->at(i));

        break;
    case EVENT::DCLOSE: temp = static_cast<vector<Object*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecDoorToClose.push_back(temp->at(i));

        break;
    case EVENT::EWAKE: temp = static_cast<vector<Object*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecEnemyToWake.push_back(temp->at(i));

        break;
    case EVENT::VICTORY: isEventForVictory = true;
        break;
    case EVENT::DEFEAT: isEventForDefeat = true;
        break;
    default: break;
    }
}

void EventComponent::SetEvent()
{
    for_each(vecDoorToOpen.begin(), vecDoorToOpen.end(), [&](Object* o) { o->FindComponent<DoorComponent>()->Open(); });

    for_each(vecDoorToClose.begin(), vecDoorToClose.end(), [&](Object* o) { o->FindComponent<DoorComponent>()->Close(); });

    for_each(vecEnemyToWake.begin(), vecEnemyToWake.end(), [&](Object* o) { o->FindComponent<Character>()->Revive(); });

    if (isEventForVictory) g_pCurrScene->Victory();
    if (isEventForDefeat) g_pCurrScene->Defeat();

    // ���������� ���� ����
    m_pObject->SetActive(false);
    m_bEnabled = false;
}


void EventComponent::SolveConstraint()
{
    if (!m_bEnabled) return;

    if (g_pCurrScene->eventCount >= targetCount)
    {
        if (isTriggerReady)
        {
            vector<ColliderComponent*> l_vecCollider = m_pObject->FindComponents<ColliderComponent>();

            for_each(l_vecCollider.begin(), l_vecCollider.end(), [&](ColliderComponent* c)
            {
                for (int i = 0; i < c->m_vecpCollided.size(); i++)
                {
                    HumanoidControllerComponent* hcc = c->m_vecpCollided[i]->m_pObject->FindComponent<HumanoidControllerComponent>();
                    if (hcc) SetEvent();
                }
            });
        }
    }
}

void EventComponent::Update(float fTimeElapsed)
{
    /*
    �� �ݶ��̴�(Ʈ����)�� �����ų�, ���� �� �װų�, �ٸ� �̺�Ʈ�� �Ѱų�
    */

    if (!m_bEnabled) return;

    if (g_pCurrScene->eventCount >= targetCount)
    {
        if (isWatchingEnemy)
        {
            for_each(vecEnemyToWatch.begin(), vecEnemyToWatch.end(), [&](Object* o) { if (o->FindComponent<Character>()->isAlive()) return; });

            SetEvent();
        }
    }
}
