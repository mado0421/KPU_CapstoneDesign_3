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

    EventInfo(EVENT t, void* d) : type(t), data(d) {}
};

class EventComponent : public Component
{
public:
    EventComponent() = delete;
    EventComponent(GameObject* pObject);
    EventComponent(GameObject* pObject, int count);
    ~EventComponent() override;

    void AddEvent(EventInfo& info);
    // trigger�� ����Ǹ�, event�� �����Ѵ�.
    void SetEvent();

    void SolveConstraint() override;
    void Update(float fTimeElapsed) override;

private:
    bool            isTriggerReady = false;
    vector<GameObject*> vecEnemyToWatch;
    bool            isWatchingEnemy = false;

    vector<GameObject*> vecDoorToOpen;
    vector<GameObject*> vecDoorToClose;
    vector<GameObject*> vecEnemyToWake;
    bool            isEventForVictory = false;
    bool            isEventForDefeat  = false;
    int             targetCount       = -1;
};
