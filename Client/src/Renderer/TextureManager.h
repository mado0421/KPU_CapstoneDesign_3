#pragma once
#include <d3d12.h>
#include <string>
#include <unordered_map>

class Texture;

using UnorderedTextureMap = std::unordered_map<std::string, Texture*>;

class TextureManager
{
public:
	TextureManager(const TextureManager& other) = delete;
	TextureManager(TextureManager&& other) noexcept = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
	TextureManager& operator=(TextureManager&& other) noexcept = delete;

	TextureManager();
	~TextureManager();
	
	void Initialize(ID3D12Device* device);

	void AddDepthBufferTexture(const char* name, ID3D12Device* device, UINT width, UINT height,
							   D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
							   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void AddDepthBufferTextureArray(const char* name, UINT array_size, ID3D12Device* device, UINT width, UINT height,
									D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
									D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void AddDepthBufferTextureCube(const char* name, ID3D12Device* device, UINT width, UINT height,
								   D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
								   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void AddRenderTargetTexture(const char* name, ID3D12Device* device, UINT width, UINT height,
								D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
								D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void AddUnorderedAccessTexture(const char* name, ID3D12Device* device, UINT width, UINT height,
								   D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
								   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void LoadFromFile(const char*                  name, ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
					  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);

	void DeleteTexture(const char* name);
	bool IsExist(const char* name) const;

	ID3D12Resource* GetTextureResource(const char* name);
	void UseForShaderResource(const char* name, ID3D12GraphicsCommandList* command_list, UINT root_parameter_idx);
	void UseForComputeShaderResourceSRV(const char* name, ID3D12GraphicsCommandList* command_list,
										UINT root_parameter_idx);
	void UseForComputeShaderResourceUAV(const char* name, ID3D12GraphicsCommandList* command_list,
										UINT        root_parameter_idx);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUHandle(const char* name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle(const char* name);
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle(const char* name);
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUHandle(const char* name);

private:
	void CreateDSVDescriptorHeap(ID3D12Device* device);
	void CreateRTVDescriptorHeap(ID3D12Device* device);
	void CreateUAVDescriptorHeap(ID3D12Device* device);

	const std::string file_directory_ = "Resources/Images/";
	const std::string file_extension_ = ".dds";
	
	ID3D12DescriptorHeap*       dsv_descriptor_heap_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_descriptor_handle_ = {};

	ID3D12DescriptorHeap*       rtv_descriptor_heap_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_cpu_descriptor_handle_ = {};

	ID3D12DescriptorHeap*       uav_descriptor_heap_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_descriptor_handle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_descriptor_handle_ = {};

	UnorderedTextureMap unordered_texture_map_;
};

inline wchar_t* CharToWChar(const char* str)
{
	size_t newsize = strlen(str) + 1;

	// The following creates a buffer large enough to contain
	// the exact number of characters in the original string
	// in the new format. If you want to add more characters
	// to the end of the string, increase the value of newsize
	// to increase the size of the buffer.
	auto wcstring = new wchar_t[newsize];

	// Convert char* string to a wchar_t* string.
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE);
	return wcstring;
}
