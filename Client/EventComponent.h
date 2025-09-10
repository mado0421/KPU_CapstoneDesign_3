#pragma once
#include "Component.h"

enum class EVENT
{
	TRIGGER,
	DOPEN,
	DCLOSE,
	EWAKE,
	EDIED,
	VICTORY,
	DEFEAT,
};

struct EventInfo
{
	EVENT type;
	void* data;

	EventInfo(EVENT t, void* d)
		: type(t)
		  , data(d)
	{
	}
};

class EventComponent : public Component
{
public:
	EventComponent() = delete;
	EventComponent(Object* pObject);
	EventComponent(Object* pObject, int count);
	~EventComponent() override;

	void AddEvent(EventInfo& info);
	// trigger가 실행되면, event를 실행한다.
	void SetEvent();

	void SolveConstraint() override;
	void Update(float fTimeElapsed) override;

private:
	bool isTriggerReady = false;
	vector<Object*> vecEnemyToWatch;
	bool isWatchingEnemy = false;

	vector<Object*> vecDoorToOpen;
	vector<Object*> vecDoorToClose;
	vector<Object*> vecEnemyToWake;
	bool isEventForVictory = false;
	bool isEventForDefeat = false;
	int targetCount = -1;
};
