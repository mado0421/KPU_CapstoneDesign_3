#pragma once

#define MAXNUMSHADOW 64
#define MAXNUMRENDERTARGETTEXTURE 8
#define MAXNUMPOSTPROCESSINGTEXTURE 8

#include <d3d12.h>

enum class TextureType
{
	kNone,
	kOnlySrv,
	kRtvSrv,
	kDsvSrv,
	kUavSrv,
};


class Texture
{
public:
	Texture();
	Texture(const Texture&)            = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&)                 = delete;
	Texture& operator=(Texture&&)      = delete;

	~Texture();

	void SetByDepthBuffer(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle,
						  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void SetByDepthBuffer(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle,
						  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle,
						  UINT array_size);
	void SetByCubeDepthBuffer(ID3D12Device* device, UINT width, UINT height,
							  D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
							  D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void SetByRenderTarget(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& rtv_cpu_handle,
						   D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void SetByUnorderedAccessTexture(ID3D12Device* device, UINT width, UINT height,
									 D3D12_CPU_DESCRIPTOR_HANDLE& uav_cpu_handle,
									 D3D12_GPU_DESCRIPTOR_HANDLE& uav_gpu_handle,
									 D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
									 D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);
	void LoadFromFile(const wchar_t* file_name, ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
					  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle);

	ID3D12Resource*             GetTextureResource() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGPUHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCPUHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCPUHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetUavCPUHandle() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavGPUHandle() const;

private:
	bool IsInitialized() const;

	TextureType                 texture_type_              = TextureType::kNone;
	ID3D12Resource*             texture_                   = nullptr;
	ID3D12Resource*             upload_buffer_             = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_descriptor_handle_ = {};
	D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_descriptor_handle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_descriptor_handle_ = {};
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_descriptor_handle_ = {};
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_cpu_descriptor_handle_ = {};
};
