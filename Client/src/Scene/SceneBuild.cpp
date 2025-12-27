#include "pch.h"
#include "Scene.h"
#include "../Engine/ECS/Object.h"
#include "../Engine/ECS/Components.h"
#include "../IO/Importer.h"
#include "../Renderer/Elements/Model.h"
#include "../Renderer/Elements/Texture.h"

#include <fstream>
#include <sstream>

using namespace std;

#define MAX_PARTICLE_NUM 1000

vector<string> LoadMy::Split(istringstream& ss, const char delim)
{
    vector<string> result;
    string         str;

    while (getline(ss, str, delim)) result.push_back(str);

    return result;
}

vector<EnvironmentObjectData> LoadMy::LoadEnvMeshList(const char* path)
{
    vector<EnvironmentObjectData> result;

    string fullPath = "Resources/";
    fullPath += "/EnvMeshList.txt";
    ifstream ifs(fullPath);
    if (ifs.fail()) cout << "Error\n";
    else
    {
        string s;
        ifs >> s;
        ifs >> s;
        int n = stoi(s);

        for (int i = 0; i < n; i++)
        {
            EnvironmentObjectData temp = {};
            ifs >> s;
            temp.strMeshName = s;

            ifs >> s;
            {
                istringstream  ss(s);
                vector<string> pos  = Split(ss, ',');
                temp.xmf3Position.x = stof(pos[0]);
                temp.xmf3Position.y = stof(pos[1]);
                temp.xmf3Position.z = stof(pos[2]);
            }

            ifs >> s;
            {
                istringstream  ss(s);
                vector<string> pos  = Split(ss, ',');
                temp.xmf4Rotation.x = stof(pos[0]);
                temp.xmf4Rotation.y = stof(pos[1]);
                temp.xmf4Rotation.z = stof(pos[2]);
                temp.xmf4Rotation.w = stof(pos[3]);
            }

            ifs >> s;
            temp.strMatName = s;

            result.push_back(temp);
        }
        ifs.close();
    }

    return result;
}

vector<ColliderObjectData> LoadMy::LoadColliderList(const char* path)
{
    vector<ColliderObjectData> result;

    string fullPath = "Resources/";
    fullPath += "/ColliderList.txt";
    ifstream ifs(fullPath);
    if (ifs.fail()) cout << "Error\n";
    else
    {
        string s;
        ifs >> s;
        ifs >> s;
        int n = stoi(s);

        for (int i = 0; i < n; i++)
        {
            ColliderObjectData temp = {};
            ifs >> s;
            {
                istringstream  ss(s);
                vector<string> pos  = Split(ss, ',');
                temp.xmf3Position.x = stof(pos[0]);
                temp.xmf3Position.y = stof(pos[1]);
                temp.xmf3Position.z = stof(pos[2]);
            }

            ifs >> s;
            {
                istringstream  ss(s);
                vector<string> pos = Split(ss, ',');
                temp.xmf3Extents.x = stof(pos[0]);
                temp.xmf3Extents.y = stof(pos[1]);
                temp.xmf3Extents.z = stof(pos[2]);
            }

            ifs >> s;
            {
                istringstream  ss(s);
                vector<string> pos  = Split(ss, ',');
                temp.xmf4Rotation.x = stof(pos[0]);
                temp.xmf4Rotation.y = stof(pos[1]);
                temp.xmf4Rotation.z = stof(pos[2]);
                temp.xmf4Rotation.w = stof(pos[3]);
            }

            result.push_back(temp);
        }
        ifs.close();
    }

    return result;
}

