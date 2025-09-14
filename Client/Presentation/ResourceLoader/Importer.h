#pragma once
#include "Presentation/Renderer/Elements/Light.h"

class Object;
class Material;
class MeshData;
struct AnimationClip;
struct Key;

struct VertexIdx
{
	int vid, vtid, vnid;
};

struct OBJECT_DESC
{
	XMFLOAT3 position = XMFLOAT3(0, 0, 0);
	XMFLOAT3 rotation = XMFLOAT3(0, 0, 0);
	string model;
	string material = "defaultMaterial";
	bool isMaterial = false;
	bool isAnimated = false;
};


class IImporter
{
protected:
	static XMFLOAT3 GetFloat3(stringstream& ss);
	static XMFLOAT2 GetFloat2(stringstream& ss);
	static float GetFloat(stringstream& ss);
	static bool GetBool(stringstream& ss);
	static string GetPath(stringstream& ss);
	static VertexIdx GetIdx(stringstream& ss);
	static XMFLOAT4X4 GetMatrix(const float* fIn, int& offset);
	static Key GetKeyframe(const float* fIn, int& offset);
};


class ObjectDataImporter : public IImporter
{
public:
	//static vector<OBJECT_DESC> Load(const char* file_path);
};

class LightDataImporter : public IImporter
{
public:
	static vector<LightDescription> Load(const char* file_path);
};

class MeshDataImporter : public IImporter
{
public:
	static vector<MeshData> Load(const char* file_path);
	static vector<MeshData> FbxLoad(const char* file_path);
};

class MaterialDataImporter : public IImporter
{
public:
	static void Load(const char* name);
};

class AssetListDataImporter : public IImporter
{
public:
	static void Load(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
	                 D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
};

class AnimClipDataImporter : public IImporter
{
public:
	static AnimationClip Load(const char* asset_name);
};
