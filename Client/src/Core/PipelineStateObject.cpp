#include "pch.h"
#include "PipelineStateObject.h"

/*========================================================================
* Default PipelineStateObject
*=======================================================================*/
void PipelineStateObject::CreatePipelineState(ID3D12Device* pd3d_device, ID3D12RootSignature* pd3d_root_signature)
{
    m_pd3dPipelineState               = nullptr;
    ID3DBlob *pd3d_vertex_shader_blob = nullptr, *pd3d_pixel_shader_blob = nullptr, *pd3_geometry_shader_blob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_pipeline_state_desc;
    ::ZeroMemory(&d3d_pipeline_state_desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3d_pipeline_state_desc.pRootSignature        = pd3d_root_signature;
    d3d_pipeline_state_desc.VS                    = CreateVertexShader(&pd3d_vertex_shader_blob);
    d3d_pipeline_state_desc.PS                    = CreatePixelShader(&pd3d_pixel_shader_blob);
    d3d_pipeline_state_desc.GS                    = CreateGeometryShader(&pd3_geometry_shader_blob);
    d3d_pipeline_state_desc.RasterizerState       = CreateRasterizerState();
    d3d_pipeline_state_desc.BlendState            = CreateBlendState();
    d3d_pipeline_state_desc.DepthStencilState     = CreateDepthStencilState();
    d3d_pipeline_state_desc.InputLayout           = CreateInputLayout();
    d3d_pipeline_state_desc.SampleMask            = UINT_MAX;
    d3d_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3d_pipeline_state_desc.NumRenderTargets      = 2;
    d3d_pipeline_state_desc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3d_pipeline_state_desc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3d_pipeline_state_desc.SampleDesc.Count      = 1;
    d3d_pipeline_state_desc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT h_result                              = pd3d_device->CreateGraphicsPipelineState(&d3d_pipeline_state_desc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&m_pd3dPipelineState));

    if (pd3d_vertex_shader_blob) pd3d_vertex_shader_blob->Release();
    if (pd3d_pixel_shader_blob) pd3d_pixel_shader_blob->Release();
    if (pd3_geometry_shader_blob) pd3_geometry_shader_blob->Release();

    if (d3d_pipeline_state_desc.InputLayout.pInputElementDescs) delete[] d3d_pipeline_state_desc.InputLayout.pInputElementDescs;
}

