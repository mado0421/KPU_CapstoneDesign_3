#include "stdafx.h"
#include "RigidbodyComponent.h"
#include "Object.h"
#include "Components.h"

RigidbodyComponent::RigidbodyComponent(Object* pObject)
	: Component(pObject)
	  , m_xmf3PrevPosition(XMFLOAT3(0, 0, 0))
{
}

RigidbodyComponent::~RigidbodyComponent()
{
}

void RigidbodyComponent::SolveConstraint()
{
	vector<ColliderComponent*> l_vecCollider = m_pObject->FindComponents<ColliderComponent>();

	for_each(l_vecCollider.begin(), l_vecCollider.end(), [&](ColliderComponent* c)
	{
		if (!c->m_vecpCollided.empty())
		{
			for (int idx = 0; idx < c->m_vecpCollided.size(); idx++)
			{
				if (c->m_vecpCollided[idx]->isTrigger()) continue;

				// ray direction을 구한다.
				TransformComponent* transform = m_pObject->FindComponent<TransformComponent>();
				XMFLOAT3 xmf3CurrPosition = transform->GetPosition();
				XMFLOAT3 xmf3CurrVector = Vector3::Subtract(xmf3CurrPosition, m_xmf3PrevPosition);
				if (0 == Vector3::Length(xmf3CurrVector)) continue;
				XMFLOAT3 xmf3Direction = Vector3::Normalize(xmf3CurrVector);

				// ray의 origin은 currPosition과 currPosition +- xmf3Vertical * m_sphere.radius 한 좌표들
				// direction과 UpVector에 내적한 벡터(direction에 수직인 벡터)에 +/-0.5만큼 스칼라곱한 벡터를 더해줘야 함.
				XMFLOAT3 xmf3Vertical = Vector3::CrossProduct(xmf3Direction, XMFLOAT3(0, 1, 0));
				XMFLOAT3 xmf3Origins[3] = {
					Vector3::Add(xmf3CurrPosition, Vector3::Multiply(-0.5f, xmf3Vertical)),
					xmf3CurrPosition,
					Vector3::Add(xmf3CurrPosition, Vector3::Multiply(0.5f, xmf3Vertical))
				};

				// **** origin이 충돌체 안에 파묻히는 문제가 생겨서 bias 값을 추가하기로 함 ****
				// **** bias 값은 1로 하고, direction 단위벡터를 사용 ****
				for (int i = 0; i < 3; i++) xmf3Origins[i] = Vector3::Add(
					xmf3Origins[i], Vector3::Multiply(-1, xmf3Direction));

				XMVECTOR origins[3] = {
					XMLoadFloat3(&xmf3Origins[0]),
					XMLoadFloat3(&xmf3Origins[1]),
					XMLoadFloat3(&xmf3Origins[2])
				};
				XMVECTOR direction = XMLoadFloat3(&xmf3Direction);
				float lengths[3] = {0, 0, 0};

				// 충돌한 상대가 BoxCollider인지, SphereCollider인지 확인해야 함.
				auto otherBox = dynamic_cast<BoxColliderComponent*>(c->m_vecpCollided[idx]);
				if (otherBox)
				{
					// Box
					// ray와 box와 충돌검사
					for (int i = 0; i < 3; i++) otherBox->m_box.Intersects(origins[i], direction, lengths[i]);
				}
				else
				{
					// Sphere
					auto otherSphere = dynamic_cast<SphereColliderComponent*>(c->m_vecpCollided[0]);
					// ray와 sphere와 충돌검사
					for (int i = 0; i < 3; i++) otherSphere->m_sphere.Intersects(origins[i], direction, lengths[i]);
				}

				// length가 0보다 크면 충돌점을 구한다.
				// **** origin이 충돌체 안에 파묻히는 문제가 생겨서 bias 값을 추가하기로 함 ****
				// **** bias 값은 1로 하고, direction 단위벡터를 사용 ****
				// **** 따라서 length가 bias 값보다 커야 함.
				int collisionCount = 0;
				XMFLOAT3 xmf3CollsionPoint[3];
				for (int i = 0; i < 3; i++)
				{
					if (1 < lengths[i]) // bias값 1
						xmf3CollsionPoint[collisionCount++] = Vector3::Add(
							xmf3Origins[i], Vector3::Multiply(lengths[i], xmf3Direction));
				}

				// 충돌점이 두 개 이상이면 충돌면의 노멀 벡터를 구한다.
				if (2 <= collisionCount)
				{
					XMVECTOR normal, adjusted, nonuse;

					// 충돌점이 두 개일 경우, 첫번째 충돌점에 (0, 1, 0)을 더하여 세번째 충돌점을 만든다.
					// 이렇게 하면 XZ평면에서의 노멀벡터를 구할 수 있다.
					if (2 == collisionCount)
					{
						xmf3CollsionPoint[2] = Vector3::Add(xmf3CollsionPoint[0], XMFLOAT3(0, 1, 0));

						// 점 ABC에 대해, AB벡터와 AC벡터의 외적을 하여 노멀벡터를 구한다.
						// 노멀벡터를 구한다.
						normal = XMVector3Cross(
							XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]),
							XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0])
						);
						normal = XMVector3Normalize(normal);
					}

					// 충돌점이 세 개일 경우, 세 개의 점이 모두 y값이 같아 하나의 평면으로 구해지지 않기 때문에
					// 첫번째 충돌점의 y값에 1을 더한다.
					else
					{
						xmf3CollsionPoint[0].y += 1;

						// 점 ABC에 대해, AB벡터와 AC벡터의 외적을 하여 노멀벡터를 구한다.
						// 노멀벡터를 구한다.
						normal = XMVector3Cross(
							XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]),
							XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0])
						);
						normal = XMVector3Normalize(normal);

						// 만약, normal의 y값이 0이 아니면 [0][1][1], 또는 [1][1][2]로 새로 normal을 구해야 한다.
						XMFLOAT3 test;
						XMStoreFloat3(&test, normal);
						if (0 != test.y)
						{
							xmf3CollsionPoint[2] = xmf3CollsionPoint[1];
							xmf3CollsionPoint[2].y += 1;

							normal = XMVector3Cross(
								XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]),
								XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0])
							);
							normal = XMVector3Normalize(normal);

							XMStoreFloat3(&test, normal);
							transform->SetPosition(Vector3::Add(xmf3CurrPosition, Vector3::Multiply(0.04f, test)));
							continue;
						}
					}

					// XMVector3ComponentsFromNormal()은 벡터 v를 노멀벡터 n에 수평, 수직하는 벡터 둘로 나눠서 반환해준다.
					// adjusted 벡터에 수직값을 넣어준다. 투영값.
					XMVector3ComponentsFromNormal(&nonuse, &adjusted, XMLoadFloat3(&xmf3CurrVector), normal);

					XMStoreFloat3(&xmf3CurrVector, adjusted);

					//// 이전 위치에 adjusted 벡터 값을 더해서 구한 새 위치를 현재 위치로 변경한다.
					transform->SetPosition(Vector3::Add(m_xmf3PrevPosition, xmf3CurrVector));
				}

				// 충돌점이 한 개면 모서리가 확실한 상황.
				// 이럴땐, 몇 번째 Ray([0]인지, [2]인지)인지 확인하고
				// 그 만큼 옆으로 밀어서 보정해보는 것이?
				else if (1 == collisionCount)
				{
					if (1 < lengths[0])
					{
						// Ray[0]
						transform->SetPosition(Vector3::Add(xmf3CurrPosition,
						                                    Vector3::Multiply(
							                                    Vector3::Length(xmf3CurrVector), xmf3Vertical)));
					}
					else if (1 < lengths[2])
					{
						// Ray[2]
						transform->SetPosition(Vector3::Add(xmf3CurrPosition,
						                                    Vector3::Multiply(
							                                    -Vector3::Length(xmf3CurrVector), xmf3Vertical)));
					}
				}
			}
		}
	});
}

// Rigidbody::Update()가 이동하는 컴포넌트들 보다 먼저 실행되어야 함!!
void RigidbodyComponent::Update(float fTimeElapsed)
{
	m_xmf3PrevPosition = m_pObject->FindComponent<TransformComponent>()->GetPosition();
}