void Scene::BuildObject()
{
    // Particle Pool Initialize
    for (int i = 0; i < MAX_PARTICLE_NUM; i++)
    {
        auto ptc = new Object();
        auto t  = new TransformComponent(ptc);
        auto pc = new ParticleComponent(ptc, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
        ptc->SetActive(false);
        m_vecParticlePool.push_back(ptc);
    }

    BuildWeapons(nullptr);
    BuildPlayer();
    BuildEnemies();
    BuildProps();
    BuildTriggers();
    BuildUI();
}

void Scene::BuildWeapons(Object* player)
{
     // muzzle, empty object for weapon
    auto muzzle     = new Object("muzzle");
    auto mTransform = new TransformComponent(muzzle);
    auto effect     = new EffectComponent(muzzle);
    auto mrcm       = new MeshRendererComponent(muzzle, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

    mrcm->SetActive(false);
    mrcm->SetModelByName("muzzleFlash");
    mrcm->SetMaterialByName("MuzzleFlashMat");

    effect->SetDuration(0.05f);

    mTransform->Translate(0, 0.07f, 0.15f);
    m_vecObject.push_back(muzzle);
    m_vecEffectRenderGroup.push_back(muzzle);

    // weapon
    auto weapon = new Object("pistol");

    auto wTransform = new TransformComponent(weapon);
    auto wcc        = new WeaponControllerComponent(weapon, muzzle, nullptr);
    auto mrc        = new MeshRendererComponent(weapon, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

    mrc->SetModelByName("pistol");
    mrc->SetMaterialByName("PistolMat");

    m_vecObject.push_back(weapon);
    m_vecNonAnimObjectRenderGroup.push_back(weapon);
    muzzle->m_pParent = weapon;
}

void Scene::BuildPlayer()
{
    // player
    auto player = new Object("player"); 

    auto transform           = new TransformComponent(player);
    auto controller          = new InputManagerComponent(player);
    auto rigidbody           = new RigidbodyComponent(player);
    auto skinnedMeshRenderer = new SkinnedMeshRendererComponent(player, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
    
    Object* pistol = FindObjectByName("pistol");
    auto humanoidController  = new HumanoidControllerComponent(player, pistol);
    auto humanoidAnimator    = new HumanoidAnimatorComponent(player, "Humanoid_Idle");
    auto sphereCollider      = new SphereColliderComponent(player, XMFLOAT3(0, 0.5f, 0), 0.5f);

    skinnedMeshRenderer->SetModelByName("human");
    skinnedMeshRenderer->SetMaterialByName("DefaultMaterial");

    transform->Translate(-6.78999996, 0, 15.6700001);
    transform->RotateXYZDegree(0, 210, 0);

    m_vecObject.push_back(player);
    m_vecAnimObjectRenderGroup.push_back(player);
    
    if(pistol) pistol->m_pParent = player;

    {
        auto head = new Object("head");
        auto transform = new TransformComponent(head);
        transform->Translate(0, 1.5f, 0.0f);
        m_vecObject.push_back(head);
        head->m_pParent = player;
    }
    {
        auto look = new Object("lookAt");
        auto transform = new TransformComponent(look);
        transform->Translate(0, 1.5f, 5.0f);
        m_vecObject.push_back(look);
        look->m_pParent = player;
        player->FindComponent<HumanoidControllerComponent>()->SetLookAt(look);
    }
    {
        auto camera = new Object("camera");
        auto transform = new TransformComponent(camera);
        auto cam       = new CameraComponent(camera);

        cam->SetHeadAndLookAt(FindObjectByName("head"), FindObjectByName("lookAt"), XMFLOAT3(0.6f, 0.3f, -2.2f));
        camera->m_pParent = player;
        m_pCameraObject = camera;
        m_vecObject.push_back(camera);

        if(pistol) pistol->FindComponent<WeaponControllerComponent>()->SetCam(camera);
    }
}

void Scene::BuildEnemies()
{
    //FirstHallway
    CreateTargetBoard("t0", XMFLOAT3(-15.8400002, 0, 2.50999999), XMFLOAT3(0, 270, 0), true);

    //FirstRoom
    CreateTargetBoard("t1", XMFLOAT3(-24.0699997, 0, 5.28999996), XMFLOAT3(0, 270, 0), true);
    CreateTargetBoard("t3", XMFLOAT3(-24.2700005, 0, -0.639999986), XMFLOAT3(0, 270, 0), true);

    //SecondHallway
    CreateTargetBoard("t4", XMFLOAT3(-26.2800007, 0, 10.4700003), XMFLOAT3(0, 270, 0), true);
    CreateTargetBoard("t5", XMFLOAT3(-24.6700001, 0, 18.3500004), XMFLOAT3(0, 180, 0), true);

    //FinalRoom
    CreateTargetBoard("t6", XMFLOAT3(-11.4799995, 1.3, 33.0699997), XMFLOAT3(0, 0, 0), true);
    CreateTargetBoard("t8", XMFLOAT3(-2.5, 1.3, 35.8800011), XMFLOAT3(0, 90, 0), true);
    CreateTargetBoard("t9", XMFLOAT3(-6.11999989, 1.3, 33.0699997), XMFLOAT3(0, 0, 0), true);
}

void Scene::BuildProps()
{
    LoadLevelEnvironment();

    CreateDoor("d0", XMFLOAT3(-3, 0.0f, 6.2f), XMFLOAT3(0, 0, 0), true);
    CreateDoor("d1", XMFLOAT3(-16.7180004, 0, 2.38000011), XMFLOAT3(0, 90, 0), false);
    CreateDoor("d2", XMFLOAT3(-22.2059994, 0, 6.60099983), XMFLOAT3(0, 0, 0), false);
    CreateDoor("d3", XMFLOAT3(-22.9099998, 0, 19.2399998), XMFLOAT3(0, 0, 0), false);
    CreateDoor("d4", XMFLOAT3(-14.4899998, 0, 40.5599976), XMFLOAT3(0, 90, 0), true);
    CreateDoor("d5", XMFLOAT3(-3.1400001, 0, 40.7099991), XMFLOAT3(0, 90, 0), true);
    CreateDoor("d6", XMFLOAT3(-8.38860321, 0, 18.455431), XMFLOAT3(0, 20, 0), false);
}

void Scene::BuildTriggers()
{
    eventCount = 0;

    {
        // 1
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 0);

        transform->Translate(-4.48, 0, 0.7);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("d0"));
        auto close     = EventInfo(EVENT::DCLOSE, temp);
        auto triggerOn = EventInfo(EVENT::TRIGGER, nullptr);

        trigEvent->AddEvent(close);
        trigEvent->AddEvent(triggerOn);

        m_vecObject.push_back(trig);
    }
    {
        // 2
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 0);

        transform->Translate(-14.5600004, 0, 0.879999995);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t0"));
        auto i         = EventInfo(EVENT::EWAKE, temp);
        auto triggerOn = EventInfo(EVENT::TRIGGER, nullptr);

        trigEvent->AddEvent(i);
        trigEvent->AddEvent(triggerOn);

        m_vecObject.push_back(trig);
    }
    {
        // 3
        auto trig      = new Object();
        auto transform = new TransformComponent(trig);
        auto trigEvent = new EventComponent(trig, 1);

        transform->Translate(-8.81999969, 0, 39.6100006);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("d1"));
        auto i = EventInfo(EVENT::DOPEN, temp);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        temp2->push_back(FindObjectByName("t0"));
        i = EventInfo(EVENT::EDIED, temp2);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 4
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 1);

        transform->Translate(-19.2399998, 0.39199999, 0.949999988);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t1"));
        temp->push_back(FindObjectByName("t3"));
        auto i = EventInfo(EVENT::EWAKE, temp);
        trigEvent->AddEvent(i);

        i = EventInfo(EVENT::TRIGGER, nullptr);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 5
        auto trig      = new Object();
        auto transform = new TransformComponent(trig);
        auto trigEvent = new EventComponent(trig, 3);

        transform->Translate(-19.2399998, 0.39199999, 0.949999988);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t1"));
        temp->push_back(FindObjectByName("t3"));
        auto i = EventInfo(EVENT::EDIED, temp);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        temp2->push_back(FindObjectByName("d2"));
        i = EventInfo(EVENT::DOPEN, temp2);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 6
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 3);

        transform->Translate(-24.0100002, 0.39199999, 10.3000002);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t4"));
        temp->push_back(FindObjectByName("t5"));
        auto i = EventInfo(EVENT::EWAKE, temp);
        trigEvent->AddEvent(i);

        i = EventInfo(EVENT::TRIGGER, nullptr);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 7
        auto trig      = new Object();
        auto transform = new TransformComponent(trig);
        auto trigEvent = new EventComponent(trig, 5);

        transform->Translate(-19.2399998, 0.39199999, 0.949999988);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t4"));
        temp->push_back(FindObjectByName("t5"));
        auto i = EventInfo(EVENT::EDIED, temp);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        temp2->push_back(FindObjectByName("d3"));
        i = EventInfo(EVENT::DOPEN, temp2);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 8
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 5);

        transform->Translate(-8.18999958, 0.39199999, 39.4799995);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t6"));
        temp->push_back(FindObjectByName("t8"));
        temp->push_back(FindObjectByName("t9"));
        auto i = EventInfo(EVENT::EWAKE, temp);
        trigEvent->AddEvent(i);

        i = EventInfo(EVENT::TRIGGER, nullptr);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        temp2->push_back(FindObjectByName("d4"));
        temp2->push_back(FindObjectByName("d5"));
        auto close = EventInfo(EVENT::DCLOSE, temp2);
        trigEvent->AddEvent(close);

        m_vecObject.push_back(trig);
    }
    {
        // 9
        auto trig      = new Object();
        auto transform = new TransformComponent(trig);
        auto trigEvent = new EventComponent(trig, 8);

        auto temp = new vector<Object*>;
        temp->push_back(FindObjectByName("t6"));
        temp->push_back(FindObjectByName("t8"));
        temp->push_back(FindObjectByName("t9"));
        auto i = EventInfo(EVENT::EDIED, temp);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        temp2->push_back(FindObjectByName("d5"));
        temp2->push_back(FindObjectByName("d6"));
        i = EventInfo(EVENT::DOPEN, temp2);
        trigEvent->AddEvent(i);

        m_vecObject.push_back(trig);
    }
    {
        // 10
        auto trig        = new Object();
        auto transform   = new TransformComponent(trig);
        auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1), true);
        auto trigEvent   = new EventComponent(trig, 8);

        transform->Translate(-11.7200003, 0.39199999, 23.3899994);

        auto i = EventInfo(EVENT::TRIGGER, nullptr);
        trigEvent->AddEvent(i);

        auto temp2 = new vector<Object*>;
        auto close = EventInfo(EVENT::VICTORY, temp2);
        trigEvent->AddEvent(close);

        m_vecObject.push_back(trig);
    }
}

