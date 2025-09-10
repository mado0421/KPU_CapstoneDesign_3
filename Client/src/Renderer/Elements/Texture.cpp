#include "pch.h"
#include "Texture.h"

Texture::Texture() = default;

Texture::~Texture() {
	if (texture_) texture_->Release();
	if (upload_buffer_) upload_buffer_->Release();
}

void Texture::SetByDepthBuffer(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ���̹��� �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kDsvSrv;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize   = 1;
	d3dResourceDesc.Alignment          = 0;
	d3dResourceDesc.Width              = width;
	d3dResourceDesc.Height             = height;
	d3dResourceDesc.MipLevels          = 1;
	d3dResourceDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count   = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask     = 1;
	d3dHeapProperties.VisibleNodeMask      = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth   = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	device->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&texture_);

	D3D12_TEX2D_DSV tex;
	tex.MipSlice = 0;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D     = tex;
	dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(texture_, &dsvDesc, dsv_cpu_handle);
	dsv_cpu_descriptor_handle_ = dsv_cpu_handle;
	dsv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format                          = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip       = 0;
	srvDesc.Texture2D.MipLevels             = 1;
	srvDesc.Texture2D.ResourceMinLODClamp   = 0.0f;
	srvDesc.Texture2D.PlaneSlice            = 0;

	device->CreateShaderResourceView(texture_, &srvDesc, srv_cpu_handle);
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::SetByDepthBuffer(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle, UINT array_size)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ���̹��� �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kDsvSrv;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize   = array_size;
	d3dResourceDesc.Alignment          = 0;
	d3dResourceDesc.Width              = width;
	d3dResourceDesc.Height             = height;
	d3dResourceDesc.MipLevels          = 1;
	d3dResourceDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count   = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask     = 1;
	d3dHeapProperties.VisibleNodeMask      = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth   = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	device->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&texture_);

	D3D12_TEX2D_ARRAY_DSV tex;
	tex.MipSlice        = 0;
	tex.ArraySize       = array_size;
	tex.FirstArraySlice = 0;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format         = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension  = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray = tex;
	dsvDesc.Flags          = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(texture_, &dsvDesc, dsv_cpu_handle);
	dsv_cpu_descriptor_handle_ = dsv_cpu_handle;
	dsv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc    = {};
	srvDesc.Shader4ComponentMapping            = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format                             = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize           = array_size;
	srvDesc.Texture2DArray.FirstArraySlice     = 0;
	srvDesc.Texture2DArray.MipLevels           = 1;
	srvDesc.Texture2DArray.MostDetailedMip     = 0;
	srvDesc.Texture2DArray.PlaneSlice          = 0;
	srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

	device->CreateShaderResourceView(texture_, &srvDesc, srv_cpu_handle);
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::SetByCubeDepthBuffer(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& dsv_cpu_handle, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ���̹��� �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kDsvSrv;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.DepthOrArraySize   = 6;
	d3dResourceDesc.Alignment          = 0;
	d3dResourceDesc.Width              = width;
	d3dResourceDesc.Height             = height;
	d3dResourceDesc.MipLevels          = 1;
	d3dResourceDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count   = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask     = 1;
	d3dHeapProperties.VisibleNodeMask      = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth   = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	device->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&texture_);

	D3D12_TEX2D_ARRAY_DSV tex;
	tex.MipSlice        = 0;
	tex.ArraySize       = 6;
	tex.FirstArraySlice = 0;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format         = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension  = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray = tex;
	dsvDesc.Flags          = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(texture_, &dsvDesc, dsv_cpu_handle);
	dsv_cpu_descriptor_handle_ = dsv_cpu_handle;
	dsv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc    = {};
	srvDesc.Shader4ComponentMapping            = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format                             = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2DArray.ArraySize           = 6;
	srvDesc.Texture2DArray.FirstArraySlice     = 0;
	srvDesc.Texture2DArray.MipLevels           = 1;
	srvDesc.Texture2DArray.MostDetailedMip     = 0;
	srvDesc.Texture2DArray.PlaneSlice          = 0;
	srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

	device->CreateShaderResourceView(texture_, &srvDesc, srv_cpu_handle);
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::SetByRenderTarget(ID3D12Device* device, UINT width, UINT height, D3D12_CPU_DESCRIPTOR_HANDLE& rtv_cpu_handle, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ����Ÿ�� �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kRtvSrv;

	HRESULT hr = E_FAIL;

	D3D12_RESOURCE_DESC desc = {};
	desc.Width               = width;
	desc.Height              = height;
	desc.MipLevels           = 1;
	desc.DepthOrArraySize    = 1;
	desc.Format              = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	desc.SampleDesc.Count    = 1;
	desc.SampleDesc.Quality  = 0;
	desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	D3D12_HEAP_PROPERTIES hp;
	hp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask     = 1;
	hp.VisibleNodeMask      = 1;
	hp.Type                 = D3D12_HEAP_TYPE_DEFAULT;

	hr = device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&texture_));

	device->CreateRenderTargetView(texture_, nullptr, rtv_cpu_handle);
	rtv_cpu_descriptor_handle_ = rtv_cpu_handle;
	rtv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_RESOURCE_DESC d3dResourceDesc = texture_->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	device->CreateShaderResourceView(texture_, &d3dShaderResourceViewDesc, srv_cpu_handle);
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::SetByUnorderedAccessTexture(ID3D12Device*                device,
										  UINT                         width,
										  UINT                         height,
										  D3D12_CPU_DESCRIPTOR_HANDLE& uav_cpu_handle,
										  D3D12_GPU_DESCRIPTOR_HANDLE& uav_gpu_handle,
										  D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle,
										  D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ��ó�� �ؽ�ó �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kUavSrv;

	HRESULT hr = E_FAIL;


	//	��ó�� �ؽ�ó�� SRV�� UAV �� �� �������� ��.
	D3D12_RESOURCE_DESC desc = {};
	desc.Width               = width;
	desc.Height              = height;
	desc.MipLevels           = 1;
	desc.DepthOrArraySize    = 1;
	desc.Format              = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.SampleDesc.Count    = 1;
	desc.SampleDesc.Quality  = 0;
	desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Alignment           = 0;

	D3D12_HEAP_PROPERTIES hp;
	hp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask     = 1;
	hp.VisibleNodeMask      = 1;
	hp.Type                 = D3D12_HEAP_TYPE_DEFAULT;

	hr = device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture_));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {}; // = {}; �� �޾��ִϱ� ���ڱ� ��
	uavDesc.Format                           = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice               = 0;

	device->CreateUnorderedAccessView(texture_, nullptr, &uavDesc, srv_cpu_handle);
	uav_cpu_descriptor_handle_ = srv_cpu_handle;
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	uav_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	D3D12_RESOURCE_DESC d3dResourceDesc = texture_->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);

	device->CreateShaderResourceView(texture_, &d3dShaderResourceViewDesc, srv_cpu_handle);
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Texture::LoadFromFile(const wchar_t* file_name, ID3D12Device* device, ID3D12GraphicsCommandList* command_list, D3D12_CPU_DESCRIPTOR_HANDLE& srv_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& srv_gpu_handle)
{
	if (IsInitialized())
	{
		assert(!"�̹� �ٸ� �뵵�� ���� �ؽ�ó�� ���̴� ���ҽ� ���� �뵵�� �缳�� �Ϸ� �Ͽ����ϴ�.\n");
		return;
	}
	texture_type_ = TextureType::kOnlySrv;

	texture_ = CreateTextureResourceFromFile(device, command_list, file_name, &upload_buffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	D3D12_RESOURCE_DESC d3dResourceDesc = texture_->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, RESOURCE_TEXTURE2D);
	device->CreateShaderResourceView(texture_, &d3dShaderResourceViewDesc, srv_cpu_handle);

	srv_gpu_descriptor_handle_ = srv_gpu_handle;
	srv_cpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
	srv_gpu_handle.ptr += gnCbvSrvDescriptorIncrementSize;
}