D3D12_INPUT_LAYOUT_DESC PipelineStateObject::CreateInputLayout()
{
    constexpr UINT n_input_element_descs    = 6;
    const auto     pd3d_input_element_descs = new D3D12_INPUT_ELEMENT_DESC[n_input_element_descs];

    pd3d_input_element_descs[0] = {
        .SemanticName = "POSITION",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 0,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    pd3d_input_element_descs[1] = {
        .SemanticName = "NORMAL",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 12,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    pd3d_input_element_descs[2] = {
        .SemanticName = "TANGENT",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 24,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    pd3d_input_element_descs[3] = {
        .SemanticName = "BLENDINDICES",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32A32_UINT,
        .InputSlot = 0,
        .AlignedByteOffset = 36,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    pd3d_input_element_descs[4] = {
        .SemanticName = "BLENDWEIGHT",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 52,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };
    pd3d_input_element_descs[5] = {
        .SemanticName = "TEXCOORD",
        .SemanticIndex = 0,
        .Format = DXGI_FORMAT_R32G32_FLOAT,
        .InputSlot = 0,
        .AlignedByteOffset = 68,
        .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0
    };

    D3D12_INPUT_LAYOUT_DESC d3d_input_layout_desc;
    d3d_input_layout_desc.pInputElementDescs = pd3d_input_element_descs;
    d3d_input_layout_desc.NumElements        = n_input_element_descs;

    return d3d_input_layout_desc;
}

D3D12_RASTERIZER_DESC PipelineStateObject::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3d_rasterizer_desc;
    ::ZeroMemory(&d3d_rasterizer_desc, sizeof(D3D12_RASTERIZER_DESC));
    d3d_rasterizer_desc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3d_rasterizer_desc.CullMode              = D3D12_CULL_MODE_BACK;
    d3d_rasterizer_desc.FrontCounterClockwise = TRUE;
    d3d_rasterizer_desc.DepthBias             = 0;
    d3d_rasterizer_desc.DepthBiasClamp        = 0.0f;
    d3d_rasterizer_desc.SlopeScaledDepthBias  = 0.0f;
    d3d_rasterizer_desc.DepthClipEnable       = TRUE;
    d3d_rasterizer_desc.MultisampleEnable     = FALSE;
    d3d_rasterizer_desc.AntialiasedLineEnable = FALSE;
    d3d_rasterizer_desc.ForcedSampleCount     = 0;
    d3d_rasterizer_desc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3d_rasterizer_desc;
}

D3D12_BLEND_DESC PipelineStateObject::CreateBlendState()
{
    D3D12_BLEND_DESC d3d_blend_desc;
    ::ZeroMemory(&d3d_blend_desc, sizeof(D3D12_BLEND_DESC));
    d3d_blend_desc.AlphaToCoverageEnable                 = FALSE;
    d3d_blend_desc.IndependentBlendEnable                = FALSE;
    d3d_blend_desc.RenderTarget[0].BlendEnable           = FALSE;
    d3d_blend_desc.RenderTarget[0].LogicOpEnable         = FALSE;
    d3d_blend_desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_ONE;
    d3d_blend_desc.RenderTarget[0].DestBlend             = D3D12_BLEND_ZERO;
    d3d_blend_desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    d3d_blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    d3d_blend_desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    d3d_blend_desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    d3d_blend_desc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_NOOP;
    d3d_blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return d3d_blend_desc;
}

D3D12_DEPTH_STENCIL_DESC PipelineStateObject::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3d_depth_stencil_desc;
    ::ZeroMemory(&d3d_depth_stencil_desc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3d_depth_stencil_desc.DepthEnable                  = TRUE;
    d3d_depth_stencil_desc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
    d3d_depth_stencil_desc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3d_depth_stencil_desc.StencilEnable                = FALSE;
    d3d_depth_stencil_desc.StencilReadMask              = 0x00;
    d3d_depth_stencil_desc.StencilWriteMask             = 0x00;
    d3d_depth_stencil_desc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3d_depth_stencil_desc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3d_depth_stencil_desc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3d_depth_stencil_desc;
}

D3D12_SHADER_BYTECODE PipelineStateObject::CreateVertexShader(ID3DBlob** ppd3d_shader_blob)
{
    D3D12_SHADER_BYTECODE d3d_shader_byte_code;
    d3d_shader_byte_code.BytecodeLength  = 0;
    d3d_shader_byte_code.pShaderBytecode = nullptr;

    return d3d_shader_byte_code;
}

D3D12_SHADER_BYTECODE PipelineStateObject::CreatePixelShader(ID3DBlob** ppd3d_shader_blob)
{
    D3D12_SHADER_BYTECODE d3d_shader_byte_code;
    d3d_shader_byte_code.BytecodeLength  = 0;
    d3d_shader_byte_code.pShaderBytecode = nullptr;

    return d3d_shader_byte_code;
}

D3D12_SHADER_BYTECODE PipelineStateObject::CreateGeometryShader(ID3DBlob** ppd3d_shader_blob)
{
    D3D12_SHADER_BYTECODE d3d_shader_byte_code;
    d3d_shader_byte_code.BytecodeLength  = 0;
    d3d_shader_byte_code.pShaderBytecode = nullptr;

    return d3d_shader_byte_code;
}

D3D12_SHADER_BYTECODE PipelineStateObject::CompileShaderFromFile(const WCHAR* psz_file_name, LPCSTR psz_shader_name, LPCSTR psz_shader_profile, ID3DBlob** ppd3d_shader_blob)
{
    wstring shader_path = L"shaders/";
    shader_path += psz_file_name;


    constexpr UINT n_compile_flags = 0;

    ID3DBlob* pd3dErrorBlob = nullptr;
    HRESULT   result        = D3DCompileFromFile(shader_path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, psz_shader_name, psz_shader_profile, n_compile_flags, 0, ppd3d_shader_blob, &pd3dErrorBlob);
    char*     pErrorString  = nullptr;
    if (pd3dErrorBlob) pErrorString = static_cast<char*>(pd3dErrorBlob->GetBufferPointer());

    D3D12_SHADER_BYTECODE d3dShaderByteCode;
    d3dShaderByteCode.BytecodeLength  = (*ppd3d_shader_blob)->GetBufferSize();
    d3dShaderByteCode.pShaderBytecode = (*ppd3d_shader_blob)->GetBufferPointer();

    return d3dShaderByteCode;
}

void SRToRtPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3GeometryShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3GeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3GeometryShaderBlob) pd3GeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE SRToRtPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_ColorFromGBuffer", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE SRToRtPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_FlatScreen", "vs_5_1", ppd3dShaderBlob); }