void Scene::BuildUI()
{
    {
        auto text = new Object("TextHpInfo");

        auto transform = new TransformComponent(text);
        auto TRC       = new TextRendererComponent(text, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

        TRC->Initialize("consolas");
        TRC->SetMaterialByName("font_consolasMat");
        TRC->SetText("+");
        TRC->SetSize(16);
        transform->Translate(960 - 16, 540 + 24, 0);

        m_vecObject.push_back(text);
        m_vecUIRenderGroup.push_back(text);
    }
    {
        auto text = new Object("TextHpInfo");

        auto transform = new TransformComponent(text);
        auto TRC       = new TextRendererComponent(text, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

        TRC->Initialize("consolas");
        TRC->SetMaterialByName("font_consolasMat");
        TRC->SetSize(10);
        auto tuiHP = new TextUIPlayerHPComponent(text, FindObjectByName("player"));
        transform->Translate(1200, 300, 0);

        m_vecObject.push_back(text);
        m_vecUIRenderGroup.push_back(text);
    }
    {
        auto text = new Object("TextAmmoInfo");

        auto transform = new TransformComponent(text);
        auto TRC       = new TextRendererComponent(text, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
        TRC->Initialize("consolas");
        TRC->SetMaterialByName("font_consolasMat");
        TRC->SetSize(8);
        auto tuiAmmo = new TextUIAmmoComponent(text, FindObjectByName("pistol"));
        transform->Translate(1200, 270, 0);

        m_vecObject.push_back(text);
        m_vecUIRenderGroup.push_back(text);
    }
}

void Scene::ReloadLight()
{
    LightDataImporter  lightDataImporter;
    vector<LIGHT_DESC> vecLightDesc = lightDataImporter.Load("Resources/LightData.txt");
    string             shadow("ShadowMap_");

    m_LightMng->DeleteAll();

    for (int i = 0; i < vecLightDesc.size(); i++)
    {
        switch (vecLightDesc[i].lightType)
        {
        case LIGHT_POINT: m_LightMng->AddPointLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
            break;
        case LIGHT_DIRECTIONAL: m_LightMng->AddDirectionalLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
            break;
        case LIGHT_SPOT: m_LightMng->AddSpotLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
            break;
        case LIGHT_NONE: default: break;
        }

        if (vecLightDesc[i].bIsShadow)
        {
            string temp = to_string(i);
            temp        = shadow + temp;

            g_texture_manager.DeleteTexture(temp.c_str());

            g_texture_manager.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
            m_LightMng->SetShadowMapName(temp.c_str(), i);
        }
    }
}

void Scene::LoadLevelEnvironment()
{
    vector<EnvironmentObjectData> envData = LoadMy::LoadEnvMeshList("Data");
    for (int i = 0; i < envData.size(); i++) CreateEnvObject(envData[i]);

    vector<ColliderObjectData> colliderData = LoadMy::LoadColliderList("Data");
    for (int i = 0; i < colliderData.size(); i++) CreateCollider(colliderData[i]);
}

void Scene::CreateEnvObject(const char* strModelName, const char* strMaterialName, XMFLOAT3 pos, XMFLOAT4 rot)
{
    auto env = new Object();

    auto transform = new TransformComponent(env);
    auto mrc       = new MeshRendererComponent(env, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

    mrc->SetModelByName(strModelName);
    mrc->SetMaterialByName(strMaterialName);

    transform->SetPosition(pos);
    transform->Rotate(rot);

    m_vecObject.push_back(env);
    m_vecNonAnimObjectRenderGroup.push_back(env);
}

void Scene::CreateEnvObject(EnvironmentObjectData objData) { CreateEnvObject(objData.strMeshName.c_str(), objData.strMatName.c_str(), objData.xmf3Position, objData.xmf4Rotation); }

void Scene::CreateCollider(ColliderObjectData colData)
{
    auto box = new Object();

    XMFLOAT3 extents(colData.xmf3Extents.x * 0.5f, colData.xmf3Extents.y * 0.5f, colData.xmf3Extents.z * 0.5f);

    auto transform   = new TransformComponent(box);
    auto boxCollider = new BoxColliderComponent(box, colData.xmf3Position, extents, colData.xmf4Rotation);

    m_vecObject.push_back(box);
}

void Scene::CreateTargetBoard(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool initialState)
{
    auto targetBoard = new Object(strName);

    auto transform           = new TransformComponent(targetBoard);
    auto skinnedMeshRenderer = new SkinnedMeshRendererComponent(targetBoard, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
    
    auto TCC          = new TargetBoardControllerComponent(targetBoard);
    auto TAC          = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
    auto bodyCollider = new BoxColliderComponent(targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
    auto headCollider = new BoxColliderComponent(targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f), XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

    skinnedMeshRenderer->SetModelByName("targetBoardStand");
    skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
    transform->Translate(position);
    transform->RotateXYZDegree(rotationAngle);
    if (initialState) TCC->Die();
    else TCC->Revive();

    TCC->SetPlayer(FindObjectByName("player"));

    m_vecObject.push_back(targetBoard);
    m_vecAnimObjectRenderGroup.push_back(targetBoard);
}

void Scene::CreateDoor(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool isOpen)
{
    auto d         = new Object(strName);
    auto transform = new TransformComponent(d);
    transform->Translate(position);
    transform->RotateXYZDegree(rotationAngle);

    auto boxCollider = new BoxColliderComponent(d, XMFLOAT3(0, 1.5, 0), XMFLOAT3(2.9, 3.6, 0.2), XMFLOAT4(0, 0, 0, 1));
    auto mrcm        = new MeshRendererComponent(d, m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
    mrcm->SetModelByName("level00_door");
    mrcm->SetMaterialByName("level00_1");

    auto dc = new DoorComponent(d, isOpen);
    m_vecObject.push_back(d);
    m_vecNonAnimObjectRenderGroup.push_back(d);
}
