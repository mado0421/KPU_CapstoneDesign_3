#include "pch.h"
#include "RigidbodyComponent.h"
#include "../../Core/Components.h"
#include "../../Core/Object.h"

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

                // ray direction�� ���Ѵ�.
                TransformComponent* transform        = m_pObject->FindComponent<TransformComponent>();
                XMFLOAT3            xmf3CurrPosition = transform->GetPosition();
                XMFLOAT3            xmf3CurrVector   = Vector3::Subtract(xmf3CurrPosition, m_xmf3PrevPosition);
                if (0 == Vector3::Length(xmf3CurrVector)) continue;
                XMFLOAT3 xmf3Direction = Vector3::Normalize(xmf3CurrVector);

                // ray�� origin�� currPosition�� currPosition +- xmf3Vertical * m_sphere.radius �� ��ǥ��
                // direction�� UpVector�� ������ ����(direction�� ������ ����)�� +/-0.5��ŭ ��Į����� ���͸� ������� ��.
                XMFLOAT3 xmf3Vertical   = Vector3::CrossProduct(xmf3Direction, XMFLOAT3(0, 1, 0));
                XMFLOAT3 xmf3Origins[3] = {Vector3::Add(xmf3CurrPosition, Vector3::Multiply(-0.5f, xmf3Vertical)), xmf3CurrPosition, Vector3::Add(xmf3CurrPosition, Vector3::Multiply(0.5f, xmf3Vertical))};

                // **** origin�� �浹ü �ȿ� �Ĺ����� ������ ���ܼ� bias ���� �߰��ϱ�� �� ****
                // **** bias ���� 1�� �ϰ�, direction �������͸� ��� ****
                for (int i = 0; i < 3; i++) xmf3Origins[i] = Vector3::Add(xmf3Origins[i], Vector3::Multiply(-1, xmf3Direction));

                XMVECTOR origins[3] = {XMLoadFloat3(&xmf3Origins[0]), XMLoadFloat3(&xmf3Origins[1]), XMLoadFloat3(&xmf3Origins[2])};
                XMVECTOR direction  = XMLoadFloat3(&xmf3Direction);
                float    lengths[3] = {0, 0, 0};

                // �浹�� ��밡 BoxCollider����, SphereCollider���� Ȯ���ؾ� ��.
                auto otherBox = dynamic_cast<BoxColliderComponent*>(c->m_vecpCollided[idx]);
                if (otherBox)
                {
                    // Box
                    // ray�� box�� �浹�˻�
                    for (int i = 0; i < 3; i++) otherBox->m_box.Intersects(origins[i], direction, lengths[i]);
                }
                else
                {
                    // Sphere
                    auto otherSphere = dynamic_cast<SphereColliderComponent*>(c->m_vecpCollided[0]);
                    // ray�� sphere�� �浹�˻�
                    for (int i = 0; i < 3; i++) otherSphere->m_sphere.Intersects(origins[i], direction, lengths[i]);
                }

                // length�� 0���� ũ�� �浹���� ���Ѵ�.
                // **** origin�� �浹ü �ȿ� �Ĺ����� ������ ���ܼ� bias ���� �߰��ϱ�� �� ****
                // **** bias ���� 1�� �ϰ�, direction �������͸� ��� ****
                // **** ���� length�� bias ������ Ŀ�� ��.
                int      collisionCount = 0;
                XMFLOAT3 xmf3CollsionPoint[3];
                for (int i = 0; i < 3; i++)
                {
                    if (1 < lengths[i]) // bias�� 1
                        xmf3CollsionPoint[collisionCount++] = Vector3::Add(xmf3Origins[i], Vector3::Multiply(lengths[i], xmf3Direction));
                }

                // �浹���� �� �� �̻��̸� �浹���� ��� ���͸� ���Ѵ�.
                if (2 <= collisionCount)
                {
                    XMVECTOR normal, adjusted, nonuse;

                    // �浹���� �� ���� ���, ù��° �浹���� (0, 1, 0)�� ���Ͽ� ����° �浹���� �����.
                    // �̷��� �ϸ� XZ��鿡���� ��ֺ��͸� ���� �� �ִ�.
                    if (2 == collisionCount)
                    {
                        xmf3CollsionPoint[2] = Vector3::Add(xmf3CollsionPoint[0], XMFLOAT3(0, 1, 0));

                        // �� ABC�� ����, AB���Ϳ� AC������ ������ �Ͽ� ��ֺ��͸� ���Ѵ�.
                        // ��ֺ��͸� ���Ѵ�.
                        normal = XMVector3Cross(XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]), XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0]));
                        normal = XMVector3Normalize(normal);
                    }

                    // �浹���� �� ���� ���, �� ���� ���� ��� y���� ���� �ϳ��� ������� �������� �ʱ� ������
                    // ù��° �浹���� y���� 1�� ���Ѵ�.
                    else
                    {
                        xmf3CollsionPoint[0].y += 1;

                        // �� ABC�� ����, AB���Ϳ� AC������ ������ �Ͽ� ��ֺ��͸� ���Ѵ�.
                        // ��ֺ��͸� ���Ѵ�.
                        normal = XMVector3Cross(XMLoadFloat3(&xmf3CollsionPoint[1]) - XMLoadFloat3(&xmf3CollsionPoint[0]), XMLoadFloat3(&xmf3CollsionPoint[2]) - XMLoadFloat3(&xmf3CollsionPoint[0]));
                        normal = XMVector3Normalize(normal);

                        // ����, normal�� y���� 0�� �ƴϸ� [0][1][1], �Ǵ� [1][1][2]�� ���� normal�� ���ؾ� �Ѵ�.
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

                    // XMVector3ComponentsFromNormal()�� ���� v�� ��ֺ��� n�� ����, �����ϴ� ���� �ѷ� ������ ��ȯ���ش�.
                    // adjusted ���Ϳ� �������� �־��ش�. ������.
                    XMVector3ComponentsFromNormal(&nonuse, &adjusted, XMLoadFloat3(&xmf3CurrVector), normal);

                    XMStoreFloat3(&xmf3CurrVector, adjusted);

                    //// ���� ��ġ�� adjusted ���� ���� ���ؼ� ���� �� ��ġ�� ���� ��ġ�� �����Ѵ�.
                    transform->SetPosition(Vector3::Add(m_xmf3PrevPosition, xmf3CurrVector));
                }

                // �浹���� �� ���� �𼭸��� Ȯ���� ��Ȳ.
                // �̷���, �� ��° Ray([0]����, [2]����)���� Ȯ���ϰ�
                // �� ��ŭ ������ �о �����غ��� ����?
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

// Rigidbody::Update()�� �̵��ϴ� ������Ʈ�� ���� ���� ����Ǿ�� ��!!
void RigidbodyComponent::Update(float fTimeElapsed) { m_xmf3PrevPosition = m_pObject->FindComponent<TransformComponent>()->GetPosition(); }
