#include "pch.h"
#include "Framework.h"
#include "Scene.h"

void Framework::Update()
{
    m_Timer.Tick(0.0f);
    float fTimeElapsed = m_Timer.GetTimeElapsed();

    Input();
    if (m_pCurrentScene) m_pCurrentScene->Update(fTimeElapsed);

    HRESULT hResult = m_pd3dCommandAllocator->Reset();
    hResult         = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

    //�踮��� �� ����� �𸣰ڴ� å �����ϱ�
    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
    ::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
    d3dResourceBarrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource   = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    d3dResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

    D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    d3dRtvCPUDescriptorHandle.ptr += m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize;

    //���� ���� �����
    float pfClearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    ////======================================
	//// RenderPass1
    //if (m_pCurrentScene) m_pCurrentScene->RenderPass1();
    //m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
    ////======================================

    ////�ƿ�ǲ���� �ܰ迡 � RT�� ������ ���ϴ°ǰ���
    //m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
    ////m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, NULL);


    //if (m_pCurrentScene) m_pCurrentScene->RenderPass2();

    if (m_pCurrentScene) m_pCurrentScene->Render(d3dRtvCPUDescriptorHandle, d3dDsvCPUDescriptorHandle);


    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

    hResult = m_pd3dCommandList->Close();

    ID3D12CommandList* ppd3dCommandLists[] = {m_pd3dCommandList};
    m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

    WaitForGpuComplete();
    m_pdxgiSwapChain->Present(0, 0);

    MoveToNextFrame();

    m_Timer.GetFrameRate(m_pszFrameRate + 12, 37);
    ::SetWindowText(m_hWnd, m_pszFrameRate);
}

void Framework::Input()
{
    static UCHAR pKeysBuffer[256];
    bool         bProcessedByScene = false;
    if (GetKeyboardState(pKeysBuffer) && m_pCurrentScene) m_pCurrentScene->Input(pKeysBuffer);
}

void Framework::OnDestroy() {}

void Framework::OnCreate(HINSTANCE hInstance, HWND hWnd)
{
    m_hInstance = hInstance;
    m_hWnd      = hWnd;

    CreateDirect3DDevice();
    CreateCommandQueueAndList();
    CreateRtvAndDsvDescriptorHeaps();
    CreateSwapChain();

    m_hFenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr); // �� �߰�����.
    BuildScenes();
}

void Framework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    switch (nMessageID)
    {
    case WM_ACTIVATE:
        {
            if (LOWORD(wParam) == WA_INACTIVE)
            {
                //m_GameTimer.Stop();
            }
            else
            {
                //m_GameTimer.Start();
            }
            break;
        }
    case WM_SIZE:
        {
            m_nWndClientWidth  = LOWORD(lParam);
            m_nWndClientHeight = HIWORD(lParam);

            OnResizeBackBuffers();
            break;
        }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE: OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP: OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
        break;
    }
}

void Framework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    //if (m_pCurrentScene) m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void Framework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
    switch (nMessageID)
    {
    case WM_KEYUP: switch (wParam)
        {
        case VK_ESCAPE: PostQuitMessage(0);
            break;

        // ��ü ȭ�� �׽�Ʈ �뵵
        //case VK_F9:
        //{
        //	BOOL bFullScreenState = FALSE;
        //	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
        //	if (!bFullScreenState)
        //	{
        //		DXGI_MODE_DESC dxgiTargetParameters;
        //		dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        //		dxgiTargetParameters.Width = m_nWndClientWidth;
        //		dxgiTargetParameters.Height = m_nWndClientHeight;
        //		dxgiTargetParameters.RefreshRate.Numerator = 60;
        //		dxgiTargetParameters.RefreshRate.Denominator = 1;
        //		dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        //		dxgiTargetParameters.ScanlineOrdering =
        //			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        //		m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
        //	}
        //	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
        //	OnResizeBackBuffers();
        //	break;
        //}
        default: break;
        }
        break;
    default: break;
    }
    //if (m_pCurrentScene) m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

void Framework::RefreshScene()
{
    m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

    delete[] m_ppScenes;

    m_ppScenes      = new Scene*[1];
    m_ppScenes[0]   = new Scene();
    m_pCurrentScene = m_ppScenes[0];

    g_pCurrScene = m_pCurrentScene;

    m_pCurrentScene->Init(this, m_pd3dDevice, m_pd3dCommandList);

    m_pd3dCommandList->Close();
    ID3D12CommandList* ppd3dCommandLists[] = {m_pd3dCommandList};
    m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

    m_Timer.Reset();
}

ID3D12Resource* Framework::GetDSBResource() { return m_pd3dDepthStencilBuffer; }

