#include "pch.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Object.h"
#include "Presentation/Game/Core/Scene.h"

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
    for_each(vecDoorToOpen.begin(), vecDoorToOpen.end(), [&](Object* o) { o->GetComponent<DoorComponent>()->Open(); });

    for_each(vecDoorToClose.begin(), vecDoorToClose.end(), [&](Object* o) { o->GetComponent<DoorComponent>()->Close(); });

    for_each(vecEnemyToWake.begin(), vecEnemyToWake.end(), [&](Object* o) { o->GetComponent<TargetBoardControllerComponent>()->Awake(); });

    if (isEventForVictory) g_pCurrScene->Victory();
    if (isEventForDefeat) g_pCurrScene->Defeat();

    // ���������� ���� ����
    object->SetActive(false);
    is_enable = false;
}


void EventComponent::SolveConstraint()
{
    if (!is_enable) return;

    if (g_pCurrScene->event_count >= targetCount)
    {
        if (isTriggerReady)
        {
            vector<ColliderComponent*> l_vecCollider = object->GetComponents<ColliderComponent>();

            for_each(l_vecCollider.begin(), l_vecCollider.end(), [&](ColliderComponent* c)
            {
                for (int i = 0; i < c->m_vecpCollided.size(); i++)
                {
                    HumanoidControllerComponent* hcc = c->m_vecpCollided[i]->object->GetComponent<HumanoidControllerComponent>();
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

    if (!is_enable) return;

    if (g_pCurrScene->event_count >= targetCount)
    {
        if (isWatchingEnemy)
        {
            for_each(vecEnemyToWatch.begin(), vecEnemyToWatch.end(), [&](Object* o) { if (o->GetComponent<TempCharacter>()->IsAlive()) return; });

            SetEvent();
        }
    }
}