/*========================================================================
* PackGBuffer PSO
*=======================================================================*/
void PackGBufferPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 2;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC PackGBufferPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = FALSE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE PackGBufferPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_PackGBuffer", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE PackGBufferPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_PackGBuffer", "vs_5_1", ppd3dShaderBlob); }

void AnimatedObjectPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 2;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC AnimatedObjectPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = FALSE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE AnimatedObjectPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_PackGBuffer", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE AnimatedObjectPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_AnimatedWVP", "vs_5_1", ppd3dShaderBlob); }

/*========================================================================
* RenderShadow PSO
*=======================================================================*/
void RenderShadowPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC RenderShadowPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 100;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 1.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE RenderShadowPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_RenderShadow", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE RenderShadowPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderSpotLightShadowObject", "vs_5_1", ppd3dShaderBlob); }

void RenderSpotLightShadowAnimatedObjectPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE RenderSpotLightShadowAnimatedObjectPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderSpotLightShadowAnimatedObject", "vs_5_1", ppd3dShaderBlob); }

/*========================================================================
* RenderPointLightShadow PSO
*=======================================================================*/
void RenderPointLightShadowPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC RenderPointLightShadowPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 1000;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 1.0;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE RenderPointLightShadowPSO::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"GeometryShader.hlsl", "GS_RenderPointLightShadow", "gs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE RenderPointLightShadowPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_RenderPointLightShadow", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE RenderPointLightShadowPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderPointLightShadow", "vs_5_1", ppd3dShaderBlob); }

void RenderPointLightShadowAnimatedObjectPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE RenderPointLightShadowAnimatedObjectPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderPointLightShadowAnimatedObject", "vs_5_1", ppd3dShaderBlob); }

/*========================================================================
* RenderDirectionalShadow PSO
*=======================================================================*/
void RenderDirectionalShadowPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_RASTERIZER_DESC RenderDirectionalShadowPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_NONE;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 500;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 1.0;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE RenderDirectionalShadowPSO::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"GeometryShader.hlsl", "GS_RenderDirectionalLightShadow", "gs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE RenderDirectionalShadowPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_RenderPointLightShadow", "ps_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE RenderDirectionalShadowPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderDirectionalLightShadow", "vs_5_1", ppd3dShaderBlob); }

void RenderDirectionalShadowAnimatedObjectPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 0;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE RenderDirectionalShadowAnimatedObjectPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
    return CompileShaderFromFile(L"VertexShader.hlsl", "VS_RenderDirectionalLightShadowAnimatedObject", "vs_5_1", ppd3dShaderBlob);
}

/*========================================================================
* ColorFromGBuffer PSO
*=======================================================================*/
void ColorFromGBufferPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_DEPTH_STENCIL_DESC ColorFromGBufferPSO::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
    ::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3dDepthStencilDesc.DepthEnable                  = FALSE;
    d3dDepthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
    d3dDepthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3dDepthStencilDesc.StencilEnable                = FALSE;
    d3dDepthStencilDesc.StencilReadMask              = 0x00;
    d3dDepthStencilDesc.StencilWriteMask             = 0x00;
    d3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3dDepthStencilDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE ColorFromGBufferPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_ColorFromGBufferAmbient", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC ColorFromGBufferPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE ColorFromGBufferPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_FlatScreen", "vs_5_1", ppd3dShaderBlob); }