void Framework::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
    ::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
    d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
    d3dDescriptorHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    d3dDescriptorHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    d3dDescriptorHeapDesc.NodeMask       = 0;
    HRESULT hResult                      = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);
    m_nRtvDescriptorIncrementSize        = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    d3dDescriptorHeapDesc.NumDescriptors = 1;
    d3dDescriptorHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    hResult                              = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);
    m_nDsvDescriptorIncrementSize        = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void Framework::CreateSwapChain()
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    m_nWndClientWidth  = rcClient.right - rcClient.left;
    m_nWndClientHeight = rcClient.bottom - rcClient.top;

    //Swapchain Descriptor �ۼ��ϱ�
    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
    ::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
    dxgiSwapChainDesc.BufferCount                        = m_nSwapChainBuffers;
    dxgiSwapChainDesc.BufferDesc.Width                   = m_nWndClientWidth;
    dxgiSwapChainDesc.BufferDesc.Height                  = m_nWndClientHeight;
    dxgiSwapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxgiSwapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiSwapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    dxgiSwapChainDesc.OutputWindow                       = m_hWnd;
    dxgiSwapChainDesc.SampleDesc.Count                   = m_bMsaa4xEnable
                                                               ? 4
                                                               : 1;
    dxgiSwapChainDesc.SampleDesc.Quality = m_bMsaa4xEnable
                                               ? m_nMsaa4xQualityLevels - 1
                                               : 0;
    dxgiSwapChainDesc.Windowed = TRUE;
    dxgiSwapChainDesc.Flags    = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    //�ۼ��� �����ڴ�� ���丮�� ������ֱ�
    HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pdxgiSwapChain);

    //�� ����������� ���� ����!
    if (!m_pdxgiSwapChain)
    {
        MessageBox(nullptr, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
        PostQuitMessage(0);
        return;
    }

    //��üȭ�� ��ȯ ����?
    hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

    //���� �ĸ���� �ε����� �޾ƿͼ� ������ ����
    m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

    //m_pdxgiSwapChain->SetFullscreenState(true, NULL);
}

void Framework::CreateRenderTargetViews()
{
    D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < m_nSwapChainBuffers; i++)
    {
        m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dSwapChainBackBuffers[i]);
        m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], nullptr, d3dRtvCPUDescriptorHandle);
        d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
    }
}

void Framework::CreateDepthStencilView()
{
    D3D12_RESOURCE_DESC d3dResourceDesc;
    d3dResourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    d3dResourceDesc.Alignment        = 0;
    d3dResourceDesc.Width            = m_nWndClientWidth;
    d3dResourceDesc.Height           = m_nWndClientHeight;
    d3dResourceDesc.DepthOrArraySize = 1;
    d3dResourceDesc.MipLevels        = 1;
    d3dResourceDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dResourceDesc.SampleDesc.Count = m_bMsaa4xEnable
                                           ? 4
                                           : 1;
    d3dResourceDesc.SampleDesc.Quality = m_bMsaa4xEnable
                                             ? m_nMsaa4xQualityLevels - 1
                                             : 0;
    d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    d3dResourceDesc.Flags  = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

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

    D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);

    D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
    ::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
    d3dDepthStencilViewDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    d3dDepthStencilViewDesc.Flags         = D3D12_DSV_FLAG_NONE;

    m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, nullptr, d3dDsvCPUDescriptorHandle);
}

void Framework::CreateDirect3DDevice()
{
    HRESULT hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_pdxgiFactory);

    IDXGIAdapter1* pd3dAdapter = nullptr;

    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
    {
        DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
        pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
        if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice))) break ;
    }

    if (!m_pd3dDevice)
    {
        hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
        hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
    }

    if (!m_pd3dDevice)
    {
        MessageBox(nullptr, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
        PostQuitMessage(0);
        return;
    }

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
    d3dMsaaQualityLevels.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    d3dMsaaQualityLevels.SampleCount      = 4;
    d3dMsaaQualityLevels.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    d3dMsaaQualityLevels.NumQualityLevels = 0;
    hResult                               = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
    m_nMsaa4xQualityLevels                = d3dMsaaQualityLevels.NumQualityLevels;
    m_bMsaa4xEnable                       = m_nMsaa4xQualityLevels > 1;

    hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
    for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
    m_hFenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

    gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    if (pd3dAdapter) pd3dAdapter->Release();
}

void Framework::CreateCommandQueueAndList()
{
    D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
    ::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
    d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    d3dCommandQueueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
    HRESULT hResult           = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pd3dCommandQueue);

    hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3dCommandAllocator);

    hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, nullptr, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
    hResult = m_pd3dCommandList->Close();
}

void Framework::BuildScenes()
{
    m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

    m_ppScenes      = new Scene*[1];
    m_ppScenes[0]   = new Scene();
    m_pCurrentScene = m_ppScenes[0];

    g_pCurrScene = m_pCurrentScene;

    m_pCurrentScene->Init(this, m_pd3dDevice, m_pd3dCommandList);

    m_pd3dCommandList->Close();
    ID3D12CommandList* ppd3dCommandLists[] = {m_pd3dCommandList};
    m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

    m_Timer.Reset();
}

void Framework::WaitForGpuComplete()
{
    const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
    HRESULT      hResult     = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

    if (m_pd3dFence->GetCompletedValue() < nFenceValue)
    {
        hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
        WaitForSingleObject(m_hFenceEvent, INFINITE);
    }
}

void Framework::MoveToNextFrame()
{
    m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

    UINT64  nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
    HRESULT hResult     = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

    if (m_pd3dFence->GetCompletedValue() < nFenceValue)
    {
        hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
        WaitForSingleObject(m_hFenceEvent, INFINITE);
    }
}

void Framework::OnResizeBackBuffers()
{
    WaitForGpuComplete();

    m_pd3dCommandList->Reset(m_pd3dCommandAllocator, nullptr);

    for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
    if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();

    DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
    m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
    m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
    m_nSwapChainBufferIndex = 0;

    CreateRenderTargetViews();
    CreateDepthStencilView();

    m_pd3dCommandList->Close();

    ID3D12CommandList* ppd3dCommandLists[] = {m_pd3dCommandList};
    m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

    WaitForGpuComplete();
}
