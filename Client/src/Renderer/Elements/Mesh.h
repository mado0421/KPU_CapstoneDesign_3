#pragma once
#include "Vertex.h"

class MeshData
{
public:
	string name;
	vector<Vertex>  vertices;
};

class Mesh final
{
public:
	Mesh() = delete;
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) noexcept = delete;
	
	/// <summary>
	///		It initializes with mesh data.
	/// </summary>
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const MeshData& mesh_data);
	/// <summary>
	///		It initializes to a rectangular mesh.
	/// </summary>
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, float width, float height);

    ~Mesh();

	void Render(ID3D12GraphicsCommandList* command_list) const;

protected:
    ID3D12Resource*          vertex_buffer_;
    ID3D12Resource*          vertex_upload_buffer_;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

	/// <summary>
	///		Stride is the distance (in bytes) between the start of one vertex and the start of the next vertex when vertex data is listed in memory.
	/// </summary>
	UINT stride_;
    UINT vertex_count_;
    string name_;
};
