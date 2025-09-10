#pragma once

class Character
{
public:
    Character(int maxHp, bool bAutoRevive = false, bool bDead = true);
    virtual ~Character();

    virtual void Update(float fTimeElapsed);

    virtual void Damage(int);
    bool         isAlive() const { return !m_bDead; }
    virtual void Die();
    virtual void Revive();
    int          GetCurrHp();
    int          GetMaxHp();
    float        GetCurrHpPerMaxHp();

private:
    virtual bool isDead() const { return 0 >= m_currHp; }

    const int   m_maxHp;
    int         m_currHp;
    const float m_fReviveTime;
    float       m_fCurrReviveTime;
    bool        m_bDead;
    bool        m_bAutoRevive;
};
