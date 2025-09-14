#include "pch.h"
#include "Importer.h"
#include "Presentation/Renderer/Elements/Animation.h"
#include "Presentation/Renderer/Elements/Model.h"

#include "Presentation/Game/Manager/AnimationManager.h"
#include "Presentation/Game/Manager/MaterialManager.h"
#include "Presentation/Renderer/Elements/Mesh.h"


XMFLOAT3 IImporter::GetFloat3(stringstream& ss)
{
	XMFLOAT3 pos;

	ss >> pos.x;
	ss >> pos.y;
	ss >> pos.z;

	return pos;
}

XMFLOAT2 IImporter::GetFloat2(stringstream& ss)
{
	XMFLOAT2 xmf2;

	ss >> xmf2.x;
	ss >> xmf2.y;


	return xmf2;
}

float IImporter::GetFloat(stringstream& ss)
{
	float f;

	ss >> f;


	return f;
}

bool IImporter::GetBool(stringstream& ss)
{
	bool b;

	ss >> boolalpha >> b;


	return b;
}

string IImporter::GetPath(stringstream& ss)
{
	string path;

	ss >> path;

	return path;
}

VertexIdx IImporter::GetIdx(stringstream& ss)
{
	string token;
	VertexIdx output;
	const auto str_buff = new char[256];
	char* next_tok = nullptr;
	constexpr char seps[] = " ,\t\n/";

	getline(ss, token, ' ');
	strcpy_s(str_buff, 256, token.c_str());
	const char* tok = strtok_s(str_buff, seps, &next_tok);
	output.vid = atoi(tok) - 1;
	tok = strtok_s(next_tok, seps, &next_tok);
	output.vtid = atoi(tok) - 1;
	tok = strtok_s(next_tok, seps, &next_tok);
	output.vnid = atoi(tok) - 1;

	return output;
}

XMFLOAT4X4 IImporter::GetMatrix(const float* fIn, int& offset)
{
	int i = 0;
	XMFLOAT4X4 result;
	XMFLOAT4 r;
	XMFLOAT3 t;

	r.x = fIn[offset + i++];
	r.y = fIn[offset + i++];
	r.z = fIn[offset + i++];
	r.w = fIn[offset + i++];
	t.x = fIn[offset + i++];
	t.y = fIn[offset + i++];
	t.z = fIn[offset + i++];

	offset += i;

	XMStoreFloat4x4(&result, XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&r)),
	                                          XMMatrixTranslationFromVector(XMLoadFloat3(&t))));

	return result;
}

Key IImporter::GetKeyframe(const float* fIn, int& offset)
{
	Key result;
	int i = 0;
	result.rotation.x = fIn[offset + i++];
	result.rotation.y = fIn[offset + i++];
	result.rotation.z = fIn[offset + i++];
	result.rotation.w = fIn[offset + i++];
	result.translation.x = fIn[offset + i++];
	result.translation.y = fIn[offset + i++];
	result.translation.z = fIn[offset + i++];
	offset += i;
	return result;
}

vector<LightDescription> LightDataImporter::Load(const char* file_path)
{
	ifstream in(file_path);
	string s;
	string token;
	vector<LightDescription> vecLightDesc;

	if (!in.is_open())
	{
		assert(!"파일을 여는데 실패했습니다.");
		return vecLightDesc;
	}

	while (in)
	{
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token == "#End") break;

		if (token == "{")
		{
			LightDescription temp;
			while (token != "}")
			{
				getline(ss, token, ' ');

				if (token == "point") temp.lightType = LIGHT_POINT;
				else if (token == "dir") temp.lightType = LIGHT_DIRECTIONAL;
				else if (token == "spot") temp.lightType = LIGHT_SPOT;
				else if (token == "position") temp.xmf3Position = GetFloat3(ss);
				else if (token == "color") temp.xmf3Color = GetFloat3(ss);
				else if (token == "direction") temp.xmf3Direction = GetFloat3(ss);
				else if (token == "falloff") temp.xmf2Falloff = GetFloat2(ss);
				else if (token == "spotPower") temp.fSpotPower = GetFloat(ss);
				else if (token == "shadow") temp.bIsShadow = GetBool(ss);
			}
			vecLightDesc.push_back(temp);
		}
	}

	return vecLightDesc;
}

