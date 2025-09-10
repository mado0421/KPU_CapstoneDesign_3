#include "stdafx.h"
#include "Mesh.h"
#include "Vertex.h"

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nStride = sizeof(Vertex);
	m_nVertices = 36;
	m_strName = "";

	auto pVertices = new Vertex[m_nVertices];

	int i = 0;
	XMFLOAT3 pos[8];
	pos[i++] = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	pos[i++] = XMFLOAT3(0.5f, 0.5f, 0.5f);
	pos[i++] = XMFLOAT3(0.5f, 0.5f, -0.5f);
	pos[i++] = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	pos[i++] = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	pos[i++] = XMFLOAT3(0.5f, -0.5f, 0.5f);
	pos[i++] = XMFLOAT3(0.5f, -0.5f, -0.5f);
	pos[i++] = XMFLOAT3(-0.5f, -0.5f, -0.5f);

	i = 0;
	XMFLOAT3 normal[6];
	normal[i++] = XMFLOAT3(0.0f, 1.0f, 0.0f);
	normal[i++] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	normal[i++] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	normal[i++] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	normal[i++] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	normal[i++] = XMFLOAT3(0.0f, -1.0f, 0.0f);

	i = 0;
	XMFLOAT3 tangent[6];
	tangent[i++] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	tangent[i++] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	tangent[i++] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	tangent[i++] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	tangent[i++] = XMFLOAT3(0.0f, 0.0f, -1.0f);
	tangent[i++] = XMFLOAT3(1.0f, 0.0f, 0.0f);

	i = 0;
	XMFLOAT2 uv[13];
	uv[i++] = XMFLOAT2(0.0f, 0.0f);
	uv[i++] = XMFLOAT2(0.3f, 0.0f);
	uv[i++] = XMFLOAT2(0.6f, 0.0f);
	uv[i++] = XMFLOAT2(0.9f, 0.0f);
	uv[i++] = XMFLOAT2(0.0f, 0.3f);
	uv[i++] = XMFLOAT2(0.3f, 0.3f);
	uv[i++] = XMFLOAT2(0.6f, 0.3f);
	uv[i++] = XMFLOAT2(0.9f, 0.3f);
	uv[i++] = XMFLOAT2(0.0f, 0.6f);
	uv[i++] = XMFLOAT2(0.3f, 0.6f);
	uv[i++] = XMFLOAT2(0.6f, 0.6f);
	uv[i++] = XMFLOAT2(0.0f, 0.9f);
	uv[i++] = XMFLOAT2(0.3f, 0.9f);


	i = 0;
	pVertices[i++] = Vertex(pos[0], normal[0], tangent[0], uv[0]);
	pVertices[i++] = Vertex(pos[2], normal[0], tangent[0], uv[5]);
	pVertices[i++] = Vertex(pos[1], normal[0], tangent[0], uv[1]);
	pVertices[i++] = Vertex(pos[0], normal[0], tangent[0], uv[0]);
	pVertices[i++] = Vertex(pos[3], normal[0], tangent[0], uv[4]);
	pVertices[i++] = Vertex(pos[2], normal[0], tangent[0], uv[5]);
	pVertices[i++] = Vertex(pos[1], normal[1], tangent[1], uv[1]);
	pVertices[i++] = Vertex(pos[4], normal[1], tangent[1], uv[6]);
	pVertices[i++] = Vertex(pos[0], normal[1], tangent[1], uv[2]);
	pVertices[i++] = Vertex(pos[1], normal[1], tangent[1], uv[1]);
	pVertices[i++] = Vertex(pos[5], normal[1], tangent[1], uv[5]);
	pVertices[i++] = Vertex(pos[4], normal[1], tangent[1], uv[6]);
	pVertices[i++] = Vertex(pos[2], normal[2], tangent[2], uv[2]);
	pVertices[i++] = Vertex(pos[5], normal[2], tangent[2], uv[7]);
	pVertices[i++] = Vertex(pos[1], normal[2], tangent[2], uv[3]);
	pVertices[i++] = Vertex(pos[2], normal[2], tangent[2], uv[2]);
	pVertices[i++] = Vertex(pos[6], normal[2], tangent[2], uv[6]);
	pVertices[i++] = Vertex(pos[5], normal[2], tangent[2], uv[7]);
	pVertices[i++] = Vertex(pos[3], normal[3], tangent[3], uv[4]);
	pVertices[i++] = Vertex(pos[6], normal[3], tangent[3], uv[9]);
	pVertices[i++] = Vertex(pos[2], normal[3], tangent[3], uv[5]);
	pVertices[i++] = Vertex(pos[3], normal[3], tangent[3], uv[4]);
	pVertices[i++] = Vertex(pos[7], normal[3], tangent[3], uv[8]);
	pVertices[i++] = Vertex(pos[6], normal[3], tangent[3], uv[9]);
	pVertices[i++] = Vertex(pos[0], normal[4], tangent[4], uv[5]);
	pVertices[i++] = Vertex(pos[7], normal[4], tangent[4], uv[10]);
	pVertices[i++] = Vertex(pos[3], normal[4], tangent[4], uv[6]);
	pVertices[i++] = Vertex(pos[0], normal[4], tangent[4], uv[5]);
	pVertices[i++] = Vertex(pos[4], normal[4], tangent[4], uv[9]);
	pVertices[i++] = Vertex(pos[7], normal[4], tangent[4], uv[10]);
	pVertices[i++] = Vertex(pos[7], normal[5], tangent[5], uv[8]);
	pVertices[i++] = Vertex(pos[5], normal[5], tangent[5], uv[12]);
	pVertices[i++] = Vertex(pos[6], normal[5], tangent[5], uv[9]);
	pVertices[i++] = Vertex(pos[7], normal[5], tangent[5], uv[8]);
	pVertices[i++] = Vertex(pos[4], normal[5], tangent[5], uv[11]);
	pVertices[i++] = Vertex(pos[5], normal[5], tangent[5], uv[12]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
	                                          D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	                                          &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[]pVertices;
}

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, MESH_DATA meshData)
{
	m_nStride = sizeof(Vertex);
	m_nVertices = meshData.shape.size();

	m_strName = meshData.name;

	auto pVertices = new Vertex[m_nVertices];

	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = meshData.shape[i];

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
	                                          D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	                                          &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[]pVertices;
}

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height)
{
	m_nStride = sizeof(Vertex);
	m_nVertices = 6;

	auto pVertices = new Vertex[m_nVertices];

	int i = 0;
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

	i = 0;
	pVertices[i++] = Vertex(pos[0], XMFLOAT3(0, 0, -1), uv[0]);
	pVertices[i++] = Vertex(pos[3], XMFLOAT3(0, 0, -1), uv[3]);
	pVertices[i++] = Vertex(pos[1], XMFLOAT3(0, 0, -1), uv[1]);
	pVertices[i++] = Vertex(pos[0], XMFLOAT3(0, 0, -1), uv[0]);
	pVertices[i++] = Vertex(pos[2], XMFLOAT3(0, 0, -1), uv[2]);
	pVertices[i++] = Vertex(pos[3], XMFLOAT3(0, 0, -1), uv[3]);

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
	                                          D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	                                          &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[]pVertices;
}

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	/*========================================================================
	* 이제 메쉬마다 구분해서 토폴로지 변경함
	* 근데 맘에 안 드니까
	* 빨리 Fbx 모델만 쓰든지 하자
	* 아님 OBJ도 저걸로 뽑든지 허 참내
	*=======================================================================*/
	//if (isFbx)pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//else pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}
