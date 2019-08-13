#include "D3D12App.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

extern Setting gSetting;

extern ComPtr<ID3D12Device> gD3D12Device;
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;

#include "Common/FrameResource.h"
extern const int gNumFrameResources;
extern int gCurrFrameResourceIndex;
extern std::unique_ptr<MainFrameResource> gMainFrameResource;
extern std::unique_ptr<FrameResource<PassConstants>> gPassCB;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

D3D12App::D3D12App(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

D3D12App::~D3D12App()
{
}

bool D3D12App::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// 重置指令列表，为初始化指令做准备
	ThrowIfFailed(gCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	gMainFrameResource->Initialize(gD3D12Device.Get());
	gPassCB->Initialize(gD3D12Device.Get(), 1, false);

	gPhysX.InitPhysics();

	gSceneManager->Initialize();

	BuildRenders();
	BuildFilters();

	// 执行初始化指令
	ThrowIfFailed(gCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { gCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 等待初始化完成
	FlushCommandQueue();

	return true;
}

void D3D12App::OnResize()
{
	D3DApp::OnResize();
}

void D3D12App::Update(const GameTimer& gt)
{
	// 扫描帧资源环形数组
	gCurrFrameResourceIndex = (gCurrFrameResourceIndex + 1) % gNumFrameResources;

	// 判断GPU是否完成处理引用当前帧资源的指令
	// 如果没有，需等待GPU
	if (gMainFrameResource->GetCurrFence() != 0 && mFence->GetCompletedValue() < gMainFrameResource->GetCurrFence())
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(gMainFrameResource->GetCurrFence(), eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// 移动光
	mLightRotationAngle += 0.1f * gt.DeltaTime();
	XMMATRIX R = XMMatrixRotationY(mLightRotationAngle);
	for (int i = 0; i < 3; ++i) {
		XMVECTOR lightDir = XMLoadFloat3(&mBaseLightDirections[i]);
		lightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mRotatedLightDirections[i], lightDir);
	}

	// 获取当前的指令分配器
	auto cmdListAlloc = gMainFrameResource->GetCurrCmdListAlloc();

	//重置指令分配器以重用相关联的内存
	//必须在GPU执行完关联的指令列表后才能进行该操作
	ThrowIfFailed(cmdListAlloc->Reset());

	//重置指令列表以重用内存
	//必须在使用ExecuteCommandList将指令列表添加进指令队列后才能执行该操作
	ThrowIfFailed(gCommandList->Reset(cmdListAlloc.Get(), nullptr));

	gSceneManager->Update(gt);

	if (gSceneManager->ChangingScene()) {

		mTimer.Stop();

		//关闭指令列表
		ThrowIfFailed(gCommandList->Close());

		//将指令列表添加进指令队列，供GPU执行
		ID3D12CommandList* cmdsLists1[] = { gCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists1), cmdsLists1);

		// 标记指令到达该fence点
		gMainFrameResource->GetCurrFence() = ++mCurrentFence;

		// 指令同步
		mCommandQueue->Signal(mFence.Get(), mCurrentFence);


		// 等待指令完成
		// 因为指令队列中可能残留有引用上一个场景资源的指令
		// 因此必须先清空指令队列
		FlushCommandQueue();

		// 获取当前的指令分配器
		auto cmdListAlloc = gMainFrameResource->GetCurrCmdListAlloc();

		//重置指令分配器以重用相关联的内存
		//必须在GPU执行完关联的指令列表后才能进行该操作
		ThrowIfFailed(cmdListAlloc->Reset());

		//重置指令列表以重用内存
		//必须在使用ExecuteCommandList将指令列表添加进指令队列后才能执行该操作
		ThrowIfFailed(gCommandList->Reset(cmdListAlloc.Get(), nullptr));

		gSceneManager->ChangeScene();

		//关闭指令列表
		ThrowIfFailed(gCommandList->Close());

		//将指令列表添加进指令队列，供GPU执行
		ID3D12CommandList* cmdsLists2[] = { gCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists2), cmdsLists2);

		// 等待指令完成
		FlushCommandQueue();


		// 获取当前的指令分配器
		cmdListAlloc = gMainFrameResource->GetCurrCmdListAlloc();

		//重置指令分配器以重用相关联的内存
		//必须在GPU执行完关联的指令列表后才能进行该操作
		ThrowIfFailed(cmdListAlloc->Reset());

		//重置指令列表以重用内存
		//必须在使用ExecuteCommandList将指令列表添加进指令队列后才能执行该操作
		ThrowIfFailed(gCommandList->Reset(cmdListAlloc.Get(), nullptr));

		mTimer.Start();
	}

	if (GetCurrIsShadowMap()) {
		mShadowMap->Update(mRotatedLightDirections[0]);
	}

	UpdateFrameResource(gt);
}

void D3D12App::Draw(const GameTimer& gt)
{
	if (GetCurrIsWireframe()) {
		mWireframe->Draw(mRenderTarget->Rtv(), DepthStencilView());
	} 
	else if (GetCurrIsDepthComplexityUseStencil()) {
		mDepthComplexityUseStencil->Draw(mRenderTarget->Rtv(), DepthStencilView());
	}
	else if (GetCurrIsDepthComplexityUseBlend()) {
		mDepthComplexityUseBlend->Draw(mRenderTarget->Rtv(), DepthStencilView());
		mInverseFilter->ExcuteInOut(mRenderTarget->Resource(), mRenderTarget->Resource());
	}
	else {

		if (GetCurrIsShadowMap()) {
			mShadowMap->DrawSceneToShadowMap();
		}

		if (GetCurrIsSsao()) {
			mSsao->DrawNormalsAndDepth();
			mSsao->ComputeSsao(3);
		}

		if (GetCurrIsCubeMap()) {
			mCubeMap->SetShadow(mShadowMap->GetSrvDescriptorHeapPtr(), mShadowMap->Srv());
			// 这里直接使用该Ssao贴图是有问题的
			// 因为该Ssao贴图的计算是基于主摄像机，而非cubemap的6个摄像机
			// 正确的做法是基于cubemap的6个摄像机计算出6个不同的Ssao贴图
			// 这里做近似处理，直接使用该Ssao贴图
			mCubeMap->SetSsao(mSsao->GetSrvDescriptorHeapPtr(), mSsao->Srv());
			mCubeMap->DrawSceneToCubeMap();
		}

		mMainRender->SetShadow(mShadowMap->GetSrvDescriptorHeapPtr(), mShadowMap->Srv());
		mMainRender->SetCubeMap(mCubeMap->GetSrvDescriptorHeapPtr(), mCubeMap->Srv());
		mMainRender->SetSsao(mSsao->GetSrvDescriptorHeapPtr(), mSsao->Srv());
		mMainRender->Draw(mRenderTarget->Rtv(), DepthStencilView());
	}

	//
	// 后处理
	//

	if (GetCurrIsBlur()) {
		mBlurFilter->ExcuteInOut(mRenderTarget->Resource(), mRenderTarget->Resource(), 4);
	}

	if (GetCurrIsSobel()) {
		mSobelFilter->ExcuteInOut(mRenderTarget->Resource(), mShaderResourceTemp->Resource());
		mInverseFilter->ExcuteInOut(mShaderResourceTemp->Resource(), mShaderResourceTemp->Resource());
		mMultiplyFilter->ExcuteInOut(mRenderTarget->Resource(), mShaderResourceTemp->Resource(), mRenderTarget->Resource());
	}

	//
	// 转换回后缓冲渲染
	//

	// 改变资源状态
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mDrawQuad->Draw(mRenderTarget->Resource(), CurrentBackBufferView(), DepthStencilView());

	// 改变资源状态
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//关闭指令列表
	ThrowIfFailed(gCommandList->Close());

	//将指令列表添加进指令队列，供GPU执行
	ID3D12CommandList* cmdsLists[] = { gCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//交换前后缓冲
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// 标记指令到达该fence点
	gMainFrameResource->GetCurrFence() = ++mCurrentFence;

	// 指令同步
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void D3D12App::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0) {
		SetCapture(mhMainWnd);
	}

	GetCurrInputManager()->OnMouseDown(btnState, x, y);
}

void D3D12App::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

	GetCurrInputManager()->OnMouseUp(btnState, x, y);
}

