#include "pch.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/GameObject.h"
#include "Presentation/Game/Core/Scene.h"

TargetBoardControllerComponent::TargetBoardControllerComponent(GameObject* object) : TempCharacter(object, 10)
{
}

TargetBoardControllerComponent::~TargetBoardControllerComponent()
= default;

void TargetBoardControllerComponent::Update(const float delta_time)
{
	if (!IsEnabled()) return;
	if (!IsAlive()) return;

	float temp = floor(total_elapsed_time_);
	if (particle_emitter_object_)
	{
		if (0 == temp) particle_emitter_component_->SetMaterialByName("p5");
		else if (1 == temp) particle_emitter_component_->SetMaterialByName("p4");
		else if (2 == temp) particle_emitter_component_->SetMaterialByName("p3");
		else if (3 == temp) particle_emitter_component_->SetMaterialByName("p2");
		else if (4 == temp) particle_emitter_component_->SetMaterialByName("p1");
	}

	if (total_elapsed_time_ > attack_delay_seconds_)
	{
		player_character_->Damage(10);
		total_elapsed_time_ = 0;
	}
	total_elapsed_time_ += delta_time;

	TempCharacter::Update(delta_time);
}

void TargetBoardControllerComponent::Awake()
{
	total_elapsed_time_ = 0;

	SetEnabled(true);

	particle_emitter_object_ = new GameObject("particleEmitter");

	TransformComponent*       transform           = new TransformComponent(particle_emitter_object_);
	ParticleEmitterComponent* particle_emitter    = new ParticleEmitterComponent(particle_emitter_object_);
	ParticleBurstInfo         particle_burst_info = {};

	transform->Translate(GetGameObject()->GetComponent<TransformComponent>()->GetPosition(Space::world));
	transform->Translate(0, 2.5, 0);

	particle_emitter->m_bIsBilboard      = true;
	particle_emitter->m_fGravityModifier = 0.0f;
	particle_emitter->SetMaterialByName("p5");
	particle_emitter->m_fStartSpeed     = fRange(0, 0);
	particle_emitter->m_nMaxParticles   = 6;
	particle_emitter->m_fDuration       = 5.0f;
	particle_emitter->m_fStartSize      = fRange(1, 1);
	particle_emitter->m_fCreateCooltime = 1.0f;
	particle_emitter->m_fStartLifetime  = fRange(1, 1);
	particle_emitter->SetBurst(particle_burst_info);

	g_pCurrScene->AddObject(particle_emitter_object_, RenderGroup::PARTICLE);

	particle_emitter_component_ = particle_emitter;
}

void TargetBoardControllerComponent::Die()
{
	g_pCurrScene->event_count++;

	TempCharacter::Die();

	if (particle_emitter_object_)
	{
		particle_emitter_component_->SetEnabled(false);

		particle_emitter_object_    = nullptr;
		particle_emitter_component_ = nullptr;
	}
}

void TargetBoardControllerComponent::SetPlayer(GameObject* object)
{
	player_character_ = object->GetComponent<HumanoidControllerComponent>();
}
