#include "pch.h"
#include "Scene.h"

#include "Presentation/Renderer/Elements/Light.h"
#include "Presentation/ResourceLoader/Importer.h"
#include "Presentation/Renderer/Elements/Model.h"

#include "Presentation/Game/Component/Components.h"
#include "Presentation/Renderer/PipelineStateObject.h"
#include "Presentation/Renderer/Screen.h"

#include "Presentation/Renderer/DirectX/DirectXMethods.h"
#include "Presentation/Renderer/DirectX/d3dx12.h"


void Scene::Init(Framework* framework, ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	device_         = device;
	command_list_   = command_list;
	root_signature_ = CreateRootSignature();

	if (test_mouse_usable_) SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
	ShowCursor(false);
	CreateDescriptorHeap();

	CreatePassInfoShaderResource();
	{
		int back_buffer_pixel_count = FRAME_BUFFER_WIDTH * 2;

		D3D12_HEAP_PROPERTIES hp   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Buffer(back_buffer_pixel_count * sizeof(float),
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		HRESULT hr = device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&uab_hdr_avg_lum_));

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		uav_desc.Format                           = DXGI_FORMAT_UNKNOWN;
		uav_desc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
		uav_desc.Buffer.NumElements               = back_buffer_pixel_count * sizeof(float);
		uav_desc.Buffer.StructureByteStride       = 1;
		uav_desc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_NONE;

		device_->CreateUnorderedAccessView(uab_hdr_avg_lum_, nullptr, &uav_desc, srv_cpu_descriptor_start_handle_);
		srv_cpu_descriptor_start_handle_.ptr += gnCbvSrvDescriptorIncrementSize;
		cbv_gpu_uab_hdr_avg_lum_handle_ = srv_gpu_descriptor_start_handle_;
		srv_gpu_descriptor_start_handle_.ptr += gnCbvSrvDescriptorIncrementSize;
	}

	g_texture_manager.Initialize(device_);
	g_texture_manager.AddUnorderedAccessTexture("DownScaled", device_, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4,
		srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddUnorderedAccessTexture("Blur_Vertical", device_, FRAME_BUFFER_WIDTH / 4,
		FRAME_BUFFER_HEIGHT / 4, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddUnorderedAccessTexture("Blur_Horizontal", device_, FRAME_BUFFER_WIDTH / 4,
		FRAME_BUFFER_HEIGHT / 4, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddDepthBufferTexture("GBuffer_Depth", device_, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddRenderTargetTexture("GBuffer_Color", device_, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddRenderTargetTexture("GBuffer_Normal", device_, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);
	g_texture_manager.AddRenderTargetTexture("Screen", device_, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);

	g_texture_manager.LoadFromFile("defaultDiffuseMap", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.LoadFromFile("defaultNormalMap", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.LoadFromFile("defaultSpecularMap", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.LoadFromFile("titleImage_rescale", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.LoadFromFile("endImage_rescale", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);
	g_texture_manager.LoadFromFile("victory", device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);

	MaterialDataImporter::Load("MaterialData");


	g_ModelMng.Initialize();


	AssetListDataImporter::Load(device_, command_list_, srv_cpu_descriptor_start_handle_,
		srv_gpu_descriptor_start_handle_);


	BuildObject();

	Screen* screen = new Screen(device_, command_list_, cbv_cpu_descriptor_start_handle_,
		cbv_gpu_descriptor_start_handle_, 1.0f, 1.0f);
	screens_.push_back(screen);

	LightDataImporter        light_data_importer;
	vector<LightDescription> light_descriptions = light_data_importer.Load("Resources/LightData.txt");
	string                   shadow("ShadowMap_");
	light_manager_ = new LightManager();
	for (int i = 0; i < light_descriptions.size(); i++)
	{
		switch (light_descriptions[i].lightType)
		{
		case LIGHT_POINT: light_manager_->AddPointLight(light_descriptions[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_, cbv_gpu_descriptor_start_handle_);

			break;
		case LIGHT_DIRECTIONAL: light_manager_->AddDirectionalLight(light_descriptions[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_,
				cbv_gpu_descriptor_start_handle_, 3);

			break;
		case LIGHT_SPOT: light_manager_->AddSpotLight(light_descriptions[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_,
				cbv_gpu_descriptor_start_handle_);

			break;
		case LIGHT_NONE: default: break;
		}

		if (light_descriptions[i].bIsShadow)
		{
			string temp = to_string(i);
			temp        = shadow + temp;
			switch (light_descriptions[i].lightType)
			{
			case LIGHT_POINT: g_texture_manager.AddDepthBufferTextureCube(
					temp.c_str(), device_, SHADOWMAPSIZE, SHADOWMAPSIZE, srv_cpu_descriptor_start_handle_,
					srv_gpu_descriptor_start_handle_);

				break;

			case LIGHT_SPOT: g_texture_manager.AddDepthBufferTexture(temp.c_str(), device_, SHADOWMAPSIZE,
					SHADOWMAPSIZE, srv_cpu_descriptor_start_handle_,
					srv_gpu_descriptor_start_handle_);

				break;
			case LIGHT_DIRECTIONAL: g_texture_manager.AddDepthBufferTextureArray(
					temp.c_str(), 3, device_, SHADOWMAPSIZE, SHADOWMAPSIZE, srv_cpu_descriptor_start_handle_,
					srv_gpu_descriptor_start_handle_);


				break;
			case LIGHT_NONE: default: break;
			}
			light_manager_->SetShadowMapName(temp.c_str(), i);
		}
	}

	CreatePSO();
}

void Scene::CheckCollision()
{
	static bool first = true;
	if (first)
	{
		first = false;
		return;
	}
	for (int i = 0; i < objects_.size(); i++)
	{
		if (!objects_[i]->m_bEnable) continue;

		for (int j = i + 1; j < objects_.size(); j++) objects_[i]->CheckCollision(objects_[j]);
	}
}

void Scene::SolveConstraint()
{
	for (int i = 0; i < objects_.size(); i++) if (objects_[i]->m_bEnable) objects_[i]->SolveConstraint();
}

void Scene::Input(UCHAR* key_buffer)
{
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);
	XMFLOAT2 xmf2MouseMovement;
	if (test_mouse_usable_)
	{
		xmf2MouseMovement.x = static_cast<float>(ptCursorPos.x - FRAME_BUFFER_WIDTH / 2);
		xmf2MouseMovement.y = static_cast<float>(ptCursorPos.y - FRAME_BUFFER_HEIGHT / 2);
		SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
	}
	else
	{
		xmf2MouseMovement.x = 0;
		xmf2MouseMovement.y = 0;
	}

	for_each(objects_.begin(), objects_.end(), [&](Object* o) { o->Input(key_buffer, xmf2MouseMovement); });

	if (key_buffer[_M] & 0xF0)
	{
		test_mouse_usable_ = true;
		ShowCursor(false);
	}
	if (key_buffer[_N] & 0xF0)
	{
		test_mouse_usable_ = false;
		ShowCursor(true);
	}

	if (1 == start_end_state)
	{
		if (key_buffer[_Space] & 0xF0)
		{
			start_end_state    = 0;
			test_mouse_usable_ = true;
			ShowCursor(false);
		}
	}
	else if (2 == start_end_state) if (key_buffer[_Space] & 0xF0) start_end_state = 1;
}

void Scene::Update(float delta_time)
{
	current_time_ += delta_time;
	memcpy(&cb_mapped_pass_info_->m_xmfCurrentTime, &current_time_, sizeof(float));

	CheckCollision();
	SolveConstraint();

	for_each(objects_.begin(), objects_.end(), [&delta_time](Object* o) { if (o->m_bEnable) o->Update(delta_time); });
}

void Scene::Render(D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv, D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_dsv)
{
	command_list_->SetGraphicsRootSignature(root_signature_);
	command_list_->SetDescriptorHeaps(1, &cbv_srv_descriptor_heap_);
	command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CameraComponent* cam = camera_object_->FindComponent<CameraComponent>();
	cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	cam->SetViewportsAndScissorRects(command_list_);
	UpdatePassInfoAboutCamera();

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = cb_pass_info_->GetGPUVirtualAddress();
	command_list_->SetGraphicsRootConstantBufferView(ROOTSIGNATURE_PASSCONSTANTS, d3dGpuVirtualAddress);

	XMFLOAT4X4 texture = {0.5f, 0, 0, 0, 0, -0.5f, 0, 0, 0, 0, 1.0f, 0, 0.5f, 0.5f, 0, 1.0f};
	XMStoreFloat4x4(&cb_mapped_pass_info_->m_xmf4x4TextureTransform, XMMatrixTranspose(XMLoadFloat4x4(&texture)));

	D3D12_RESOURCE_BARRIER d3dResourceBarrier[3];
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER) * 3);
	d3dResourceBarrier[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[0].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[1].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[1].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	d3dResourceBarrier[2].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier[2].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier[2].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	d3dResourceBarrier[2].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	command_list_->ResourceBarrier(3, d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_texture_manager.GetDSVCPUHandle("GBuffer_Depth");
	command_list_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0,
		nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2];
	rtvHandle[0] = g_texture_manager.GetRTVCPUHandle("GBuffer_Color");
	rtvHandle[1] = g_texture_manager.GetRTVCPUHandle("GBuffer_Normal");

	float pfClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	command_list_->ClearRenderTargetView(rtvHandle[0], pfClearColor, 0, nullptr);
	command_list_->ClearRenderTargetView(rtvHandle[1], pfClearColor, 0, nullptr);

	command_list_->OMSetRenderTargets(2, rtvHandle, FALSE, &dsvHandle);

	command_list_->SetPipelineState(pipeline_states_["AnimatedObject"]);
	for (auto iter = anim_object_render_group_.begin(); iter != anim_object_render_group_.end(); ++iter)
		(*iter)->
			Render(command_list_);
	command_list_->SetPipelineState(pipeline_states_["PackGBuffer"]);
	for (auto iter = non_anim_object_render_group_.begin(); iter != non_anim_object_render_group_.end(); ++iter)
		(*iter)
			->Render(command_list_);


	d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Depth");
	d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;

	d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Color");
	d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	d3dResourceBarrier[2].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Normal");
	d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier[2].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	command_list_->ResourceBarrier(3, d3dResourceBarrier);

	cam->SetViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
	cam->SetViewportsAndScissorRects(command_list_);

	for (UINT i = 0; i < light_manager_->GetNumLight(); i++)
	{
		if (light_manager_->GetIsShadow(i))
		{
			light_manager_->SetShaderResource(command_list_, i);

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_texture_manager.GetDSVCPUHandle(
				light_manager_->GetShadowMapName(i).c_str());
			command_list_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
				0, nullptr);
			command_list_->OMSetRenderTargets(0, nullptr, TRUE, &dsvHandle);

			switch (light_manager_->GetLightType(i))
			{
			case LIGHT_SPOT: command_list_->SetPipelineState(pipeline_states_["SpotLightShadow"]);
				for (int i = 0; i < non_anim_object_render_group_.size(); i++)
					non_anim_object_render_group_[i]->Render(
						command_list_);

				command_list_->SetPipelineState(pipeline_states_["SpotLightShadowAnim"]);
				for (int i = 0; i < anim_object_render_group_.size(); i++)
					anim_object_render_group_[i]->Render(
						command_list_);
				break;

			case LIGHT_POINT: command_list_->SetPipelineState(pipeline_states_["PointLightShadow"]);
				for (int i = 0; i < non_anim_object_render_group_.size(); i++)
					non_anim_object_render_group_[i]->Render(
						command_list_);

				command_list_->SetPipelineState(pipeline_states_["PointLightShadowAnim"]);
				for (int i = 0; i < anim_object_render_group_.size(); i++)
					anim_object_render_group_[i]->Render(
						command_list_);
				break;

			case LIGHT_DIRECTIONAL: light_manager_->UpdateDirectionalLightOrthographicLH(cam->GetViewMatrix(), i);

				command_list_->SetPipelineState(pipeline_states_["DirectionalLightShadow"]);
				for (int i = 0; i < non_anim_object_render_group_.size(); i++)
					non_anim_object_render_group_[i]->Render(
						command_list_);

				command_list_->SetPipelineState(pipeline_states_["DirectionalLightShadowAnim"]);
				for (int i = 0; i < anim_object_render_group_.size(); i++)
					anim_object_render_group_[i]->Render(
						command_list_);
				break;

			case LIGHT_NONE: default: break;
			}
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE screenRtv = g_texture_manager.GetRTVCPUHandle("Screen");
	command_list_->ClearRenderTargetView(screenRtv, pfClearColor, 0, nullptr);
	command_list_->OMSetRenderTargets(1, &screenRtv, TRUE, &dsvHandle);

	cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	cam->SetViewportsAndScissorRects(command_list_);

	command_list_->SetPipelineState(pipeline_states_["ColorFromGBuffer"]);
	g_texture_manager.UseForShaderResource("GBuffer_Normal", command_list_, ROOTSIGNATURE_NORMAL_TEXTURE);
	g_texture_manager.UseForShaderResource("GBuffer_Depth", command_list_, ROOTSIGNATURE_DEPTH_TEXTURE);
	g_texture_manager.UseForShaderResource("GBuffer_Color", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
	screens_[0]->Render(command_list_);

	command_list_->SetPipelineState(pipeline_states_["Effect"]);
	for (auto iter = effect_render_group_.begin(); iter != effect_render_group_.end(); ++iter)
		(*iter)->
			Render(command_list_);
	command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	command_list_->SetPipelineState(pipeline_states_["Particle"]);
	for (auto iter = particle_emitters_.begin(); iter != particle_emitters_.end(); ++iter)
		(*iter)->Render(
			command_list_);
	command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_texture_manager.UseForShaderResource("GBuffer_Normal", command_list_, ROOTSIGNATURE_NORMAL_TEXTURE);
	g_texture_manager.UseForShaderResource("GBuffer_Depth", command_list_, ROOTSIGNATURE_DEPTH_TEXTURE);
	g_texture_manager.UseForShaderResource("GBuffer_Color", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
	command_list_->SetPipelineState(pipeline_states_["AddLight"]);

	for (UINT i = 0; i < light_manager_->GetNumLight(); i++)
	{
		light_manager_->SetShaderResource(command_list_, i);

		if (light_manager_->GetIsShadow(i))
		{
			switch (light_manager_->GetLightType(i))
			{
			case LIGHT_SPOT: g_texture_manager.UseForShaderResource(light_manager_->GetShadowMapName(i).c_str(),
					command_list_, ROOTSIGNATURE_SHADOW_TEXTURE);
				break;
			case LIGHT_POINT: g_texture_manager.UseForShaderResource(light_manager_->GetShadowMapName(i).c_str(),
					command_list_, ROOTSIGNATURE_CUBE_TEXTURE);
				break;
			case LIGHT_DIRECTIONAL: g_texture_manager.UseForShaderResource(
					light_manager_->GetShadowMapName(i).c_str(), command_list_, ROOTSIGNATURE_SHADOWARRAY_TEXTURE);
				break;
			case LIGHT_NONE: default: break;
			}
		}

		screens_[0]->Render(command_list_);
	}

	if (gTestInt == 2)
	{
		/*===========================================================================
		* Post Process List
		* - Calc AvgLum
		* - HDR Rendering to 'Screen' Texture, 
		    DownScaled Rendering to 'DownScaled' Texture.
		* - Bloom Threshold Rendering to 'PostProcess_Temp1' Texture.
		* - Vertical Blur Bloom Threshold Rendering to 'PostProcess_Temp2' Texture.
		* - Horizontal to 'PostProcess_Temp1' Texture.
		* - 'PostProcess_Temp1' Add to 'Screen'
		*==========================================================================*/
		command_list_->SetComputeRootSignature(root_signature_);


		/*===========================================================================
		* Calc AvgLum and store to 'gfAvgLum' Buffer.
		* Render to 'DownScaled' Texture.
		*==========================================================================*/
		command_list_->SetPipelineState(pipeline_states_["HDR_First"]);


		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("DownScaled");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Screen");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		command_list_->ResourceBarrier(2, d3dResourceBarrier);

		g_texture_manager.UseForComputeShaderResourceSRV("Screen", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
		// Render Result
		g_texture_manager.
			UseForComputeShaderResourceUAV("DownScaled", command_list_, ROOTSIGNATURE_POSTPROCESS_TEXTURE);
		// DownScaled
		command_list_->SetComputeRootUnorderedAccessView(
			ROOTSIGNATURE_HDRLUMBUFFER, uab_hdr_avg_lum_->GetGPUVirtualAddress()); // AvgLum Buff

		//UINT numGroups = (UINT)ceilf(FRAME_BUFFER_WIDTH / 1024.0f);
		command_list_->Dispatch(1, FRAME_BUFFER_HEIGHT / 4, 1);
		/*
		�� �κ��� �𸣴� ���¿��� ���� ���� ��ƴ�.
		���� �޸� ����д�.

		�� ������ �׷��� [numthreads(1024, 1, 1,)]�� �Ǿ��ִ�.
		CS_DownScaleFirstPass()�� x�� �������� 1024���� �����ϰ�,
		���μ��� 1/4������ ������ �Ǳ� ������ 1920 / 4, 1080 / 4 �� 480, 270
		480�� 1024 �� ���� ��.
		270�� 1024, 1, 1�̴ϱ� 270��

		�̷��� �ϸ� gfAvgLum[270]���� ������ ����.
		*/

		/*===========================================================================
		* Calc FinalAvgLum and store to 'gfAvgLum' Buffer's [0].
		*==========================================================================*/
		command_list_->SetPipelineState(pipeline_states_["HDR_Second"]);

		command_list_->Dispatch(1, 1, 1);
		/*
		270���� �� ������ �ٿ���� �ؼ� �ϳ��� ����� �װ� gfAvgLum[0]�� �־���.
		*/

		/*===========================================================================
		* Bloom Rendering to 'Blur_Horizontal' Texture.
		*==========================================================================*/
		command_list_->SetPipelineState(pipeline_states_["PP_Bloom"]);

		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("DownScaled");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		command_list_->ResourceBarrier(2, d3dResourceBarrier);

		g_texture_manager.UseForComputeShaderResourceSRV("DownScaled", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
		g_texture_manager.UseForComputeShaderResourceUAV("Blur_Horizontal", command_list_,
			ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		UINT numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_WIDTH / 4 / 1024.0f));
		command_list_->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);


		/*===========================================================================
		* Bloom Blur Vertical
		*==========================================================================*/
		command_list_->SetPipelineState(pipeline_states_["Blur_Vertical"]);

		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Vertical");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		command_list_->ResourceBarrier(2, d3dResourceBarrier);

		g_texture_manager.UseForComputeShaderResourceSRV("Blur_Horizontal", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
		g_texture_manager.UseForComputeShaderResourceUAV("Blur_Vertical", command_list_,
			ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_WIDTH / 4 / 256.0f));
		command_list_->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);

		/*===========================================================================
		* Bloom Blur Horizontal
		*==========================================================================*/
		command_list_->SetPipelineState(pipeline_states_["Blur_Horizontal"]);

		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Vertical");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		command_list_->ResourceBarrier(2, d3dResourceBarrier);

		g_texture_manager.UseForComputeShaderResourceSRV("Blur_Vertical", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
		g_texture_manager.UseForComputeShaderResourceUAV("Blur_Horizontal", command_list_,
			ROOTSIGNATURE_POSTPROCESS_TEXTURE);

		numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_HEIGHT / 4 / 256.0f));
		command_list_->Dispatch(FRAME_BUFFER_WIDTH / 4, numGroups, 1);

		/*===========================================================================
		* Screen Render
		*==========================================================================*/
		command_list_->SetGraphicsRootSignature(root_signature_);
		command_list_->SetPipelineState(pipeline_states_["HDR_Last"]);

		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		command_list_->ResourceBarrier(1, d3dResourceBarrier);

		g_texture_manager.UseForShaderResource("Screen", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
		g_texture_manager.UseForShaderResource("Blur_Horizontal", command_list_, ROOTSIGNATURE_NORMAL_TEXTURE);
		command_list_->SetGraphicsRootUnorderedAccessView(
			ROOTSIGNATURE_HDRLUMBUFFER, uab_hdr_avg_lum_->GetGPUVirtualAddress());

		command_list_->ClearDepthStencilView(back_buffer_dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
			0, 0, nullptr);
		command_list_->OMSetRenderTargets(1, &back_buffer_rtv, TRUE, &back_buffer_dsv);
		screens_[0]->Render(command_list_);

		d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Screen");
		d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		command_list_->ResourceBarrier(1, d3dResourceBarrier);

		///*========================================================================
		//* Pass 2. Text UI ����
		//*=======================================================================*/
		command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		command_list_->SetPipelineState(pipeline_states_["Text"]);
		for (auto iter = ui_render_group_.begin(); iter != ui_render_group_.end(); ++iter)
			(*iter)->Render(
				command_list_);
		command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/*========================================================================
		* Pass 2. Title Card or End Card Render
		*=======================================================================*/
		if (start_end_state)
		{
			command_list_->SetGraphicsRootSignature(root_signature_);
			command_list_->SetPipelineState(pipeline_states_["SRToRt"]);

			if (1 == start_end_state)
				g_texture_manager.UseForShaderResource(
					"titleImage_rescale", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
			else if (3 == start_end_state)
				g_texture_manager.UseForShaderResource(
					"victory", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);
			else g_texture_manager.UseForShaderResource("endImage_rescale", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);

			command_list_->ClearDepthStencilView(back_buffer_dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
				1.0f, 0, 0, nullptr);
			command_list_->OMSetRenderTargets(1, &back_buffer_rtv, TRUE, &back_buffer_dsv);
			screens_[0]->Render(command_list_);
		}
	}
	else
	{
		command_list_->SetGraphicsRootSignature(root_signature_);
		command_list_->SetPipelineState(pipeline_states_["SRToRt"]);

		g_texture_manager.UseForShaderResource("Screen", command_list_, ROOTSIGNATURE_COLOR_TEXTURE);

		command_list_->ClearDepthStencilView(back_buffer_dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
			0, 0, nullptr);
		command_list_->OMSetRenderTargets(1, &back_buffer_rtv, TRUE, &back_buffer_dsv);
		screens_[0]->Render(command_list_);
	}
}

void Scene::Release()
{
	if (cb_pass_info_)
	{
		cb_pass_info_->Unmap(0, nullptr);
		cb_pass_info_->Release();
	}
}

void Scene::Clear()
{
	event_count     = 0;
	start_end_state = 2; // End;

	objects_.clear();
	non_anim_object_render_group_.clear();
	anim_object_render_group_.clear();
	effect_render_group_.clear();
	ui_render_group_.clear();
	particle_emitters_.clear();

	BuildObject();

	test_mouse_usable_ = false;
	ShowCursor(true);
}

void Scene::Victory() { start_end_state = 3; }

void Scene::Defeat()
{
}

void Scene::AddObject(Object* object, RenderGroup render_group)
{
	objects_.push_back(object);

	switch (render_group)
	{
	case RenderGroup::OBJECT: non_anim_object_render_group_.push_back(object);
		break;
	case RenderGroup::ANIMATED: anim_object_render_group_.push_back(object);
		break;
	case RenderGroup::PARTICLE: particle_emitters_.push_back(object);
		break;
	case RenderGroup::EFFECT: effect_render_group_.push_back(object);
		break;
	}
}

void Scene::DeleteObject(Object* object)
{
}

ID3D12RootSignature* Scene::CreateRootSignature()
{
	ID3D12RootSignature*   pd3dGraphicsRootSignature = nullptr;
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[10];

	d3dDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[0].NumDescriptors                    = 1;
	d3dDescriptorRange[0].BaseShaderRegister                = ROOTSIGNATURE_OBJECTS;
	d3dDescriptorRange[0].RegisterSpace                     = 0;
	d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[1].NumDescriptors                    = 1;
	d3dDescriptorRange[1].BaseShaderRegister                = ROOTSIGNATURE_LIGHTS;
	d3dDescriptorRange[1].RegisterSpace                     = 0;
	d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[2].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[2].NumDescriptors                    = 1;
	d3dDescriptorRange[2].BaseShaderRegister                = ROOTSIGNATURE_COLOR_TEXTURE;
	d3dDescriptorRange[2].RegisterSpace                     = 0;
	d3dDescriptorRange[2].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[3].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[3].NumDescriptors                    = 1;
	d3dDescriptorRange[3].BaseShaderRegister                = ROOTSIGNATURE_NORMAL_TEXTURE;
	d3dDescriptorRange[3].RegisterSpace                     = 0;
	d3dDescriptorRange[3].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[4].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[4].NumDescriptors                    = 1;
	d3dDescriptorRange[4].BaseShaderRegister                = ROOTSIGNATURE_DEPTH_TEXTURE;
	d3dDescriptorRange[4].RegisterSpace                     = 0;
	d3dDescriptorRange[4].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[5].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[5].NumDescriptors                    = 1;
	d3dDescriptorRange[5].BaseShaderRegister                = ROOTSIGNATURE_SHADOW_TEXTURE;
	d3dDescriptorRange[5].RegisterSpace                     = 0;
	d3dDescriptorRange[5].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[6].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[6].NumDescriptors                    = 1;
	d3dDescriptorRange[6].BaseShaderRegister                = ROOTSIGNATURE_CUBE_TEXTURE;
	d3dDescriptorRange[6].RegisterSpace                     = 0;
	d3dDescriptorRange[6].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[7].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	d3dDescriptorRange[7].NumDescriptors                    = 1;
	d3dDescriptorRange[7].BaseShaderRegister                = ROOTSIGNATURE_SHADOWARRAY_TEXTURE;
	d3dDescriptorRange[7].RegisterSpace                     = 0;
	d3dDescriptorRange[7].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[8].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRange[8].NumDescriptors                    = 1;
	d3dDescriptorRange[8].BaseShaderRegister                = ROOTSIGNATURE_ANIMTRANSFORM;
	d3dDescriptorRange[8].RegisterSpace                     = 0;
	d3dDescriptorRange[8].OffsetInDescriptorsFromTableStart = 0;

	d3dDescriptorRange[9].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	d3dDescriptorRange[9].NumDescriptors                    = 1;
	d3dDescriptorRange[9].BaseShaderRegister                = ROOTSIGNATURE_POSTPROCESS_TEXTURE;
	d3dDescriptorRange[9].RegisterSpace                     = 0;
	d3dDescriptorRange[9].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

	pd3dRootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = ROOTSIGNATURE_PASSCONSTANTS;
	pd3dRootParameters[0].Descriptor.RegisterSpace  = 0;
	pd3dRootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[0];
	pd3dRootParameters[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[1];
	pd3dRootParameters[2].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[2];
	pd3dRootParameters[3].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[3];
	pd3dRootParameters[4].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[4];
	pd3dRootParameters[5].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[5];
	pd3dRootParameters[6].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[7].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[6];
	pd3dRootParameters[7].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[8].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[7];
	pd3dRootParameters[8].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[9].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[8];
	pd3dRootParameters[9].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[10].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[9];
	pd3dRootParameters[10].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[11].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = ROOTSIGNATURE_HDRLUMBUFFER;
	pd3dRootParameters[11].Descriptor.RegisterSpace  = 0;
	pd3dRootParameters[11].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[2];
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC) * 2);
	d3dSamplerDesc[0].Filter           = D3D12_FILTER_ANISOTROPIC;
	d3dSamplerDesc[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc[0].MipLODBias       = 0;
	d3dSamplerDesc[0].MaxAnisotropy    = 1;
	d3dSamplerDesc[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc[0].MinLOD           = 0;
	d3dSamplerDesc[0].MaxLOD           = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[0].ShaderRegister   = 0;
	d3dSamplerDesc[0].RegisterSpace    = 0;
	d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	d3dSamplerDesc[1].Filter           = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc[1].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	d3dSamplerDesc[1].BorderColor      = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	d3dSamplerDesc[1].MipLODBias       = 0;
	d3dSamplerDesc[1].MaxAnisotropy    = 1;
	d3dSamplerDesc[1].ComparisonFunc   = D3D12_COMPARISON_FUNC_LESS;
	d3dSamplerDesc[1].MinLOD           = 0;
	d3dSamplerDesc[1].MaxLOD           = D3D12_FLOAT32_MAX;
	d3dSamplerDesc[1].ShaderRegister   = 1;
	d3dSamplerDesc[1].RegisterSpace    = 0;
	d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters     = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters       = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 2;
	d3dRootSignatureDesc.pStaticSamplers   = &d3dSamplerDesc[0];
	d3dRootSignatureDesc.Flags             = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob     = nullptr;
	HRESULT   isSuccess         = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = device_->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return pd3dGraphicsRootSignature;
}

void Scene::CreateDescriptorHeap()
{
	// heap ������ �Ϸ��� �̸� �����ڸ� �� ���� ������ �� �˾ƾ� �ϱ� ������
	// obj ������ �߿������� srv�� �� texture ������ �߿���..
	// �̸� �� �� ������ �� �� ������ �װ� ���صΰ� �� ������ �ϰڴµ�
	// ���� �������� ���ҽ� �ε�&��ε带 �Ѵٸ�?
	// �׷� ��쿡�� ���� �̸� �ѵ��� �ɾ�ΰ� ����� �͵� ����� �� ����
	// (���� ���, obj�� �� ��, ����ü�� �� ��, �ؽ��Ĵ� �� �� ������ �дٴ���)
	// ����, ������ ����. ������Ʈ�� ����ü, ���� ���� �� ���ļ� 1,024�� ������ �� ��.
	// �ؽ��ĵ� 128�� ������ ����� ��.
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = MAXNUMCBV + MAXNUMSRV;
	d3dDescriptorHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask       = 0;

	HRESULT result = device_->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap),
		(void**)&cbv_srv_descriptor_heap_);
	HRESULT reason = device_->GetDeviceRemovedReason();

	cbv_cpu_descriptor_start_handle_     = cbv_srv_descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
	cbv_gpu_descriptor_start_handle_     = cbv_srv_descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
	srv_cpu_descriptor_start_handle_.ptr = cbv_cpu_descriptor_start_handle_.ptr + gnCbvSrvDescriptorIncrementSize *
		MAXNUMCBV;
	srv_gpu_descriptor_start_handle_.ptr = cbv_gpu_descriptor_start_handle_.ptr + gnCbvSrvDescriptorIncrementSize *
		MAXNUMCBV;
}

void Scene::CreatePSO()
{
	auto PackGBufferPso             = PackGBufferPSO(device_, root_signature_);
	pipeline_states_["PackGBuffer"] = PackGBufferPso.GetPipelineState();

	auto AnimatedObjectPso             = AnimatedObjectPSO(device_, root_signature_);
	pipeline_states_["AnimatedObject"] = AnimatedObjectPso.GetPipelineState();

	auto RenderShadowPso                        = RenderShadowPSO(device_, root_signature_);
	pipeline_states_["SpotLightShadow"]         = RenderShadowPso.GetPipelineState();
	auto RenderSpotLightShadowAnimatedObjectPso = RenderSpotLightShadowAnimatedObjectPSO(device_, root_signature_);
	pipeline_states_["SpotLightShadowAnim"]     = RenderSpotLightShadowAnimatedObjectPso.GetPipelineState();

	auto RenderPointLightShadowPso               = RenderPointLightShadowPSO(device_, root_signature_);
	pipeline_states_["PointLightShadow"]         = RenderPointLightShadowPso.GetPipelineState();
	auto RenderPointLightShadowAnimatedObjectPso = RenderPointLightShadowAnimatedObjectPSO(device_, root_signature_);
	pipeline_states_["PointLightShadowAnim"]     = RenderPointLightShadowAnimatedObjectPso.GetPipelineState();

	auto RenderDirectionalShadowPso                = RenderDirectionalShadowPSO(device_, root_signature_);
	pipeline_states_["DirectionalLightShadow"]     = RenderDirectionalShadowPso.GetPipelineState();
	auto RenderDirectionalShadowAnimatedObjectPso  = RenderDirectionalShadowAnimatedObjectPSO(device_, root_signature_);
	pipeline_states_["DirectionalLightShadowAnim"] = RenderDirectionalShadowAnimatedObjectPso.GetPipelineState();

	auto ColorFromGBufferPso             = ColorFromGBufferPSO(device_, root_signature_);
	pipeline_states_["ColorFromGBuffer"] = ColorFromGBufferPso.GetPipelineState();

	auto AddLightPso             = AddLightPSO(device_, root_signature_);
	pipeline_states_["AddLight"] = AddLightPso.GetPipelineState();

	auto EffectPso             = EffectPSO(device_, root_signature_);
	pipeline_states_["Effect"] = EffectPso.GetPipelineState();

	auto ParticlePso             = ParticlePSO(device_, root_signature_);
	pipeline_states_["Particle"] = ParticlePso.GetPipelineState();

	//DebugColorPSO DebugColorPso = DebugColorPSO(device_, m_pd3dRootSignature);
	//m_uomPipelineStates["DebugColor"] = DebugColorPso.GetPipelineState();

	//DebugDepthPSO DebugDepthPso = DebugDepthPSO(device_, m_pd3dRootSignature);
	//m_uomPipelineStates["DebugDepth"] = DebugDepthPso.GetPipelineState();

	auto TextPso             = TextPSO(device_, root_signature_);
	pipeline_states_["Text"] = TextPso.GetPipelineState();

	/*============================================================================
	* Blur
	============================================================================*/
	auto VerticalBlurPso                = VerticalBlurCPSO(device_, root_signature_);
	pipeline_states_["Blur_Vertical"]   = VerticalBlurPso.GetPipelineState();
	auto HorizontalBlurPso              = HorizontalBlurCPSO(device_, root_signature_);
	pipeline_states_["Blur_Horizontal"] = HorizontalBlurPso.GetPipelineState();
	auto SRToRtPso                      = SRToRtPSO(device_, root_signature_);
	pipeline_states_["SRToRt"]          = SRToRtPso.GetPipelineState();


	/*============================================================================
	* HDR
	============================================================================*/
	auto HDRFstPassCpso            = HDRFstPassCPSO(device_, root_signature_);
	pipeline_states_["HDR_First"]  = HDRFstPassCpso.GetPipelineState();
	auto HDRScdPassCpso            = HDRScdPassCPSO(device_, root_signature_);
	pipeline_states_["HDR_Second"] = HDRScdPassCpso.GetPipelineState();
	auto HDRToneMappingPso         = HDRToneMappingPSO(device_, root_signature_);
	pipeline_states_["HDR_Last"]   = HDRToneMappingPso.GetPipelineState();

	/*============================================================================
	* Bloom
	============================================================================*/
	auto BloomCpso               = BloomCPSO(device_, root_signature_);
	pipeline_states_["PP_Bloom"] = BloomCpso.GetPipelineState();
}

void Scene::CreatePassInfoShaderResource()
{
	UINT ncbElementBytes = sizeof(ConstantBufferPassInfo) + 255 & ~255; //256�� ���
	cb_pass_info_ = CreateBufferResource(device_, command_list_, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	if (nullptr != cb_pass_info_)
	{
		cb_pass_info_->Map(0, nullptr, (void**)&cb_mapped_pass_info_);
		D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = cb_pass_info_->GetGPUVirtualAddress();
		d3dCBVDesc.SizeInBytes                         = ncbElementBytes;
		d3dCBVDesc.BufferLocation                      = d3dGpuVirtualAddress;
		device_->CreateConstantBufferView(&d3dCBVDesc, cbv_cpu_descriptor_start_handle_);

		cbv_cpu_descriptor_start_handle_.ptr += gnCbvSrvDescriptorIncrementSize;
	}

	cbv_gpu_pass_info_handle_ = cbv_gpu_descriptor_start_handle_;
	cbv_gpu_descriptor_start_handle_.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Scene::UpdatePassInfoAboutCamera()
{
	CameraComponent* cam = camera_object_->FindComponent<CameraComponent>();
	XMFLOAT4X4       xmf4x4Temp;

	//xmf4x4Temp = m_pCamera->GetViewMatrix();
	xmf4x4Temp = cam->GetViewMatrix();
	XMStoreFloat4x4(&cb_mapped_pass_info_->m_xmf4x4CameraView, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = matrix::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&cb_mapped_pass_info_->m_xmf4x4CameraViewInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//xmf4x4Temp = m_pCamera->GetProjectionMatrix();
	xmf4x4Temp = cam->GetProjectionMatrix();
	XMStoreFloat4x4(&cb_mapped_pass_info_->m_xmf4x4CameraProjection, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
	xmf4x4Temp = matrix::Inverse(xmf4x4Temp);
	XMStoreFloat4x4(&cb_mapped_pass_info_->m_xmf4x4CameraProjectionInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

	//::memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &m_pCamera->GetPosition(), sizeof(XMFLOAT3));
	XMFLOAT3 xmf3WorldPos = camera_object_->FindComponent<TransformComponent>()->GetPosition(Space::world);
	memcpy(&cb_mapped_pass_info_->m_xmf3CameraPosition, &xmf3WorldPos, sizeof(XMFLOAT3));
}

#define MAX_PARTICLE_NUM 1000

vector<string> LoadMy::Split(istringstream& ss, const char delim)
{
	vector<string> result;
	string         str;

	while (getline(ss, str, delim)) result.push_back(str);

	return result;
}

vector<EnvironmentObjectData> LoadMy::LoadEnvMeshList(const char* path)
{
	vector<EnvironmentObjectData> result;

	string fullPath = "Resources/";
	fullPath += "/EnvMeshList.txt";
	std::ifstream ifs(fullPath);
	if (ifs.fail()) cout << "Error\n";
	else
	{
		//cout << "===========================\n";
		//cout << "EnvMeshList.txt\n";
		//cout << "===========================\n";

		string s;
		ifs >> s;
		//cout << s;
		ifs >> s;
		//cout << s << "\n";
		int n = stoi(s);
		//cout << "===========================\n";


		for (int i = 0; i < n; i++)
		{
			EnvironmentObjectData temp = {};
			ifs >> s;
			//cout << s << "\n";	// mesh name
			temp.strMeshName = s;

			ifs >> s;
			//cout << s << "\n";	// position
			{
				istringstream  ss(s);
				vector<string> pos  = Split(ss, ',');
				temp.xmf3Position.x = stof(pos[0]);
				temp.xmf3Position.y = stof(pos[1]);
				temp.xmf3Position.z = stof(pos[2]);
			}

			ifs >> s;
			//cout << s << "\n";	// rotation
			{
				istringstream  ss(s);
				vector<string> pos  = Split(ss, ',');
				temp.xmf4Rotation.x = stof(pos[0]);
				temp.xmf4Rotation.y = stof(pos[1]);
				temp.xmf4Rotation.z = stof(pos[2]);
				temp.xmf4Rotation.w = stof(pos[3]);
			}

			ifs >> s;
			//cout << s << "\n\n";	// mat name
			temp.strMatName = s;

			result.push_back(temp);
		}

		//cout << "===========================\n\n";
		ifs.close();
	}

	return result;
}

vector<ColliderObjectData> LoadMy::LoadColliderList(const char* path)
{
	vector<ColliderObjectData> result;

	string fullPath = "Resources/";
	fullPath += "/ColliderList.txt";
	std::ifstream ifs(fullPath);
	if (ifs.fail()) cout << "Error\n";
	else
	{
		//cout << "===========================\n";
		//cout << "ColliderList.txt\n";
		//cout << "===========================\n";

		string s;
		ifs >> s;
		//cout << s;
		ifs >> s;
		//cout << s << "\n";
		int n = stoi(s);
		//cout << "===========================\n";


		for (int i = 0; i < n; i++)
		{
			ColliderObjectData temp = {};
			ifs >> s;
			//cout << s << "\n";
			{
				istringstream  ss(s);
				vector<string> pos  = Split(ss, ',');
				temp.xmf3Position.x = stof(pos[0]);
				temp.xmf3Position.y = stof(pos[1]);
				temp.xmf3Position.z = stof(pos[2]);
			}

			ifs >> s;
			//cout << s << "\n";
			{
				istringstream  ss(s);
				vector<string> pos = Split(ss, ',');
				temp.xmf3Extents.x = stof(pos[0]);
				temp.xmf3Extents.y = stof(pos[1]);
				temp.xmf3Extents.z = stof(pos[2]);
			}

			ifs >> s;
			//cout << s << "\n";
			{
				istringstream  ss(s);
				vector<string> pos  = Split(ss, ',');
				temp.xmf4Rotation.x = stof(pos[0]);
				temp.xmf4Rotation.y = stof(pos[1]);
				temp.xmf4Rotation.z = stof(pos[2]);
				temp.xmf4Rotation.w = stof(pos[3]);
			}

			result.push_back(temp);
		}

		//cout << "===========================\n\n";
		ifs.close();
	}

	return result;
}

Object* Scene::FindObjectByName(const char* strName)
{
	for (auto iter = objects_.begin(); iter != objects_.end(); ++iter) if (strName == (*iter)->m_strName) return *iter;
	return nullptr;
}

void Scene::BuildObject()
{
	// Particle Pool Initialize
	for (int i = 0; i < MAX_PARTICLE_NUM; i++)
	{
		auto ptc = new Object();

		auto t  = new TransformComponent(ptc);
		auto pc = new ParticleComponent(ptc, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);

		ptc->SetActive(false);

		particle_pool.push_back(ptc);
	}
	//{
	//	Object* pe = new Object("particleEmitter");
	//	
	//	TransformComponent* t = new TransformComponent(pe);
	//	ParticleEmitterComponent* pec = new ParticleEmitterComponent(pe);

	//	pec->m_bIsBilboard = true;
	//	pec->m_fGravityModifier = 0.0f;

	//	t->Translate(-1, 2, -4);
	//	//pec->SetMaterialByName("ParticleTestMat");
	//	pec->SetMaterialByName("ParticleSparkMat");
	//	pec->m_fStartSpeed = fRange(30.0f, 40.0f);
	//	pec->m_RateOverTime = 10;
	//	pec->m_fCreateCooltime = 1.0f / 10;
	//	pec->m_nMaxParticles = 100;

	//	m_vecObject.push_back(pe);
	//	m_vecParticleEmitter.push_back(pe);
	//}

	{
		// muzzle, empty object for weapon
		auto muzzle     = new Object("muzzle");
		auto mTransform = new TransformComponent(muzzle);
		auto effect     = new EffectComponent(muzzle);
		auto mrcm       = new MeshRendererComponent(muzzle, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);

		mrcm->SetActive(false);
		mrcm->SetModelByName("muzzleFlash");
		mrcm->SetMaterialByName("MuzzleFlashMat");

		effect->SetDuration(0.05f);

		mTransform->Translate(0, 0.07f, 0.15f);
		objects_.push_back(muzzle);
		effect_render_group_.push_back(muzzle);

		// weapon
		auto weapon = new Object("pistol");

		auto wTransform = new TransformComponent(weapon);
		auto wcc        = new WeaponControllerComponent(weapon, muzzle, nullptr);
		auto mrc        = new MeshRendererComponent(weapon, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);

		mrc->SetModelByName("pistol");
		mrc->SetMaterialByName("PistolMat");

		objects_.push_back(weapon);
		non_anim_object_render_group_.push_back(weapon);
		muzzle->m_pParent = weapon;
	}
	{
		// player
		auto player = new Object("player"); //Vector3(-6.78999996,0,15.6700001)

		auto transform           = new TransformComponent(player);
		auto controller          = new InputManagerComponent(player);
		auto rigidbody           = new RigidbodyComponent(player);
		auto skinnedMeshRenderer = new SkinnedMeshRendererComponent(player, device_, command_list_,
			cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);
		auto humanoidController = new HumanoidControllerComponent(player, non_anim_object_render_group_[0]);
		auto humanoidAnimator   = new HumanoidAnimatorComponent(player, "Humanoid_Idle");
		auto sphereCollider     = new SphereColliderComponent(player, XMFLOAT3(0, 0.5f, 0), 0.5f);

		skinnedMeshRenderer->SetModelByName("human");
		skinnedMeshRenderer->SetMaterialByName("DefaultMaterial");
		//skinnedMeshRenderer->SetModelByName("newBody");
		//skinnedMeshRenderer->SetMaterialByName("newBody");
		transform->Translate(-6.78999996, 0, 15.6700001);
		transform->RotateXYZDegree(0, 210, 0);

		objects_.push_back(player);
		anim_object_render_group_.push_back(player);
		FindObjectByName("pistol")->m_pParent = player;
	}
	{
		auto head = new Object("head");

		auto transform = new TransformComponent(head);
		transform->Translate(0, 1.5f, 0.0f);
		objects_.push_back(head);

		head->m_pParent = FindObjectByName("player");
	}
	{
		auto look = new Object("lookAt");

		auto transform = new TransformComponent(look);
		transform->Translate(0, 1.5f, 5.0f);
		objects_.push_back(look);

		look->m_pParent = FindObjectByName("player");

		FindObjectByName("player")->FindComponent<HumanoidControllerComponent>()->SetLookAt(look);
	}
	{
		auto camera = new Object("camera");

		auto transform = new TransformComponent(camera);
		auto cam       = new CameraComponent(camera);

		//transform->Translate(0.6f, 1.8f, -2.2f);
		cam->SetHeadAndLookAt(FindObjectByName("head"), FindObjectByName("lookAt"), XMFLOAT3(0.6f, 0.3f, -2.2f));

		camera->m_pParent = FindObjectByName("player");

		camera_object_ = camera;
		objects_.push_back(camera);

		FindObjectByName("pistol")->FindComponent<WeaponControllerComponent>()->SetCam(camera);
	}

	{
		//CreateTargetBoard("TB0", XMFLOAT3(-4, 0.5f, -20), XMFLOAT3(0, 270, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		//CreateTargetBoard("TB1", XMFLOAT3(-10.5, 0.5f, -24), XMFLOAT3(0, 300, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB2", XMFLOAT3(-13, 0.5f, -22), XMFLOAT3(0, 270, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB3", XMFLOAT3(-11.8, 0.5f, -19.7), XMFLOAT3(0, 240, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		//CreateTargetBoard("TB4", XMFLOAT3(-17.1, 0.5f, -22), XMFLOAT3(0, 0, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB5", XMFLOAT3(-19.5, 0.5f, -25.3), XMFLOAT3(0, 0, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB6", XMFLOAT3(-23.8, 0.5f, -25.3), XMFLOAT3(0, 0, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB7", XMFLOAT3(-26.5, 0.5f, -21.7), XMFLOAT3(0, 0, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		//CreateTargetBoard("TB8", XMFLOAT3(-11.8, 0.5f, -13), XMFLOAT3(0, 70, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB9", XMFLOAT3(-10.6, 0.5f, -7.1), XMFLOAT3(0, 70, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB10", XMFLOAT3(-14.9, 0.5f, -8.2), XMFLOAT3(0, 90, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

		//CreateTargetBoard("TB11", XMFLOAT3(-19.5, 0.5f, -8), XMFLOAT3(0, 80, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//CreateTargetBoard("TB12", XMFLOAT3(-22.6, 0.5f, -6.1), XMFLOAT3(0, 90, 0), true,
		//	device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
	}

	/*===========================================================================
	* Env Object
	*==========================================================================*/
	LoadLevelEnvironment();
	{
		//// Door
		//{
		//	Object* env = new Object("door00");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-1.5, 0, -9.15);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env, false);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}
		//{
		//	Object* env = new Object("door01");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-1.5, 0, -16.85);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}
		//{
		//	Object* env = new Object("door02");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-5.85, 0, -23.5);
		//	transform->RotateXYZDegree(0, 90, 0);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}
		//{
		//	Object* env = new Object("door03");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-14.85, 0, -16.5);
		//	transform->RotateXYZDegree(0, 270, 0);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}
		//{
		//	Object* env = new Object("door04");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-27.5, 0, -14.85);
		//	transform->RotateXYZDegree(0, 180, 0);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}
		//{
		//	Object* env = new Object("door05");

		//	TransformComponent* transform = new TransformComponent(env);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(env, XMFLOAT3(0, 1.5f, 0), XMFLOAT3(1.5f, 1.5f, 0.15f), XMFLOAT4(0, 0, 0, 1));
		//	transform->Translate(-7.85, 0, -1.5);
		//	transform->RotateXYZDegree(0, 270, 0);
		//	MeshRendererComponent* mrc = new MeshRendererComponent(env, device_, command_list_, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		//	mrc->SetModelByName("tutorialHouse_door");
		//	mrc->SetMaterialByName("doorMat");
		//	DoorComponent* dc = new DoorComponent(env);


		//	m_vecObject.push_back(env);
		//	m_vecNonAnimObjectRenderGroup.push_back(env);
		//}

		//// Trigger
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-1.5, 0, -7.5);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 1.5), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorOpen(FindObjectByName("door00")->FindComponent<DoorComponent>());

		//	m_vecObject.push_back(trig);
		//}
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-1.5, 0, -15);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorOpen(FindObjectByName("door01")->FindComponent<DoorComponent>());

		//	m_vecObject.push_back(trig);
		//}
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-1.5, 0, -20);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorClose(FindObjectByName("door02")->FindComponent<DoorComponent>());
		//	trigEvent->AddSpawn(FindObjectByName("TB0")->FindComponent<Character>());

		//	m_vecObject.push_back(trig);
		//}
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-7.5, 0, -21);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorClose(FindObjectByName("door03")->FindComponent<DoorComponent>());
		//	trigEvent->AddDoorClose(FindObjectByName("door02")->FindComponent<DoorComponent>());
		//	trigEvent->AddSpawn(FindObjectByName("TB1")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB2")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB3")->FindComponent<Character>());

		//	m_vecObject.push_back(trig);
		//}
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-19, 0, -17);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorClose(FindObjectByName("door03")->FindComponent<DoorComponent>());
		//	trigEvent->AddDoorClose(FindObjectByName("door04")->FindComponent<DoorComponent>());
		//	trigEvent->AddSpawn(FindObjectByName("TB4")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB5")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB6")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB7")->FindComponent<Character>());

		//	m_vecObject.push_back(trig);
		//}
		//{
		//	Object* trig = new Object();

		//	TransformComponent* transform = new TransformComponent(trig);
		//	transform->Translate(-27, 0, -11.5);
		//	BoxColliderComponent* boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(1.5, 1.5, 2), XMFLOAT4(0, 0, 0, 1), true);
		//	EventComponent* trigEvent = new EventComponent(trig);
		//	trigEvent->AddDoorClose(FindObjectByName("door04")->FindComponent<DoorComponent>());
		//	trigEvent->AddDoorClose(FindObjectByName("door05")->FindComponent<DoorComponent>());
		//	trigEvent->AddSpawn(FindObjectByName("TB8")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB9")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB10")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB11")->FindComponent<Character>());
		//	trigEvent->AddSpawn(FindObjectByName("TB12")->FindComponent<Character>());

		//	m_vecObject.push_back(trig);
		//}
	}


	/*===========================================================================
	* Interaction Object
	*==========================================================================*/
	CreateDoor("d0", XMFLOAT3(-3, 0.0f, 6.2f), XMFLOAT3(0, 0, 0), true);
	CreateDoor("d1", XMFLOAT3(-16.7180004, 0, 2.38000011), XMFLOAT3(0, 90, 0), false);
	CreateDoor("d2", XMFLOAT3(-22.2059994, 0, 6.60099983), XMFLOAT3(0, 0, 0), false);
	CreateDoor("d3", XMFLOAT3(-22.9099998, 0, 19.2399998), XMFLOAT3(0, 0, 0), false);
	CreateDoor("d4", XMFLOAT3(-14.4899998, 0, 40.5599976), XMFLOAT3(0, 90, 0), true);
	CreateDoor("d5", XMFLOAT3(-3.1400001, 0, 40.7099991), XMFLOAT3(0, 90, 0), true);
	CreateDoor("d6", XMFLOAT3(-8.38860321, 0, 18.455431), XMFLOAT3(0, 20, 0), false);

	//FirstHallway
	CreateTargetBoard("t0", XMFLOAT3(-15.8400002, 0, 2.50999999), XMFLOAT3(0, 270, 0), true);

	//FirstRoom
	CreateTargetBoard("t1", XMFLOAT3(-24.0699997, 0, 5.28999996), XMFLOAT3(0, 270, 0), true);
	//CreateTargetBoard("t2", XMFLOAT3(-24.7700005, 0, 2.24000001), XMFLOAT3(0, 270, 0), true);
	CreateTargetBoard("t3", XMFLOAT3(-24.2700005, 0, -0.639999986), XMFLOAT3(0, 270, 0), true);

	//SecondHallway
	CreateTargetBoard("t4", XMFLOAT3(-26.2800007, 0, 10.4700003), XMFLOAT3(0, 270, 0), true);
	CreateTargetBoard("t5", XMFLOAT3(-24.6700001, 0, 18.3500004), XMFLOAT3(0, 180, 0), true);

	//FinalRoom
	CreateTargetBoard("t6", XMFLOAT3(-11.4799995, 1.3, 33.0699997), XMFLOAT3(0, 0, 0), true);
	//CreateTargetBoard("t7", XMFLOAT3(-6.11999989,	1.3, 33.0699997), XMFLOAT3(0, 0, 0), true);
	CreateTargetBoard("t8", XMFLOAT3(-2.5, 1.3, 35.8800011), XMFLOAT3(0, 90, 0), true);
	//CreateTargetBoard("t9", XMFLOAT3(-2.5, 1.3, 43.5999985), XMFLOAT3(0, 90, 0), true);
	CreateTargetBoard("t9", XMFLOAT3(-6.11999989, 1.3, 33.0699997), XMFLOAT3(0, 0, 0), true);
	//CreateTargetBoard("t10", XMFLOAT3(-5.78000021,	1.3, 46.8300018), XMFLOAT3(0, 180, 0), true);
	//CreateTargetBoard("t11", XMFLOAT3(-11.46, 1.3, 46.7999992), XMFLOAT3(0, 180, 0), true);
	//CreateTargetBoard("t11", XMFLOAT3(-11.46, 1.3, 46.7999992), XMFLOAT3(0, 180, 0), true);

	// TEST
	//CreateTargetBoard("t12", XMFLOAT3(-6.78999996, 0, 13.6700001), XMFLOAT3(0, 90, 0), false);


	event_count = 0; // ������ ������ �� �� �� ���� ������ �� ��

	{
		// ó�� �� �ݱ�
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 0);

		transform->Translate(-4.48, 0, 0.7); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("d0"));
		auto close     = EventInfo(EVENT::DCLOSE, temp);
		auto triggerOn = EventInfo(EVENT::TRIGGER, nullptr);

		trigEvent->AddEvent(close);
		trigEvent->AddEvent(triggerOn);


		//vector<Object*>* temp2 = new vector<Object*>;
		//EventInfo i = EventInfo(EVENT::VICTORY, temp2);
		//trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}
	{
		// ù �������� �� ����
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 0);

		transform->Translate(-14.5600004, 0, 0.879999995); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t0"));
		auto i         = EventInfo(EVENT::EWAKE, temp);
		auto triggerOn = EventInfo(EVENT::TRIGGER, nullptr);

		trigEvent->AddEvent(i);
		trigEvent->AddEvent(triggerOn);

		objects_.push_back(trig);
	}


	{
		// ù ���� �� ������ ù �� �� ����
		auto trig      = new Object();
		auto transform = new TransformComponent(trig);
		auto trigEvent = new EventComponent(trig, 1);

		transform->Translate(-8.81999969, 0, 39.6100006); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("d1"));
		auto i = EventInfo(EVENT::DOPEN, temp);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		temp2->push_back(FindObjectByName("t0"));
		i = EventInfo(EVENT::EDIED, temp2);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// ù �� ������ �� �� ����
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 1);

		transform->Translate(-19.2399998, 0.39199999, 0.949999988); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t1"));
		//temp->push_back(FindObjectByName("t2"));
		temp->push_back(FindObjectByName("t3"));
		auto i = EventInfo(EVENT::EWAKE, temp);
		trigEvent->AddEvent(i);

		i = EventInfo(EVENT::TRIGGER, nullptr);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// �� �� ������ ù �� ������ �� ����
		auto trig      = new Object();
		auto transform = new TransformComponent(trig);
		auto trigEvent = new EventComponent(trig, 3);

		transform->Translate(-19.2399998, 0.39199999, 0.949999988); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t1"));
		//temp->push_back(FindObjectByName("t2"));
		temp->push_back(FindObjectByName("t3"));
		auto i = EventInfo(EVENT::EDIED, temp);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		temp2->push_back(FindObjectByName("d2"));
		i = EventInfo(EVENT::DOPEN, temp2);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// �ι�° ���� �����ϸ� �� �� ����
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 3);

		transform->Translate(-24.0100002, 0.39199999, 10.3000002);

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t4"));
		temp->push_back(FindObjectByName("t5"));
		auto i = EventInfo(EVENT::EWAKE, temp);
		trigEvent->AddEvent(i);

		i = EventInfo(EVENT::TRIGGER, nullptr);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// �� ���� ������ �ι�° ���� ������ �� ����
		auto trig      = new Object();
		auto transform = new TransformComponent(trig);
		auto trigEvent = new EventComponent(trig, 5);

		transform->Translate(-19.2399998, 0.39199999, 0.949999988); //-6.78999996, 0, 15.6700001
		//transform->Translate(-6.78999996, 0, 15.6700001);//

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t4"));
		temp->push_back(FindObjectByName("t5"));
		auto i = EventInfo(EVENT::EDIED, temp);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		temp2->push_back(FindObjectByName("d3"));
		i = EventInfo(EVENT::DOPEN, temp2);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// ������ �� �����ϸ� �� �ݰ� �� ���� ����
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 5);

		transform->Translate(-8.18999958, 0.39199999, 39.4799995);

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t6"));
		//temp->push_back(FindObjectByName("t7"));
		temp->push_back(FindObjectByName("t8"));
		temp->push_back(FindObjectByName("t9"));
		//temp->push_back(FindObjectByName("t10"));
		//temp->push_back(FindObjectByName("t11"));
		auto i = EventInfo(EVENT::EWAKE, temp);
		trigEvent->AddEvent(i);

		i = EventInfo(EVENT::TRIGGER, nullptr);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		temp2->push_back(FindObjectByName("d4"));
		temp2->push_back(FindObjectByName("d5"));
		auto close = EventInfo(EVENT::DCLOSE, temp2);
		trigEvent->AddEvent(close);

		objects_.push_back(trig);
	}


	{
		// �� 12 ������ ������ �� ������ ��, �������� ���� �� ����
		auto trig      = new Object();
		auto transform = new TransformComponent(trig);
		auto trigEvent = new EventComponent(trig, 8);

		auto temp = new vector<Object*>;
		temp->push_back(FindObjectByName("t6"));
		//temp->push_back(FindObjectByName("t7"));
		temp->push_back(FindObjectByName("t8"));
		temp->push_back(FindObjectByName("t9"));
		//temp->push_back(FindObjectByName("t10"));
		//temp->push_back(FindObjectByName("t11"));
		auto i = EventInfo(EVENT::EDIED, temp);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		temp2->push_back(FindObjectByName("d5"));
		temp2->push_back(FindObjectByName("d6"));
		i = EventInfo(EVENT::DOPEN, temp2);
		trigEvent->AddEvent(i);

		objects_.push_back(trig);
	}


	{
		// ������������ ���ư� �� ������ �¸�
		auto trig        = new Object();
		auto transform   = new TransformComponent(trig);
		auto boxCollider = new BoxColliderComponent(trig, XMFLOAT3(0, 0, 0), XMFLOAT3(3, 1.5, 3), XMFLOAT4(0, 0, 0, 1),
			true);
		auto trigEvent = new EventComponent(trig, 8);

		transform->Translate(-11.7200003, 0.39199999, 23.3899994);

		auto i = EventInfo(EVENT::TRIGGER, nullptr);
		trigEvent->AddEvent(i);

		auto temp2 = new vector<Object*>;
		auto close = EventInfo(EVENT::VICTORY, temp2);
		trigEvent->AddEvent(close);

		objects_.push_back(trig);
	}

	/*===========================================================================
	* UI Object
	*==========================================================================*/

	{
		auto text = new Object("TextHpInfo");

		auto transform = new TransformComponent(text);
		auto TRC       = new TextRendererComponent(text, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);

		TRC->Initialize("consolas");
		TRC->SetMaterialByName("font_consolasMat");
		TRC->SetText("+");
		TRC->SetSize(16);
		transform->Translate(960 - 16, 540 + 24, 0);

		objects_.push_back(text);
		ui_render_group_.push_back(text);
	}
	{
		auto text = new Object("TextHpInfo");

		auto transform = new TransformComponent(text);
		auto TRC       = new TextRendererComponent(text, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);

		TRC->Initialize("consolas");
		TRC->SetMaterialByName("font_consolasMat");
		TRC->SetSize(10);
		auto tuiHP = new TextUIPlayerHPComponent(text, FindObjectByName("player"));
		transform->Translate(1200, 300, 0);

		objects_.push_back(text);
		ui_render_group_.push_back(text);
	}
	{
		auto text = new Object("TextAmmoInfo");

		auto transform = new TransformComponent(text);
		auto TRC       = new TextRendererComponent(text, device_, command_list_, cbv_cpu_descriptor_start_handle_,
			cbv_gpu_descriptor_start_handle_);
		TRC->Initialize("consolas");
		TRC->SetMaterialByName("font_consolasMat");
		TRC->SetSize(8);
		auto tuiAmmo = new TextUIAmmoComponent(text, FindObjectByName("pistol"));
		transform->Translate(1200, 270, 0);

		objects_.push_back(text);
		ui_render_group_.push_back(text);
	}
}

void Scene::ReloadLight()
{
	LightDataImporter        lightDataImporter;
	vector<LightDescription> vecLightDesc = lightDataImporter.Load("Resources/LightData.txt");
	string                   shadow("ShadowMap_");

	light_manager_->DeleteAll();

	for (int i = 0; i < vecLightDesc.size(); i++)
	{
		switch (vecLightDesc[i].lightType)
		{
		case LIGHT_POINT: light_manager_->AddPointLight(vecLightDesc[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_,
				cbv_gpu_descriptor_start_handle_);
			break;
		case LIGHT_DIRECTIONAL: light_manager_->AddDirectionalLight(vecLightDesc[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_,
				cbv_gpu_descriptor_start_handle_);
			break;
		case LIGHT_SPOT: light_manager_->AddSpotLight(vecLightDesc[i], device_, command_list_,
				cbv_cpu_descriptor_start_handle_,
				cbv_gpu_descriptor_start_handle_);
			break;
		case LIGHT_NONE: default: break;
		}

		if (vecLightDesc[i].bIsShadow)
		{
			string temp = to_string(i);
			temp        = shadow + temp;

			g_texture_manager.DeleteTexture(temp.c_str());

			g_texture_manager.AddDepthBufferTexture(temp.c_str(), device_, SHADOWMAPSIZE, SHADOWMAPSIZE,
				srv_cpu_descriptor_start_handle_, srv_gpu_descriptor_start_handle_);
			light_manager_->SetShadowMapName(temp.c_str(), i);
		}
	}
}

void Scene::LoadLevelEnvironment()
{
	vector<EnvironmentObjectData> envData = LoadMy::LoadEnvMeshList("Data");
	for (int i = 0; i < envData.size(); i++) CreateEnvObject(envData[i]);

	vector<ColliderObjectData> colliderData = LoadMy::LoadColliderList("Data");
	for (int i = 0; i < colliderData.size(); i++) CreateCollider(colliderData[i]);
}

void Scene::CreateEnvObject(const char* strModelName, const char* strMaterialName, XMFLOAT3 pos, XMFLOAT4 rot)
{
	auto env = new Object();

	auto transform = new TransformComponent(env);
	auto mrc       = new MeshRendererComponent(env, device_, command_list_, cbv_cpu_descriptor_start_handle_,
		cbv_gpu_descriptor_start_handle_);

	mrc->SetModelByName(strModelName);
	mrc->SetMaterialByName(strMaterialName);

	transform->SetPosition(pos);
	transform->Rotate(rot);

	objects_.push_back(env);
	non_anim_object_render_group_.push_back(env);
}

void Scene::CreateEnvObject(EnvironmentObjectData objData)
{
	CreateEnvObject(objData.strMeshName.c_str(), objData.strMatName.c_str(), objData.xmf3Position,
		objData.xmf4Rotation);
}

void Scene::CreateCollider(ColliderObjectData colData)
{
	auto box = new Object();

	XMFLOAT3 extents(colData.xmf3Extents.x * 0.5f, colData.xmf3Extents.y * 0.5f, colData.xmf3Extents.z * 0.5f);

	auto transform   = new TransformComponent(box);
	auto boxCollider = new BoxColliderComponent(box, colData.xmf3Position, extents, colData.xmf4Rotation);

	objects_.push_back(box);
}

void Scene::CreateTargetBoard(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool initialState)
{
	auto targetBoard = new Object(strName);

	auto transform           = new TransformComponent(targetBoard);
	auto skinnedMeshRenderer = new SkinnedMeshRendererComponent(targetBoard, device_, command_list_,
		cbv_cpu_descriptor_start_handle_,
		cbv_gpu_descriptor_start_handle_);
	// SkinnedMeshRenderer�� �۵��Ϸ��� Animator�� �־�� ��!
	// TBAC�� �۵��Ϸ��� TargetBoardController�� �־�� ��!
	auto TCC          = new TargetBoardControllerComponent(targetBoard);
	auto TAC          = new TargetBoardAnimatorComponent(targetBoard, "targetBoardStand");
	auto bodyCollider = new BoxColliderComponent(targetBoard, XMFLOAT3(0, 0.5f, 0), XMFLOAT3(0.3f, 0.5f, 0.1f),
		XMFLOAT4(0, 0, 0, 1), false, TAC, 1);
	auto headCollider = new BoxColliderComponent(targetBoard, XMFLOAT3(0, 1.2f, 0), XMFLOAT3(0.15f, 0.2f, 0.1f),
		XMFLOAT4(0, 0, 0, 1), false, TAC, 1);

	skinnedMeshRenderer->SetModelByName("targetBoardStand");
	skinnedMeshRenderer->SetMaterialByName("TargetBoardMat");
	transform->Translate(position);
	transform->RotateXYZDegree(rotationAngle);
	if (initialState) TCC->Die();
	else TCC->Revive();

	TCC->SetPlayer(FindObjectByName("player"));

	objects_.push_back(targetBoard);
	anim_object_render_group_.push_back(targetBoard);
}

void Scene::CreateDoor(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool isOpen)
{
	auto d         = new Object(strName);
	auto transform = new TransformComponent(d);
	transform->Translate(position);
	transform->RotateXYZDegree(rotationAngle);

	auto boxCollider = new BoxColliderComponent(d, XMFLOAT3(0, 1.5, 0), XMFLOAT3(2.9, 3.6, 0.2), XMFLOAT4(0, 0, 0, 1));
	auto mrcm        = new MeshRendererComponent(d, device_, command_list_, cbv_cpu_descriptor_start_handle_,
		cbv_gpu_descriptor_start_handle_);
	mrcm->SetModelByName("level00_door");
	mrcm->SetMaterialByName("level00_1");

	auto dc = new DoorComponent(d, isOpen);
	objects_.push_back(d);
	non_anim_object_render_group_.push_back(d);
}
