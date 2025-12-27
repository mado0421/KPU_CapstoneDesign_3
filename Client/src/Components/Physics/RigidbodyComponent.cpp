#include "pch.h"
#include "RigidbodyComponent.h"
#include "../../Engine/ECS/Components.h"
#include "../../Engine/ECS/Object.h"

RigidbodyComponent::RigidbodyComponent(Object* pObject) : Component(pObject), m_xmf3PrevPosition(XMFLOAT3(0, 0, 0)) {}

RigidbodyComponent::~RigidbodyComponent() {}

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

                // ray direction占쏙옙 占쏙옙占싼댐옙.
                TransformComponent* transform        = m_pObject->FindComponent<TransformComponent>();
                XMFLOAT3            xmf3CurrPosition = transform->GetPosition();
                XMFLOAT3            xmf3CurrVector   = Vector3::Subtract(xmf3CurrPosition, m_xmf3PrevPosition);
                if (0 == Vector3::Length(xmf3CurrVector)) continue;
                XMFLOAT3 xmf3Direction = Vector3::Normalize(xmf3CurrVector);

                // ray占쏙옙 origin占쏙옙 currPosition占쏙옙 currPosition +- xmf3Vertical * m_sphere.radius 占쏙옙 占쏙옙표占쏙옙
                // direction占쏙옙 UpVector占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙(direction占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙)占쏙옙 +/-0.5占쏙옙큼 占쏙옙칼占쏙옙占쏙옙占?占쏙옙占싶몌옙 占쏙옙占쏙옙占쏙옙占?占쏙옙.
                XMFLOAT3 xmf3Vertical   = Vector3::CrossProduct(xmf3Direction, XMFLOAT3(0, 1, 0));
                XMFLOAT3 xmf3Origins[3] = {Vector3::Add(xmf3CurrPosition, Vector3::Multiply(-0.5f, xmf3Vertical)), xmf3CurrPosition, Vector3::Add(xmf3CurrPosition, Vector3::Multiply(0.5f, xmf3Vertical))};

                // **** origin占쏙옙 占썸돌체 占싫울옙 占식뱄옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌤쇽옙 bias 占쏙옙占쏙옙 占쌩곤옙占싹깍옙占?占쏙옙 ****
                // **** bias 占쏙옙占쏙옙 1占쏙옙 占싹곤옙, direction 占쏙옙占쏙옙占쏙옙占싶몌옙 占쏙옙占?****
                for (int i = 0; i < 3; i++) xmf3Origins[i] = Vector3::Add(xmf3Origins[i], Vector3::Multiply(-1, xmf3Direction));

                XMVECTOR origins[3] = {XMLoadFloat3(&xmf3Origins[0]), XMLoadFloat3(&xmf3Origins[1]), XMLoadFloat3(&xmf3Origins[2])};
                XMVECTOR direction  = XMLoadFloat3(&xmf3Direction);
                float    lengths[3] = {0, 0, 0};

                // 占썸돌占쏙옙 占쏙옙諛?BoxCollider占쏙옙占쏙옙, SphereCollider占쏙옙占쏙옙 확占쏙옙占쌔억옙 占쏙옙.
                auto otherBox = dynamic_cast<BoxColliderComponent*>(c->m_vecpCollided[idx]);
                if (otherBox)
                {
                    // Box
                    // ray占쏙옙 box占쏙옙 占썸돌占싯삼옙
                    for (int i = 0; i < 3; i++) otherBox->m_box.Intersects(origins[i], direction, lengths[i]);
                }
                else
                {
                    // Sphere
                    auto otherSphere = dynamic_cast<SphereColliderComponent*>(c->m_vecpCollided[0]);
                    // ray占쏙옙 sphere占쏙옙 占썸돌占싯삼옙
                    for (int i = 0; i < 3; i++) otherSphere->m_sphere.Intersects(origins[i], direction, lengths[i]);
                }

                // length占쏙옙 0占쏙옙占쏙옙 크占쏙옙 占썸돌占쏙옙占쏙옙 占쏙옙占싼댐옙.
                // **** origin占쏙옙 占썸돌체 占싫울옙 占식뱄옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쏙옙占쌤쇽옙 bias 占쏙옙占쏙옙 占쌩곤옙占싹깍옙占?占쏙옙 ****
                // **** bias 占쏙옙占쏙옙 1占쏙옙 占싹곤옙, direction 占쏙옙占쏙옙占쏙옙占싶몌옙 占쏙옙占?****
                // **** 占쏙옙占쏙옙 length占쏙옙 bias 占쏙옙占쏙옙占쏙옙 커占쏙옙 占쏙옙.
                int      collisionCount = 0;
                XMFLOAT3 xmf3CollsionPoint[3];
                for (int i = 0; i < 3; i++)
                {
                    if (1 < lengths[i]) // bias占쏙옙 1
                        xmf3CollsionPoint[collisionCount++] = Vector3::Add(xmf3Origins[i], Vector3::Multiply(lengths[i], xmf3Direction));
                }

                // 占썸돌占쏙옙占쏙옙 占쏙옙 占쏙옙 占싱삼옙占싱몌옙 占썸돌占쏙옙占쏙옙 占쏙옙占?占쏙옙占싶몌옙 占쏙옙占싼댐옙.
                if (2 <= collisionCount)
                {
                    XMVECTOR normal, adjusted, nonuse;

                    // 占썸돌占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏙옙占? 첫占쏙옙째 占썸돌占쏙옙占쏙옙 (0, 1, 0)占쏙옙 占쏙옙占싹울옙 占쏙옙占쏙옙째 占썸돌占쏙옙占쏙옙 占쏙옙占쏙옙占?
                    // 占싱뤄옙占쏙옙 占싹몌옙 XZ占쏙옙涌∽옙占쏙옙占?占쏙옙嶺占쏙옙拷占?占쏙옙占쏙옙 占쏙옙 占쌍댐옙.
                    if (2 == collisionCount)
                    {
                        xmf3CollsionPoint[2] = Vector3::Add(xmf3CollsionPoint[0], XMFLOAT3(0, 1, 0));

                        // 占쏙옙 ABC占쏙옙 占쏙옙占쏙옙, AB占쏙옙占싶울옙 AC占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占싹울옙 占쏙옙嶺占쏙옙拷占?占쏙옙占싼댐옙.
                        // 占쏙옙嶺占쏙옙拷占?占쏙옙占싼댐옙.
                        normal = XMVector3Cross(XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]), XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0]));
                        normal = XMVector3Normalize(normal);
                    }

                    // 占썸돌占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏙옙占? 占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占?y占쏙옙占쏙옙 占쏙옙占쏙옙 占싹놂옙占쏙옙 占쏙옙占쏙옙占쏙옙占?占쏙옙占쏙옙占쏙옙占쏙옙 占십깍옙 占쏙옙占쏙옙占쏙옙
                    // 첫占쏙옙째 占썸돌占쏙옙占쏙옙 y占쏙옙占쏙옙 1占쏙옙 占쏙옙占싼댐옙.
                    else
                    {
                        xmf3CollsionPoint[0].y += 1;

                        // 占쏙옙 ABC占쏙옙 占쏙옙占쏙옙, AB占쏙옙占싶울옙 AC占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占싹울옙 占쏙옙嶺占쏙옙拷占?占쏙옙占싼댐옙.
                        // 占쏙옙嶺占쏙옙拷占?占쏙옙占싼댐옙.
                        normal = XMVector3Cross(XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]), XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0]));
                        normal = XMVector3Normalize(normal);

                        // 占쏙옙占쏙옙, normal占쏙옙 y占쏙옙占쏙옙 0占쏙옙 占싣니몌옙 [0][1][1], 占실댐옙 [1][1][2]占쏙옙 占쏙옙占쏙옙 normal占쏙옙 占쏙옙占쌔억옙 占싼댐옙.
                        XMFLOAT3 test;
                        XMStoreFloat3(&test, normal);
                        if (0 != test.y)
                        {
                            xmf3CollsionPoint[2] = xmf3CollsionPoint[1];
                            xmf3CollsionPoint[2].y += 1;

                            normal = XMVector3Cross(XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]), XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0]));
                            normal = XMVector3Normalize(normal);

                            XMStoreFloat3(&test, normal);
                            transform->SetPosition(Vector3::Add(xmf3CurrPosition, Vector3::Multiply(0.04f, test)));
                            continue;
                        }
                    }

                    // XMVector3ComponentsFromNormal()占쏙옙 占쏙옙占쏙옙 v占쏙옙 占쏙옙嶺占쏙옙占?n占쏙옙 占쏙옙占쏙옙, 占쏙옙占쏙옙占싹댐옙 占쏙옙占쏙옙 占싼뤄옙 占쏙옙占쏙옙占쏙옙 占쏙옙환占쏙옙占쌔댐옙.
                    // adjusted 占쏙옙占싶울옙 占쏙옙占쏙옙占쏙옙占쏙옙 占쌍억옙占쌔댐옙. 占쏙옙占쏙옙占쏙옙.
                    XMVector3ComponentsFromNormal(&nonuse, &adjusted, XMLoadFloat3(&xmf3CurrVector), normal);

                    XMStoreFloat3(&xmf3CurrVector, adjusted);

                    //// 占쏙옙占쏙옙 占쏙옙치占쏙옙 adjusted 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쌔쇽옙 占쏙옙占쏙옙 占쏙옙 占쏙옙치占쏙옙 占쏙옙占쏙옙 占쏙옙치占쏙옙 占쏙옙占쏙옙占싼댐옙.
                    transform->SetPosition(Vector3::Add(m_xmf3PrevPosition, xmf3CurrVector));
                }

                // 占썸돌占쏙옙占쏙옙 占쏙옙 占쏙옙占쏙옙 占쏜서몌옙占쏙옙 확占쏙옙占쏙옙 占쏙옙황.
                // 占싱뤄옙占쏙옙, 占쏙옙 占쏙옙째 Ray([0]占쏙옙占쏙옙, [2]占쏙옙占쏙옙)占쏙옙占쏙옙 확占쏙옙占싹곤옙
                // 占쏙옙 占쏙옙큼 占쏙옙占쏙옙占쏙옙 占싻어서 占쏙옙占쏙옙占쌔븝옙占쏙옙 占쏙옙占쏙옙?
                else if (1 == collisionCount)
                {
                    if (1 < lengths[0])
                    {
                        // Ray[0]
                        transform->SetPosition(Vector3::Add(xmf3CurrPosition, Vector3::Multiply(Vector3::Length(xmf3CurrVector), xmf3Vertical)));
                    }
                    else if (1 < lengths[2])
                    {
                        // Ray[2]
                        transform->SetPosition(Vector3::Add(xmf3CurrPosition, Vector3::Multiply(-Vector3::Length(xmf3CurrVector), xmf3Vertical)));
                    }
                }
            }
        }
    });
}

// Rigidbody::Update()占쏙옙 占싱듸옙占싹댐옙 占쏙옙占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙퓸占쏙옙 占쏙옙!!
void RigidbodyComponent::Update(float fTimeElapsed) { m_xmf3PrevPosition = m_pObject->FindComponent<TransformComponent>()->GetPosition(); }
