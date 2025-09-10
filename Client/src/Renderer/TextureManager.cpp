#include "pch.h"
#include "TextureManager.h"
#include "Elements/Texture.h""

TextureManager::TextureManager() = default;

TextureManager::~TextureManager()
{
	unordered_texture_map_.clear();
	if (dsv_descriptor_heap_) dsv_descriptor_heap_->Release();
	if (rtv_descriptor_heap_) rtv_descriptor_heap_->Release();
	if (uav_descriptor_heap_) uav_descriptor_heap_->Release();
}

void TextureManager::Initialize(ID3D12Device* device)
{
	unordered_texture_map_.clear();
	if (dsv_descriptor_heap_) dsv_descriptor_heap_->Release();
	if (rtv_descriptor_heap_) rtv_descriptor_heap_->Release();
	if (uav_descriptor_heap_) uav_descriptor_heap_->Release();

	CreateDSVDescriptorHeap(device);
	CreateRTVDescriptorHeap(device);
	CreateUAVDescriptorHeap(device);
	dsv_cpu_descriptor_handle_ = dsv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
	rtv_cpu_descriptor_handle_ = rtv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
	uav_cpu_descriptor_handle_ = uav_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
	uav_gpu_descriptor_handle_ = uav_descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
}

void TextureManager::AddDepthBufferTexture(const char* name, ID3D12Device* device, const UINT width, const UINT height,
										   D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
										   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();
	texture->SetByDepthBuffer(device, width, height, dsv_cpu_descriptor_handle_, srv_cpu_handle, srv_gpu_handle);

	unordered_texture_map_[name] = texture;
}

void TextureManager::AddDepthBufferTextureArray(const char* name, const UINT array_size, ID3D12Device* device,
												const UINT width, const UINT height,
												D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
												D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();
	texture->SetByDepthBuffer(device, width, height, dsv_cpu_descriptor_handle_, srv_cpu_handle, srv_gpu_handle,
							  array_size);

	unordered_texture_map_[name] = texture;
}

void TextureManager::AddDepthBufferTextureCube(const char* name, ID3D12Device* device, const UINT width,
											   const UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
											   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();
	texture->SetByCubeDepthBuffer(device, width, height, dsv_cpu_descriptor_handle_, srv_cpu_handle, srv_gpu_handle);

	unordered_texture_map_[name] = texture;
}

void TextureManager::AddRenderTargetTexture(const char* name, ID3D12Device* device, const UINT width, const UINT height,
											D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
											D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();
	texture->SetByRenderTarget(device, width, height, rtv_cpu_descriptor_handle_, srv_cpu_handle, srv_gpu_handle);

	unordered_texture_map_[name] = texture;
}

void TextureManager::AddUnorderedAccessTexture(const char* name, ID3D12Device* device, const UINT width,
											   const UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
											   D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();
	texture->SetByUnorderedAccessTexture(device, width, height, uav_cpu_descriptor_handle_, uav_gpu_descriptor_handle_,
										 srv_cpu_handle, srv_gpu_handle);

	unordered_texture_map_[name] = texture;
}

void TextureManager::LoadFromFile(const char* name, ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
								  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
								  D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	Texture* const texture = new Texture();

	string path = file_directory_ + name;
	path += file_extension_;

	texture->LoadFromFile(CharToWChar(path.c_str()), device, command_list, srv_cpu_handle, srv_gpu_handle);

	unordered_texture_map_[name] = texture;
}

void TextureManager::DeleteTexture(const char* name) { unordered_texture_map_.erase(name); }

bool TextureManager::IsExist(const char* name) const { return unordered_texture_map_.contains(name); }

ID3D12Resource* TextureManager::GetTextureResource(const char* name)
{
	return unordered_texture_map_[name]->GetTextureResource();
}

void TextureManager::UseForShaderResource(const char* name, ID3D12GraphicsCommandList* command_list,
										  const UINT  root_parameter_idx)
{
	command_list->SetGraphicsRootDescriptorTable(root_parameter_idx, unordered_texture_map_[name]->GetSrvGPUHandle());
}

void TextureManager::UseForComputeShaderResourceSRV(const char* name, ID3D12GraphicsCommandList* command_list,
													const UINT  root_parameter_idx)
{
	command_list->SetComputeRootDescriptorTable(root_parameter_idx, unordered_texture_map_[name]->GetSrvGPUHandle());
}

void TextureManager::UseForComputeShaderResourceUAV(const char* name, ID3D12GraphicsCommandList* command_list,
													const UINT  root_parameter_idx)
{
	command_list->SetComputeRootDescriptorTable(root_parameter_idx, unordered_texture_map_[name]->GetUavGPUHandle());
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetDSVCPUHandle(const char* name)
{
	return unordered_texture_map_[name]->GetDsvCPUHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetRTVCPUHandle(const char* name)
{
	return unordered_texture_map_[name]->GetRtvCPUHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE TextureManager::GetUAVCPUHandle(const char* name)
{
	return unordered_texture_map_[name]->GetUavCPUHandle();
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetUAVGPUHandle(const char* name)
{
	return unordered_texture_map_[name]->GetUavGPUHandle();
}

void TextureManager::CreateDSVDescriptorHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	::ZeroMemory(&descriptor_heap_desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptor_heap_desc.NumDescriptors = MAXNUMSHADOW;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	HRESULT result = device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
												  reinterpret_cast<void**>(&dsv_descriptor_heap_));
}

void TextureManager::CreateRTVDescriptorHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	::ZeroMemory(&descriptor_heap_desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptor_heap_desc.NumDescriptors = MAXNUMRENDERTARGETTEXTURE;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	HRESULT result = device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
												  reinterpret_cast<void**>(&rtv_descriptor_heap_));
}

void TextureManager::CreateUAVDescriptorHeap(ID3D12Device* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc;
	::ZeroMemory(&descriptor_heap_desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptor_heap_desc.NumDescriptors = MAXNUMPOSTPROCESSINGTEXTURE;
	descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptor_heap_desc.NodeMask = 0;
	HRESULT result = device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
												  reinterpret_cast<void**>(&uav_descriptor_heap_));
}
