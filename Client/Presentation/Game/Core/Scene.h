#pragma once
#include "pch.h"

class GameObject;
class Screen;
class Camera;
class RenderToTextureClass;
class Framework;
class LightManager;
class TextureManager;

struct ConstantBufferPassInfo
{
	XMFLOAT4X4 m_xmf4x4CameraView;
	XMFLOAT4X4 m_xmf4x4CameraProjection;
	XMFLOAT4X4 m_xmf4x4CameraViewInv;
	XMFLOAT4X4 m_xmf4x4CameraProjectionInv;
	XMFLOAT4X4 m_xmf4x4TextureTransform;
	XMFLOAT3   m_xmf3CameraPosition;
	float      m_xmfCurrentTime;
};

struct EnvironmentObjectData
{
	string   strMeshName;
	string   strMatName;
	XMFLOAT3 xmf3Position;
	XMFLOAT4 xmf4Rotation;
};

struct ColliderObjectData
{
	XMFLOAT3 xmf3Position;
	XMFLOAT3 xmf3Extents;
	XMFLOAT4 xmf4Rotation;
};

namespace LoadMy
{
	vector<string>                Split(istringstream& ss, char delim);
	vector<EnvironmentObjectData> LoadEnvMeshList(const char* path);
	vector<ColliderObjectData>    LoadColliderList(const char* path);
}

enum class RenderGroup
{
	OBJECT,
	ANIMATED,
	PARTICLE,
	EFFECT
};

class Scene
{
protected:
	ID3D12RootSignature*       root_signature_          = nullptr;
	ID3D12Device*              device_                  = nullptr;
	ID3D12GraphicsCommandList* command_list_            = nullptr;
	ID3D12DescriptorHeap*      cbv_srv_descriptor_heap_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE cbv_cpu_descriptor_start_handle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_gpu_descriptor_start_handle_ = {};
	D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_descriptor_start_handle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_descriptor_start_handle_ = {};


	vector<GameObject*> objects_;

	// for Render
	vector<GameObject*> non_anim_object_render_group_;
	vector<GameObject*> anim_object_render_group_;
	vector<GameObject*> effect_render_group_;
	vector<GameObject*> ui_render_group_;

	// for Particle
	vector<GameObject*> particle_emitters_;

	// for Camera
	GameObject* camera_object_ = nullptr;

	vector<Screen*>                             screens_;
	unordered_map<string, ID3D12PipelineState*> pipeline_states_;

	LightManager* light_manager_ = nullptr;

	ID3D12Resource*             uab_hdr_avg_lum_            = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_gpu_uab_hdr_avg_lum_handle_ = {};
	ID3D12Resource*             cb_pass_info_              = nullptr;
	ConstantBufferPassInfo*     cb_mapped_pass_info_           = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE cbv_gpu_pass_info_handle_     = {};

	Framework* framework_     = nullptr;
	Camera*    camera_      = nullptr;
	float      current_time_ = 0;

	bool test_mouse_usable_ = true;


public:
	vector<GameObject*> particle_pool;

	int          event_count    = 0; //enemyDown
	bool         test          = false;
	int          start_end_state = 0; // 0: main, 1: start, 2: end
	virtual void Init(Framework* framework, ID3D12Device* device, ID3D12GraphicsCommandList* command_list);

	virtual void CheckCollision();
	virtual void SolveConstraint();
	virtual void Input(UCHAR* key_buffer);
	virtual void Update(float delta_time);
	virtual void Render(D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv, D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_dsv);

	virtual void Release();

	void Clear();
	void Victory();
	void Defeat();

	void    AddObject(GameObject* object, RenderGroup render_group);
	void    DeleteObject(GameObject* object);
	GameObject* FindObjectByName(const char*);

protected:
	virtual ID3D12RootSignature* CreateRootSignature();
	virtual void                 CreateDescriptorHeap();

	virtual void CreateCBV()
	{
	}

	virtual void CreateSRV()
	{
	}

	virtual void CreatePSO();

	void CreatePassInfoShaderResource();
	void UpdatePassInfoAboutCamera();

	void BuildObject();

	/// <summary>
	///		For Test
	/// </summary>
	void ReloadLight();	

	void LoadLevelEnvironment();

	void CreateEnvObject(const char* strModelName, const char* strMaterialName, XMFLOAT3 pos, XMFLOAT4 rot);
	void CreateEnvObject(EnvironmentObjectData objData);

	void CreateCollider(ColliderObjectData colData);

	void CreateTargetBoard(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool initialStateDied);
	void CreateDoor(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool isOpen);
};