/*========================================================================
* AddLight PSO
*=======================================================================*/
void AddLightPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_BLEND_DESC AddLightPSO::CreateBlendState()
{
    D3D12_BLEND_DESC d3dBlendDesc;
    ::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
    d3dBlendDesc.AlphaToCoverageEnable                 = FALSE;
    d3dBlendDesc.IndependentBlendEnable                = FALSE;
    d3dBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    d3dBlendDesc.RenderTarget[0].LogicOpEnable         = FALSE;
    d3dBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_ONE;
    d3dBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
    d3dBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
    d3dBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
    d3dBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_NOOP;
    d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC AddLightPSO::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
    ::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3dDepthStencilDesc.DepthEnable                  = FALSE;
    d3dDepthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
    d3dDepthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3dDepthStencilDesc.StencilEnable                = FALSE;
    d3dDepthStencilDesc.StencilReadMask              = 0x00;
    d3dDepthStencilDesc.StencilWriteMask             = 0x00;
    d3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3dDepthStencilDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE AddLightPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_AddLight", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC AddLightPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

D3D12_SHADER_BYTECODE AddLightPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_FlatScreen", "vs_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC DebugColorPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

/*========================================================================
* DebugColor PSO
*=======================================================================*/
D3D12_SHADER_BYTECODE DebugColorPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_ColorFromGBuffer", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC DebugDepthPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

/*========================================================================
* DebugDepth PSO
*=======================================================================*/
D3D12_SHADER_BYTECODE DebugDepthPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_DepthFromGBuffer", "ps_5_1", ppd3dShaderBlob); }

void EffectPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_BLEND_DESC EffectPSO::CreateBlendState()
{
    D3D12_BLEND_DESC d3dBlendDesc;
    ::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
    d3dBlendDesc.AlphaToCoverageEnable                 = FALSE;
    d3dBlendDesc.IndependentBlendEnable                = TRUE;
    d3dBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    d3dBlendDesc.RenderTarget[0].LogicOpEnable         = FALSE;
    d3dBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_NOOP;
    d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC EffectPSO::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
    ::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3dDepthStencilDesc.DepthEnable                  = TRUE;
    d3dDepthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ZERO;
    d3dDepthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3dDepthStencilDesc.StencilEnable                = FALSE;
    d3dDepthStencilDesc.StencilReadMask              = 0x00;
    d3dDepthStencilDesc.StencilWriteMask             = 0x00;
    d3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3dDepthStencilDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE EffectPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_Effect", "vs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE EffectPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_EffectAlpha", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC EffectPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_NONE;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

void ParticlePSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_BLEND_DESC ParticlePSO::CreateBlendState()
{
    D3D12_BLEND_DESC d3dBlendDesc;
    ::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
    d3dBlendDesc.AlphaToCoverageEnable                 = FALSE;
    d3dBlendDesc.IndependentBlendEnable                = FALSE;
    d3dBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    d3dBlendDesc.RenderTarget[0].LogicOpEnable         = FALSE;
    d3dBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_NOOP;
    d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC ParticlePSO::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
    ::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3dDepthStencilDesc.DepthEnable                  = TRUE;
    d3dDepthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ZERO;
    d3dDepthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3dDepthStencilDesc.StencilEnable                = FALSE;
    d3dDepthStencilDesc.StencilReadMask              = 0x00;
    d3dDepthStencilDesc.StencilWriteMask             = 0x00;
    d3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3dDepthStencilDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE ParticlePSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_Particle", "vs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE ParticlePSO::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"GeometryShader.hlsl", "GS_ParticleBillboard", "gs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE ParticlePSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_Particle", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC ParticlePSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = TRUE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

void TextPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr, *pd3dGeometryShaderBlob = nullptr;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.GS                    = CreateGeometryShader(&pd3dGeometryShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
    if (pd3dGeometryShaderBlob) pd3dGeometryShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_BLEND_DESC TextPSO::CreateBlendState()
{
    D3D12_BLEND_DESC d3dBlendDesc;
    ::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
    d3dBlendDesc.AlphaToCoverageEnable                 = FALSE;
    d3dBlendDesc.IndependentBlendEnable                = FALSE;
    d3dBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    d3dBlendDesc.RenderTarget[0].LogicOpEnable         = FALSE;
    d3dBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
    d3dBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    d3dBlendDesc.RenderTarget[0].LogicOp               = D3D12_LOGIC_OP_NOOP;
    d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC TextPSO::CreateDepthStencilState()
{
    D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
    ::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
    d3dDepthStencilDesc.DepthEnable                  = FALSE;
    d3dDepthStencilDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ZERO;
    d3dDepthStencilDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    d3dDepthStencilDesc.StencilEnable                = FALSE;
    d3dDepthStencilDesc.StencilReadMask              = 0x00;
    d3dDepthStencilDesc.StencilWriteMask             = 0x00;
    d3dDepthStencilDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_NEVER;
    d3dDepthStencilDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    d3dDepthStencilDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_NEVER;

    return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE TextPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_Text", "vs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE TextPSO::CreateGeometryShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"GeometryShader.hlsl", "GS_Text", "gs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE TextPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_Text", "ps_5_1", ppd3dShaderBlob); }

D3D12_RASTERIZER_DESC TextPSO::CreateRasterizerState()
{
    D3D12_RASTERIZER_DESC d3dRasterizerDesc;
    ::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
    d3dRasterizerDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    d3dRasterizerDesc.CullMode              = D3D12_CULL_MODE_BACK;
    d3dRasterizerDesc.FrontCounterClockwise = FALSE;
    d3dRasterizerDesc.DepthBias             = 0;
    d3dRasterizerDesc.DepthBiasClamp        = 0.0f;
    d3dRasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    d3dRasterizerDesc.DepthClipEnable       = TRUE;
    d3dRasterizerDesc.MultisampleEnable     = FALSE;
    d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
    d3dRasterizerDesc.ForcedSampleCount     = 0;
    d3dRasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    return d3dRasterizerDesc;
}

void ComputePipelineStateObject::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE ComputePipelineStateObject::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_main", "cs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE ComputePipelineStateObject::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
    wstring shaderPath = L"shaders/";
    shaderPath += pszFileName;


    UINT nCompileFlags = 0;

    ID3DBlob* pd3dErrorBlob = nullptr;
    HRESULT   result        = D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
    char*     pErrorString  = nullptr;
    if (pd3dErrorBlob) pErrorString = static_cast<char*>(pd3dErrorBlob->GetBufferPointer());

    D3D12_SHADER_BYTECODE d3dShaderByteCode;
    d3dShaderByteCode.BytecodeLength  = (*ppd3dShaderBlob)->GetBufferSize();
    d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

    return d3dShaderByteCode;
}

void VerticalBlurCPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE VerticalBlurCPSO::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_VerticalBlur", "cs_5_1", ppd3dShaderBlob); }

void HorizontalBlurCPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE HorizontalBlurCPSO::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_HorizontalBlur", "cs_5_1", ppd3dShaderBlob); }

void HDRFstPassCPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE HDRFstPassCPSO::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_DownScaleFirstPass", "cs_5_1", ppd3dShaderBlob); }

void HDRScdPassCPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE HDRScdPassCPSO::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_DownScaleSecondPass", "cs_5_1", ppd3dShaderBlob); }

void HDRToneMappingPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState            = nullptr;
    ID3DBlob *pd3dVertexShaderBlob = nullptr, *pd3dPixelShaderBlob = nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
    ::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    d3dPipelineStateDesc.pRootSignature        = pd3dRootSignature;
    d3dPipelineStateDesc.VS                    = CreateVertexShader(&pd3dVertexShaderBlob);
    d3dPipelineStateDesc.PS                    = CreatePixelShader(&pd3dPixelShaderBlob);
    d3dPipelineStateDesc.RasterizerState       = CreateRasterizerState();
    d3dPipelineStateDesc.BlendState            = CreateBlendState();
    d3dPipelineStateDesc.DepthStencilState     = CreateDepthStencilState();
    d3dPipelineStateDesc.InputLayout           = CreateInputLayout();
    d3dPipelineStateDesc.SampleMask            = UINT_MAX;
    d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    d3dPipelineStateDesc.NumRenderTargets      = 1;
    d3dPipelineStateDesc.RTVFormats[0]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
    d3dPipelineStateDesc.DSVFormat             = DXGI_FORMAT_UNKNOWN;
    d3dPipelineStateDesc.SampleDesc.Count      = 1;
    d3dPipelineStateDesc.Flags                 = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                            = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
    if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

    if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

D3D12_SHADER_BYTECODE HDRToneMappingPSO::CreateVertexShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"VertexShader.hlsl", "VS_FlatScreen", "vs_5_1", ppd3dShaderBlob); }

D3D12_SHADER_BYTECODE HDRToneMappingPSO::CreatePixelShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"PixelShader.hlsl", "PS_HDRToneMapping", "ps_5_1", ppd3dShaderBlob); }

void BloomCPSO::CreatePipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
    m_pd3dPipelineState             = nullptr;
    ID3DBlob* pd3dComputeShaderBlob = nullptr;

    D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc = {};

    d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
    d3dPipelineStateDesc.CS             = CreateComputeShader(&pd3dComputeShaderBlob);
    d3dPipelineStateDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;
    HRESULT hResult                     = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

    if (pd3dComputeShaderBlob) pd3dComputeShaderBlob->Release();
}

D3D12_SHADER_BYTECODE BloomCPSO::CreateComputeShader(ID3DBlob** ppd3dShaderBlob) { return CompileShaderFromFile(L"ComputeShader.hlsl", "CS_Bloom", "cs_5_1", ppd3dShaderBlob); }
