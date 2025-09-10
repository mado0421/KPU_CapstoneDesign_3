#include "stdafx.h"
#include "Components.h"
#include "Scene.h"
#include "Object.h"

EventComponent::EventComponent(Object* pObject)
	: Component(pObject)
	  , targetCount(-1)
{
}

EventComponent::EventComponent(Object* pObject, int count)
	: Component(pObject)
	  , targetCount(count)
{
}

EventComponent::~EventComponent()
{
}

void EventComponent::AddEvent(EventInfo& info)
{
	vector<Object*>* temp;
	switch (info.type)
	{
	case EVENT::TRIGGER: isTriggerReady = true;
		break;
	case EVENT::EDIED:
		temp = static_cast<vector<Object*>*>(info.data);
		for (int i = 0; i < temp->size(); i++)
		{
			vecEnemyToWatch.push_back(temp->at(i));
		}
		isWatchingEnemy = true;
		break;

	case EVENT::DOPEN:
		temp = static_cast<vector<Object*>*>(info.data);
		for (int i = 0; i < temp->size(); i++)
			vecDoorToOpen.push_back(temp->at(i));

		break;
	case EVENT::DCLOSE:
		temp = static_cast<vector<Object*>*>(info.data);
		for (int i = 0; i < temp->size(); i++)
			vecDoorToClose.push_back(temp->at(i));

		break;
	case EVENT::EWAKE:
		temp = static_cast<vector<Object*>*>(info.data);
		for (int i = 0; i < temp->size(); i++)
			vecEnemyToWake.push_back(temp->at(i));

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
	for_each(vecDoorToOpen.begin(), vecDoorToOpen.end(), [&](Object* o)
	{
		o->FindComponent<DoorComponent>()->Open();
	});

	for_each(vecDoorToClose.begin(), vecDoorToClose.end(), [&](Object* o)
	{
		o->FindComponent<DoorComponent>()->Close();
	});

	for_each(vecEnemyToWake.begin(), vecEnemyToWake.end(), [&](Object* o)
	{
		o->FindComponent<Character>()->Revive();
	});

	if (isEventForVictory) g_pCurrScene->Victory();
	if (isEventForDefeat) g_pCurrScene->Defeat();

	// 실행했으니 전부 끄기
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
					HumanoidControllerComponent* hcc = c->m_vecpCollided[i]->m_pObject->FindComponent<
						HumanoidControllerComponent>();
					if (hcc)
						SetEvent();
				}
			});
		}
	}
}

void EventComponent::Update(float fTimeElapsed)
{
	/*
	내 콜라이더(트리거)에 잡히거나, 적이 다 죽거나, 다른 이벤트가 켜거나
	*/

	if (!m_bEnabled) return;

	if (g_pCurrScene->eventCount >= targetCount)
	{
		if (isWatchingEnemy)
		{
			for_each(vecEnemyToWatch.begin(), vecEnemyToWatch.end(), [&](Object* o)
			{
				if (o->FindComponent<Character>()->isAlive()) return;
			});

			SetEvent();
		}
	}
}
