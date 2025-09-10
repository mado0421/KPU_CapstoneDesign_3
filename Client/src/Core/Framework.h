// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//
#include "../../pch.h"
#include "../Utils/Timer.h"
#define NUMSWAPCHAINBUFFER 2
#define NUMDEFAULTSTRINGCHAR 64

class Scene;

class Framework
{
    /////////////////////////////////////////////////////////////////////////////////////
    // 윈도우 창 생성부
    HINSTANCE m_hInstance;
    HWND      m_hWnd;
    int       m_nWndClientWidth;
    int       m_nWndClientHeight;
    _TCHAR    m_pszFrameRate[NUMDEFAULTSTRINGCHAR];

    /////////////////////////////////////////////////////////////////////////////////////
    // DirectX 12 생성부
    IDXGIFactory4*   m_pdxgiFactory;
    IDXGISwapChain3* m_pdxgiSwapChain;
    ID3D12Device*    m_pd3dDevice;

    bool m_bMsaa4xEnable        = false;
    UINT m_nMsaa4xQualityLevels = 0;

    static constexpr UINT m_nSwapChainBuffers = NUMSWAPCHAINBUFFER;
    UINT                  m_nSwapChainBufferIndex;

    ID3D12Resource*       m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
    ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap;
    UINT                  m_nRtvDescriptorIncrementSize;

    ID3D12Resource*       m_pd3dDepthStencilBuffer;
    ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap;
    UINT                  m_nDsvDescriptorIncrementSize;

    ID3D12CommandAllocator*    m_pd3dCommandAllocator;
    ID3D12CommandQueue*        m_pd3dCommandQueue;
    ID3D12GraphicsCommandList* m_pd3dCommandList;

    ID3D12Fence* m_pd3dFence;
    UINT64       m_nFenceValues[m_nSwapChainBuffers];
    HANDLE       m_hFenceEvent;
    /////////////////////////////////////////////////////////////////////////////////////
    // Scene
    Scene** m_ppScenes;
    Scene*  m_pCurrentScene;
    Timer   m_Timer;

public:
    void Update();
    void Input();
    void OnDestroy();
    void OnCreate(HINSTANCE hInstance, HWND hWnd);
    void OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

    void RefreshScene();

    //===============================================================
    // 테스트용 임시함수!!!!!!!!!!!!!!!!!!!!!
    // 테스트 했으면 꼭 제대로 정리할 것!!!!!!
    ID3D12Resource* GetDSBResource();
    //===============================================================

private:
    void CreateRtvAndDsvDescriptorHeaps();
    void CreateSwapChain();
    void CreateRenderTargetViews();
    void CreateDepthStencilView();
    void CreateDirect3DDevice();
    void CreateCommandQueueAndList();


    void BuildScenes();
    void WaitForGpuComplete();
    void MoveToNextFrame();
    void OnResizeBackBuffers();
};
