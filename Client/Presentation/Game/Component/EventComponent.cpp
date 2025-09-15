#include "pch.h"

#include "Presentation/Game/GameObject.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Core/Scene.h"

EventComponent::EventComponent(GameObject* pObject) : Component(pObject), targetCount(-1) {}

EventComponent::EventComponent(GameObject* pObject, int count) : Component(pObject), targetCount(count) {}

EventComponent::~EventComponent() {}

void EventComponent::AddEvent(EventInfo& info)
{
    vector<GameObject*>* temp;
    switch (info.type)
    {
    case EVENT::TRIGGER: isTriggerReady = true;
        break;
    case EVENT::EDIED: temp = static_cast<vector<GameObject*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecEnemyToWatch.push_back(temp->at(i));
        isWatchingEnemy = true;
        break;

    case EVENT::DOPEN: temp = static_cast<vector<GameObject*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecDoorToOpen.push_back(temp->at(i));

        break;
    case EVENT::DCLOSE: temp = static_cast<vector<GameObject*>*>(info.data);
        for (int i = 0; i < temp->size(); i++) vecDoorToClose.push_back(temp->at(i));

        break;
    case EVENT::EWAKE: temp = static_cast<vector<GameObject*>*>(info.data);
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
    for_each(vecDoorToOpen.begin(), vecDoorToOpen.end(), [&](GameObject* o) { o->GetComponent<DoorComponent>()->Open(); });

    for_each(vecDoorToClose.begin(), vecDoorToClose.end(), [&](GameObject* o) { o->GetComponent<DoorComponent>()->Close(); });

    for_each(vecEnemyToWake.begin(), vecEnemyToWake.end(), [&](GameObject* o) { o->GetComponent<TargetBoardControllerComponent>()->Awake(); });

    if (isEventForVictory) g_pCurrScene->Victory();
    if (isEventForDefeat) g_pCurrScene->Defeat();

    // ���������� ���� ����
    GetGameObject()->SetActive(false);
    SetEnabled(false);
}


void EventComponent::SolveConstraint()
{
    if (!IsEnabled()) return;

    if (g_pCurrScene->event_count >= targetCount)
    {
        if (isTriggerReady)
        {
            vector<ColliderComponent*> l_vecCollider = GetGameObject()->GetComponents<ColliderComponent>();

            for_each(l_vecCollider.begin(), l_vecCollider.end(), [&](ColliderComponent* c)
            {
                for (int i = 0; i < c->m_vecpCollided.size(); i++)
                {
                    HumanoidControllerComponent* hcc = c->m_vecpCollided[i]->GetGameObject()->GetComponent<HumanoidControllerComponent>();
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

    if (!IsEnabled()) return;

    if (g_pCurrScene->event_count >= targetCount)
    {
        if (isWatchingEnemy)
        {
            for_each(vecEnemyToWatch.begin(), vecEnemyToWatch.end(), [&](GameObject* o) { if (o->GetComponent<TempCharacter>()->IsAlive()) return; });

            SetEvent();
        }
    }
}
