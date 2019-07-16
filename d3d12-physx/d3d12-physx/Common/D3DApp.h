#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3dUtil.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:

	static D3DApp* GetApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();//������ȾĿ����ͼ�������ѣ����ģ����ͼ��������
	virtual void OnResize();
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

	virtual void OnKeyDown(WPARAM vkCode) { }
	virtual void OnKeyUp(WPARAM vkCode) { }

protected:

	bool InitMainWindow();//��ʼ��������
	bool InitDirect3D();//��ʼ��D3D
	void CreateCommandObjects();//����ָ����У�ָ���������ָ���б�
	void CreateSwapChain();//����������

	void FlushCommandQueue();//flushָ�����

	ID3D12Resource* CurrentBackBuffer()const;//���ص�ǰ���ݻ���
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;//���ص�ǰ���ݻ�����ͼ����������
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;//�������ģ����ͼ����������

	void CalculateFrameStats();//����֡״̬

	void LogAdapters();//�г�����������
	void LogAdapterOutputs(IDXGIAdapter* adapter);//�г����������������������ʾ������һ�����������Թ���������
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);//�г������֧�ֵ���ʾģʽ��һ���������֧�ֶ�����ʾģʽ

protected:

	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr; //Ӧ��ʵ�����
	HWND      mhMainWnd = nullptr; //�����ھ��
	bool      mAppPaused = false;  //Ӧ���Ƿ���ͣ��
	bool      mMinimized = false;  //Ӧ���Ƿ���С����
	bool      mMaximized = false;  //Ӧ���Ƿ���󻯣�
	bool      mResizing = false;   //�Ƿ������϶�resize bars��
	bool      mFullscreenState = false;//Ӧ���Ƿ�ȫ����

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;//ָ�����
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;//ָ�������

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount]; //��ȾĿ�껺��
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;//���ģ�建��

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;//��ȾĿ����ͼ��������
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;//���ģ����ͼ��������
};

