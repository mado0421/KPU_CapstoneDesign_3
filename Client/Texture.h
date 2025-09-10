#pragma once

#define MAXNUMSHADOW 64
#define MAXNUMRENDERTARGETTEXTURE 8
#define MAXNUMPOSTPROCESSINGTEXTURE 8

using TextureType = enum TEXTURETYPE
{
	NONE = static_cast<UINT>(0),
	ONLY_SRV,
	RTV_SRV,
	DSV_SRV,
	UAV_SRV
};

/*========================================================================
* Texture
*=======================================================================*/
class TempTexture
{
public:
	~TempTexture()
	{
		if (m_pd3dTexture) m_pd3dTexture->Release();
		if (m_pd3dUploadBuffer) m_pd3dUploadBuffer->Release();
	}

	void SetByDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
	                      D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
	                      D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	                      D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void SetByDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
	                      D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
	                      D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	                      D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle,
	                      UINT nArraySize);
	void SetByCubeDepthBuffer(ID3D12Device* pd3dDevice, UINT width, UINT height,
	                          D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle,
	                          D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	                          D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void SetByRenderTarget(ID3D12Device* pd3dDevice, UINT width, UINT height,
	                       D3D12_CPU_DESCRIPTOR_HANDLE& rtvCpuHandle,
	                       D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	                       D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void SetByUnorderedAccessTexture(ID3D12Device* pd3dDevice, UINT width, UINT height,
	                                 D3D12_CPU_DESCRIPTOR_HANDLE& uavCpuHandle,
	                                 D3D12_GPU_DESCRIPTOR_HANDLE& uavGpuHandle,
	                                 D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
	                                 D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void LoadFromFile(
		const wchar_t* pszFileName,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);

	ID3D12Resource* GetTextureResource();
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetUavCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavGPUHandle();

private:
	bool IsTexTypeIsNone() { return m_TextureType == NONE; }

	TextureType m_TextureType = NONE;
	ID3D12Resource* m_pd3dTexture = nullptr;
	ID3D12Resource* m_pd3dUploadBuffer = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dUavCPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dUavGPUDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvCPUDescriptorHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dRtvCPUDescriptorHandle;
};

/*========================================================================
* Texture Manager
*=======================================================================*/
using unorderedTextureMap = unordered_map<string, TempTexture*>;

class TextureManager
{
public:
	TextureManager();
	void Initialize(ID3D12Device* pd3dDevice);

	~TextureManager()
	{
		m_uomTextures.clear();
		if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();
		if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
		if (m_pd3dUavDescriptorHeap) m_pd3dUavDescriptorHeap->Release();
	}

	void AddDepthBufferTexture(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddDepthBufferTextureArray(
		const char* name,
		UINT nArraySize,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddDepthBufferTextureCube(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddRenderTargetTexture(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void AddUnorderedAccessTexture(
		const char* name,
		ID3D12Device* pd3dDevice, UINT width, UINT height,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);
	void LoadFromFile(
		const char* name,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);

	void DeleteTexture(const char* name)
	{
		m_uomTextures.erase(name);
	}

	bool IsAleadyExist(const char* name) { return m_uomTextures.contains(name); }

	ID3D12Resource* GetTextureResource(const char* name);
	void UseForShaderResource(const char* name, ID3D12GraphicsCommandList* pd3dCommandList, UINT rootParameterIdx);
	void UseForComputeShaderResourceSRV(const char* name, ID3D12GraphicsCommandList* pd3dCommandList,
	                                    UINT rootParameterIdx);
	void UseForComputeShaderResourceUAV(const char* name, ID3D12GraphicsCommandList* pd3dCommandList,
	                                    UINT rootParameterIdx);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle(const char* name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUHandle(const char* name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetUavCPUHandle(const char* name);
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavGPUHandle(const char* name);

private:
	void CreateDsvDescriptorHeap(ID3D12Device* pd3dDevice);
	void CreateRtvDescriptorHeap(ID3D12Device* pd3dDevice);
	void CreateUavDescriptorHeap(ID3D12Device* pd3dDevice);

	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dDsvCPUDescriptorHandle;

	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dRtvCPUDescriptorHandle;

	ID3D12DescriptorHeap* m_pd3dUavDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_d3dUavCPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dUavGPUDescriptorHandle;

	unorderedTextureMap m_uomTextures;
};