ID3D12Resource* Texture::GetTextureResource() const
{
	if (IsInitialized()) return texture_;
	throw std::logic_error("This resource has not been initialized.");
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetSrvGPUHandle() const
{
	if (IsInitialized()) return srv_gpu_descriptor_handle_;
	throw std::logic_error("This resource has not been initialized.");
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDsvCPUHandle() const
{
	if (texture_type_ == TextureType::kDsvSrv) return dsv_cpu_descriptor_handle_;
	throw std::logic_error("This resource has not been initialized for DSV, SRV use.");
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRtvCPUHandle() const
{
	if (texture_type_ == TextureType::kRtvSrv) return rtv_cpu_descriptor_handle_;
	throw std::logic_error("This resource has not been initialized for RTV, SRV use.");
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUavCPUHandle() const
{
	if (texture_type_ == TextureType::kUavSrv) return uav_cpu_descriptor_handle_;
	throw std::logic_error("This resource has not been initialized for UAV, SRV use.");
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetUavGPUHandle() const
{
	if (texture_type_ == TextureType::kUavSrv) return uav_gpu_descriptor_handle_;
	throw std::logic_error("This resource has not been initialized for UAV, SRV use.");
}

bool Texture::IsInitialized() const { return texture_type_ != TextureType::kNone; }
