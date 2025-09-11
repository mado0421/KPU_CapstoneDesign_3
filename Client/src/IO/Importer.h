#pragma once
#include "../Graphics/Light.h"

class Object;
class Material;

class MeshData;

struct VertexIdx
{
    int vid, vtid, vnid;
};

struct OBJECT_DESC
{
    XMFLOAT3 position   = XMFLOAT3(0, 0, 0);
    XMFLOAT3 rotation   = XMFLOAT3(0, 0, 0);
    string   model      = "";
    string   material   = "defaultMaterial";
    bool     isMaterial = false;
    bool     isAnimated = false;
};

struct Keyframe;

class IImporter
{
protected:
    XMFLOAT3  GetFloat3(stringstream& ss);
    XMFLOAT2  GetFloat2(stringstream& ss);
    float     GetFloat(stringstream& ss);
    bool      GetBool(stringstream& ss);
    string    GetPath(stringstream& ss);
    VertexIdx GetIdx(stringstream& ss);

    XMFLOAT4X4 GetMatrix(const float* fIn, int& offset);
    Keyframe   GetKeyframe(const float* fIn, int& offset);
};


class ObjectDataImporter : public IImporter
{
public:
    vector<OBJECT_DESC> Load(const char* filePath);
};

class LightDataImporter : public IImporter
{
public:
    vector<LIGHT_DESC> Load(const char* filePath);
};

class MeshDataImporter : public IImporter
{
public:
    vector<MeshData> Load(const char* filePath);
    vector<MeshData> FBXLoad(const char* filePath);
};

class MaterialDataImporter : public IImporter
{
public:
    void Load(const char* filePath);
};

class AssetListDataImporter : public IImporter
{
public:
    void Load(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
};

struct AnimClip;

class AnimClipDataImporter : public IImporter
{
public:
    AnimClip Load(const char* filePath);
};