void D3D12App::OnMouseMove(WPARAM btnState, int x, int y)
{
	GetCurrInputManager()->OnMouseMove(btnState, x, y);
}

void D3D12App::OnKeyDown(WPARAM vkCode)
{
	GetCurrInputManager()->OnKeyDown(vkCode);
}

void D3D12App::OnKeyUp(WPARAM vkCode)
{
	GetCurrInputManager()->OnKeyUp(vkCode);
}

void D3D12App::UpdateFrameResource(const GameTimer& gt)
{
	PassConstants mainPassCB;

	XMMATRIX view = GetCurrMainCamera()->GetView();
	XMMATRIX proj = GetCurrMainCamera()->GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	// 将NDC空间[-1,+1]^2转换至纹理空间[0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowMap->GetShadowTransform());

	XMStoreFloat4x4(&mainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&mainPassCB.ViewProjTex, XMMatrixTranspose(viewProjTex));
	XMStoreFloat4x4(&mainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));

	mainPassCB.EyePosW = GetCurrMainCamera()->GetTranslation3f();
	mainPassCB.RenderTargetSize = XMFLOAT2((float)gSetting.ClientWidth, (float)gSetting.ClientHeight);
	mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / gSetting.ClientWidth, 1.0f / gSetting.ClientHeight);
	mainPassCB.NearZ = 1.0f;
	mainPassCB.FarZ = 1000.0f;
	mainPassCB.TotalTime = gt.TotalTime();
	mainPassCB.DeltaTime = gt.DeltaTime();
	mainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mainPassCB.Lights[0].Direction = mRotatedLightDirections[0];
	mainPassCB.Lights[0].Strength = { 0.9f, 0.8f, 0.7f };
	mainPassCB.Lights[1].Direction = mRotatedLightDirections[1];
	mainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
	mainPassCB.Lights[2].Direction = mRotatedLightDirections[2];
	mainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	mainPassCB.EnableShadow = GetCurrIsShadowMap();
	mainPassCB.EnableSsao = GetCurrIsSsao();

	gPassCB->Copy(0, mainPassCB);

	mCubeMap->UpdatePassConstantsData(mainPassCB);
	mSsao->UpdateSsaoConstantData(mainPassCB);
}

