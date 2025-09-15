#pragma once
#include "Presentation/Game/Component/Component.h"

class GameObject;
class ParticleEmitterComponent;

class TargetBoardControllerComponent : public TempCharacter
{
public:
	TargetBoardControllerComponent() = delete;
	TargetBoardControllerComponent(GameObject* object);
	~TargetBoardControllerComponent() override;

	void Update(float delta_time) override;
	void Awake();
	void Die();

	void SetPlayer(GameObject* object);

private:
	float total_elapsed_time_   = 0.0f;
	float attack_delay_seconds_ = 5.0f;

	TempCharacter*            player_character_           = nullptr;
	GameObject*                   particle_emitter_object_    = nullptr;
	ParticleEmitterComponent* particle_emitter_component_ = nullptr;
};
