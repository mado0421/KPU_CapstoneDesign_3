#include "pch.h"
#include "Presentation/Game/GameObject.h"
#include "Presentation/Game/Component/Components.h"

GameManagerComponent::GameManagerComponent(GameObject* pObject) : Component(pObject) {}

GameManagerComponent::~GameManagerComponent() {}

void GameManagerComponent::Update(float fTimeElapsed) {}