void D3D12App::BuildRenders()
{
	mMainRender = std::make_unique<MainRender>();

	mDrawQuad = std::make_unique<DrawQuad>(gSetting.ClientWidth, gSetting.ClientHeight, gSetting.BackBufferFormat);
	mRenderTarget = std::make_unique<RenderTarget>(gSetting.ClientWidth, gSetting.ClientHeight, gSetting.BackBufferFormat);
	mShaderResourceTemp = std::make_unique<ShaderResource>(gSetting.ClientWidth, gSetting.ClientHeight, gSetting.BackBufferFormat);

	mWireframe = std::make_unique<Wireframe>();
	mDepthComplexityUseStencil = std::make_unique<DepthComplexityUseStencil>();
	mDepthComplexityUseBlend = std::make_unique<DepthComplexityUseBlend>();

	mCubeMap = std::make_unique<CubeMap>(DXGI_FORMAT_R8G8B8A8_UNORM, gSetting.DepthStencilFormat);
	mCubeMap->BuildCubeFaceCamera(0.0f, 2.0f, 0.0f);

	mShadowMap = std::make_unique<ShadowMap>(2048, 2048);
	// 手动设置场景的包围球
	// 通常需要迭代所有的顶点来计算包围球
	BoundingSphere sceneBounds;
	sceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f * 15.0f);
	mShadowMap->SetBoundingSphere(sceneBounds);

	mSsao = std::make_unique<Ssao>(gSetting.ClientWidth, gSetting.ClientHeight);
}

void D3D12App::BuildFilters()
{
	mBlurFilter = std::make_unique<BlurFilter>(gSetting.ClientWidth, gSetting.ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSobelFilter = std::make_unique<SobelFilter>(gSetting.ClientWidth, gSetting.ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	mInverseFilter = std::make_unique<InverseFilter>(gSetting.ClientWidth, gSetting.ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	mMultiplyFilter = std::make_unique<MultiplyFilter>(gSetting.ClientWidth, gSetting.ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
}