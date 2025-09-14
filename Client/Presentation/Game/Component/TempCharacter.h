#pragma once
#include "Component.h"

class TempCharacter : public Component
{
public:
	TempCharacter() = delete;
	TempCharacter(Object*, int max_hit_point);
	virtual ~TempCharacter();

	virtual void Update(float delta_time) override;

	virtual void Damage(int);

	int GetCurrentHitPoint() const;
	int GetMaxHitPoint() const;
	bool IsAlive() const;

protected:
	virtual void Die();

private:
	int max_hit_point;
	int current_hit_point;
};
