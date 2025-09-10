#pragma once
#include "Importer.h"

class Mesh
{
public:
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, MESH_DATA meshData);
	Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height);

	~Mesh()
	{
		if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
		if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	}

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

protected:
	ID3D12Resource* m_pd3dVertexBuffer;
	ID3D12Resource* m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

	UINT m_nStride;
	UINT m_nVertices;

	string m_strName;
	string m_strDiffMapName;
	string m_strNormMapName;
};