vector<MeshData> MeshDataImporter::Load(const char* file_path)
{
	vector<MeshData> vec_mesh_data;
	vector<XMFLOAT3> vec_control_point;
	vector<XMFLOAT3> vec_normal;
	vector<XMFLOAT2> vec_tex_coord;

	string path;
	string directory = "Resources/Meshes/";
	string extension = ".obj";

	path = directory + file_path;
	path += extension;

	int n_object = -1;

	ifstream in(path);
	string s;
	string token;
	if (!in.is_open())
		assert(!"파일을 여는데 실패했습니다.");

	/*
	ó�� ������!
	o�� ������ MESH_DATA�� �ϳ� ����� ������ ������ face�� �ű⿡ �߰��Ѵ�.
	o�� �� ������ MESH_DATA�� �ϳ� ����� idx�� �����ϸ� �ǰڴµ�?
	*/

	while (in)
	{
		string empty;
		getline(in, s);
		stringstream ss(s);

		getline(ss, token, ' ');
		if (token == "#") continue;
		if (token == "o")
		{
			s.replace(0, 2, empty);
			stringstream ss(s);

			MeshData temp;
			temp.name = GetPath(ss);
			vec_mesh_data.push_back(temp);
			n_object++;

			continue;
		}
		if (token == "v")
		{
			s.replace(0, 2, empty);
			stringstream ss(s);
			vec_control_point.push_back(GetFloat3(ss));

			continue;
		}
		if (token == "vt")
		{
			s.replace(0, 3, empty);
			stringstream ss(s);
			vec_tex_coord.push_back(GetFloat2(ss));

			continue;
		}
		if (token == "vn")
		{
			s.replace(0, 3, empty);
			stringstream ss(s);
			vec_normal.push_back(GetFloat3(ss));

			continue;
		}
		if (token == "s") continue;
		if (token == "f")
		{
			s.replace(0, 2, empty);
			stringstream ss(s);
			VertexIdx verIdx;
			verIdx = GetIdx(ss);
			vec_mesh_data[n_object].vertices.push_back(Vertex(vec_control_point[verIdx.vid], vec_normal[verIdx.vnid],
			                                                  vec_tex_coord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vec_mesh_data[n_object].vertices.push_back(Vertex(vec_control_point[verIdx.vid], vec_normal[verIdx.vnid],
			                                                  vec_tex_coord[verIdx.vtid]));

			verIdx = GetIdx(ss);
			vec_mesh_data[n_object].vertices.push_back(Vertex(vec_control_point[verIdx.vid], vec_normal[verIdx.vnid],
			                                                  vec_tex_coord[verIdx.vtid]));
		}
	}

	return vec_mesh_data;
}

struct CtrlPoint
{
	XMFLOAT3 position;
	unsigned int boneIndices[4];
	double weights[4];

	CtrlPoint() : position(XMFLOAT3(0, 0, 0))
	{
		int i = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		boneIndices[i++] = 0;
		i = 0;
		weights[i++] = 0;
		weights[i++] = 0;
		weights[i++] = 0;
		weights[i++] = 0;
	}
};

struct VertexForImport
{
	UINT ctrlPointIndex;
	XMFLOAT3 normal;
	XMFLOAT3 binormal;
	XMFLOAT3 tangent;
	XMFLOAT2 uv;

	VertexForImport() : ctrlPointIndex(0), normal(XMFLOAT3(0, 0, 0)), binormal(XMFLOAT3(0, 0, 0)),
	                    tangent(XMFLOAT3(0, 0, 0)), uv(XMFLOAT2(0, 0))
	{
	}
};

vector<MeshData> MeshDataImporter::FbxLoad(const char* file_path)
{
	vector<MeshData> vecMeshData;

	string ultimateOfPerfectFilePath;

	string fileHead = "Resources/Meshes/";
	string fileTail = ".mm"; // my mesh

	ultimateOfPerfectFilePath = fileHead + file_path;
	ultimateOfPerfectFilePath += fileTail;

	ifstream in;
	in.open(ultimateOfPerfectFilePath, ios::in | ios::binary);

	int nMesh;
	in.read(reinterpret_cast<char*>(&nMesh), sizeof(int));

	for (int iMesh = 0; iMesh < nMesh; iMesh++)
	{
		string name = "fbxMesh.";
		string num = to_string(iMesh);

		MeshData tempMesh;
		tempMesh.name = name + num;

		int nCtrlPoint;
		in.read(reinterpret_cast<char*>(&nCtrlPoint), sizeof(int));

		auto pCtrlPoint = new CtrlPoint[nCtrlPoint];
		in.read(reinterpret_cast<char*>(pCtrlPoint), sizeof(CtrlPoint) * nCtrlPoint);
		vector<CtrlPoint> vecCP;

		for (int iCP = 0; iCP < nCtrlPoint; ++iCP)
		{
			CtrlPoint temp;
			temp = pCtrlPoint[iCP];
			vecCP.push_back(temp);
		}

		int nVertex;
		in.read(reinterpret_cast<char*>(&nVertex), sizeof(int));
		vector<Vertex> vecVertex;

		auto pVertex = new VertexForImport[nVertex];
		in.read(reinterpret_cast<char*>(pVertex), sizeof(VertexForImport) * nVertex);

		for (int iV = 0; iV < nVertex; ++iV)
		{
			VertexForImport v;
			v = pVertex[iV];
			Vertex temp;

			temp.position = vecCP[v.ctrlPointIndex].position;
			temp.normal = v.normal;
			temp.tangent = v.tangent;
			temp.uv = v.uv;
			temp.bone_indices.x = vecCP[v.ctrlPointIndex].boneIndices[0];
			temp.bone_indices.y = vecCP[v.ctrlPointIndex].boneIndices[1];
			temp.bone_indices.z = vecCP[v.ctrlPointIndex].boneIndices[2];
			temp.bone_indices.w = vecCP[v.ctrlPointIndex].boneIndices[3];
			temp.bone_weights.x = vecCP[v.ctrlPointIndex].weights[0];
			temp.bone_weights.y = vecCP[v.ctrlPointIndex].weights[1];
			temp.bone_weights.z = vecCP[v.ctrlPointIndex].weights[2];
			temp.bone_weights.w = vecCP[v.ctrlPointIndex].weights[3];
			tempMesh.vertices.push_back(temp);
		}

		vecMeshData.push_back(tempMesh);
	}

	in.close();

	return vecMeshData;
}

void MaterialDataImporter::Load(const char* name)
{
	string path = "Resources/";
	string extension = ".txt";


	ifstream in(path + name + extension);
	string s;
	string token;

	if (!in.is_open())
	{
		assert(!"파일을 여는데 실패했습니다.");
		return;
	}

	while (in)
	{
		getline(in, s);

		stringstream ss(s);
		getline(ss, token, ' ');
		if (token == "#End") break;

		if (token == "{")
		{
			Material temp;
			while (token != "}")
			{
				getline(ss, token, ' ');

				if (token == "name") temp.name = GetPath(ss);
				else if (token == "d") temp.diffuse_map_name = GetPath(ss);
				else if (token == "n") temp.normal_map_name = GetPath(ss);
				else if (token == "s") temp.specular_map_name = GetPath(ss);
				else if (token == "f") temp.fresnel_factor = GetFloat3(ss);
			}
			g_MaterialMng.AddMaterial(temp);
		}
	}
}

void AssetListDataImporter::Load(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
                                 D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
                                 D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	ifstream in("Resources/AssetsData.txt");
	string s;

	if (!in.is_open())
	{
		assert(!"파일을 여는데 실패했습니다.");
		return;
	}

	while (in)
	{
		string name;
		string type;
		getline(in, s);
		stringstream ss(s);

		getline(ss, name, '.');
		getline(ss, type, '\n');
		if (type == "dds")
		{
			if (g_texture_manager.IsExist(name.c_str())) continue;
			g_texture_manager.LoadFromFile(name.c_str(), device, command_list, srv_cpu_handle, srv_gpu_handle);
		}
		if (type == "obj")
		{
			if (g_ModelMng.IsAleadyExist(name.c_str())) continue;
			g_ModelMng.AddModel(name.c_str(), device, command_list);
		}
		if (type == "mm")
		{
			if (g_ModelMng.IsAleadyExist(name.c_str())) continue;
			g_ModelMng.AddFBXModel(name.c_str(), device, command_list);
		}
		if (type == "mac")
		{
			if (g_AnimMng.IsExist(name.c_str())) continue;
			g_AnimMng.AddAnimationClip(name.c_str());
		}
	}
}

struct TransformForImport
{
	float rotation_translation[8];
};

AnimationClip AnimClipDataImporter::Load(const char* asset_name)
{
	AnimationClip anim_clip;

	anim_clip.name = asset_name;

	string path;

	string directory = "Resources/Animations/";
	string extension = ".mac"; // my animation clip

	path = directory + asset_name;
	path += extension;

	ifstream in;
	in.open(path, ios::in | ios::binary);

	int n_bone, n_keys;
	double* key_times;
	in.read(reinterpret_cast<char*>(&n_bone), sizeof(int));
	in.read(reinterpret_cast<char*>(&n_keys), sizeof(int));

	key_times = new double[n_keys];
	in.read(reinterpret_cast<char*>(key_times), sizeof(double) * n_keys);

	for (int i = 0; i < n_keys; i++) anim_clip.times.push_back(key_times[i]);
	anim_clip.length = anim_clip.times.back();

	anim_clip.bones.resize(n_bone);

	for (int i_bone = 0; i_bone < n_bone; i_bone++)
	{
		// toDressedPose + toParent + local * nKey
		in.read(reinterpret_cast<char*>(&anim_clip.bones[i_bone].parent_idx), sizeof(int));

		int n_float = 7 * (n_keys + 2);
		auto f_in = new float[n_float];
		in.read(reinterpret_cast<char*>(f_in), sizeof(float) * n_float);
		int offset = 0;
		anim_clip.bones[i_bone].to_dressed_pose_inv = GetMatrix(f_in, offset);
		anim_clip.bones[i_bone].to_parent = GetMatrix(f_in, offset);
		for (int i_keys = 0; i_keys < n_keys; i_keys++) anim_clip.bones[i_bone].keys.push_back(
			GetKeyframe(f_in, offset));
	}

	in.close();

	return anim_clip;
}
