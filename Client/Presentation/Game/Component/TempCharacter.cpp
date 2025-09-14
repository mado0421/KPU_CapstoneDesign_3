#include "pch.h"
#include "TempCharacter.h"


TempCharacter::TempCharacter(Object* object, const int max_hit_point) : Component(object), max_hit_point(max_hit_point),
	current_hit_point(max_hit_point)
{
}

TempCharacter::~TempCharacter() = default;

void TempCharacter::Update(const float delta_time)
{
	if (IsAlive() == false) return;

	Component::Update(delta_time);
}

void TempCharacter::Damage(const int amount)
{
	if (IsAlive() == false) return;
	current_hit_point = max(current_hit_point - amount, 0);
	if (IsAlive() == false) Die();
}

int TempCharacter::GetCurrentHitPoint() const
{
	return current_hit_point;
}

int TempCharacter::GetMaxHitPoint() const
{
	return max_hit_point;
}

bool TempCharacter::IsAlive() const
{
	return current_hit_point > 0;
}

void TempCharacter::Die()
{
	is_enable = false;
}
