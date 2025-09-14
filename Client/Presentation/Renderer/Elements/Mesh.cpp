#include "pch.h"
#include "Mesh.h"

#include "Presentation/Renderer/DirectX/DirectXMethods.h"


Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const MeshData& mesh_data)
{
	stride_       = sizeof(Vertex);
	vertex_count_ = mesh_data.vertices.size();

	name_ = mesh_data.name;

	Vertex* const vertices = new Vertex[vertex_count_];

	for (UINT i = 0; i < vertex_count_; i++) vertices[i] = mesh_data.vertices[i];

	vertex_buffer_ = CreateBufferResource(device, command_list, vertices, stride_ * vertex_count_,
										  D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
										  &vertex_upload_buffer_);
	vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
	vertex_buffer_view_.StrideInBytes  = stride_;
	vertex_buffer_view_.SizeInBytes    = stride_ * vertex_count_;

	delete[]vertices;
}

Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const float width, const float height)
{
	stride_       = sizeof(Vertex);
	vertex_count_ = 6;

	Vertex* const vertices = new Vertex[vertex_count_];

	int      i = 0;
	XMFLOAT3 pos[4];
	pos[i++] = XMFLOAT3(-width, height, 0.0f);
	pos[i++] = XMFLOAT3(width, height, 0.0f);
	pos[i++] = XMFLOAT3(-width, -height, 0.0f);
	pos[i++] = XMFLOAT3(width, -height, 0.0f);


	i = 0;
	XMFLOAT2 uv[4];
	uv[i++] = XMFLOAT2(0, 0);
	uv[i++] = XMFLOAT2(1, 0);
	uv[i++] = XMFLOAT2(0, 1);
	uv[i++] = XMFLOAT2(1, 1);

	i              = 0;
	vertices[i++] = Vertex(pos[0], XMFLOAT3(0, 0, -1), uv[0]);
	vertices[i++] = Vertex(pos[3], XMFLOAT3(0, 0, -1), uv[3]);
	vertices[i++] = Vertex(pos[1], XMFLOAT3(0, 0, -1), uv[1]);
	vertices[i++] = Vertex(pos[0], XMFLOAT3(0, 0, -1), uv[0]);
	vertices[i++] = Vertex(pos[2], XMFLOAT3(0, 0, -1), uv[2]);
	vertices[i++] = Vertex(pos[3], XMFLOAT3(0, 0, -1), uv[3]);

	vertex_buffer_ = CreateBufferResource(device, command_list, vertices, stride_ * vertex_count_,
										  D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
										  &vertex_upload_buffer_);
	vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
	vertex_buffer_view_.StrideInBytes  = stride_;
	vertex_buffer_view_.SizeInBytes    = stride_ * vertex_count_;

	delete[]vertices;
}

Mesh::~Mesh()
{
	if (vertex_buffer_) vertex_buffer_->Release();
	if (vertex_upload_buffer_) vertex_upload_buffer_->Release();
}

void Mesh::Render(ID3D12GraphicsCommandList* command_list) const
{
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view_);
	command_list->DrawInstanced(vertex_count_, 1, 0, 0);
}
