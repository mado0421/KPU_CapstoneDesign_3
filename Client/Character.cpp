#include "stdafx.h"
#include "Character.h"

Character::Character(int maxHp, bool bAutoRevive, bool bDead)
	: m_maxHp(maxHp)
	  , m_currHp(maxHp)
	  , m_fReviveTime(3.0f)
	  , m_fCurrReviveTime(0.0f)
	  , m_bDead(bDead)
	  , m_bAutoRevive(bAutoRevive)
{
}

Character::~Character()
{
}

void Character::Update(float fTimeElapsed)
{
	if (isDead()) Die();

	if (m_bAutoRevive && m_bDead)
	{
		m_fCurrReviveTime += fTimeElapsed;
		if (m_fReviveTime <= m_fCurrReviveTime) Revive();
	}
}

void Character::Damage(int dmg)
{
	m_currHp -= dmg;
}

void Character::Die()
{
	m_bDead = true;
}

void Character::Revive()
{
	m_currHp = m_maxHp;
	m_fCurrReviveTime = 0.0f;
	m_bDead = false;
}

int Character::GetCurrHp()
{
	return m_currHp;
}

int Character::GetMaxHp()
{
	return m_maxHp;
}

float Character::GetCurrHpPerMaxHp()
{
	return (m_currHp / m_maxHp) * 100;
}
