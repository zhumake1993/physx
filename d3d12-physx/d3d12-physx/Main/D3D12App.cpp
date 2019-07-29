#include "D3D12App.h"

#include "GameObject/Sky.h"
#include "GameObject/Box.h"
#include "GameObject/Skull.h"
#include "GameObject/Globe.h"
#include "GameObject/Grid.h"
#include "GameObject/Cylinder.h"
#include "GameObject/Sphere.h"
#include "GameObject/BoxPx.h"

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
extern bool gDrawWireframe;

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

	// ����ָ���б�Ϊ��ʼ��ָ����׼��
	ThrowIfFailed(gCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	gMainFrameResource->Initialize(gD3D12Device.Get());
	gPassCB->Initialize(gD3D12Device.Get(), 1, false);

	gPhysX.InitPhysics();

	gSceneManager->Initialize();

	BuildRenders();
	BuildFilters();

	// ִ�г�ʼ��ָ��
	ThrowIfFailed(gCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { gCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// �ȴ���ʼ�����
	FlushCommandQueue();

	return true;
}

void D3D12App::OnResize()
{
	D3DApp::OnResize();
}

void D3D12App::Update(const GameTimer& gt)
{
	// ɨ��֡��Դ��������
	gCurrFrameResourceIndex = (gCurrFrameResourceIndex + 1) % gNumFrameResources;

	// �ж�GPU�Ƿ���ɴ������õ�ǰ֡��Դ��ָ��
	// ���û�У���ȴ�GPU
	if (gMainFrameResource->GetCurrFence() != 0 && mFence->GetCompletedValue() < gMainFrameResource->GetCurrFence())
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(gMainFrameResource->GetCurrFence(), eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// �ƶ���
	mLightRotationAngle += 0.1f * gt.DeltaTime();
	XMMATRIX R = XMMatrixRotationY(mLightRotationAngle);
	for (int i = 0; i < 3; ++i) {
		XMVECTOR lightDir = XMLoadFloat3(&mBaseLightDirections[i]);
		lightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&mRotatedLightDirections[i], lightDir);
	}

	gSceneManager->Update(gt);

	if (gSceneManager->ChangingScene()) {

		// �ȴ�ָ�����
		// ��Ϊָ������п��ܲ�����������һ��������Դ��ָ��
		// ��˱��������ָ�����
		FlushCommandQueue();

		// ��ȡ��ǰ��ָ�������
		auto cmdListAlloc = gMainFrameResource->GetCurrCmdListAlloc();

		//����ָ���������������������ڴ�
		//������GPUִ���������ָ���б����ܽ��иò���
		ThrowIfFailed(cmdListAlloc->Reset());

		//����ָ���б��������ڴ�
		//������ʹ��ExecuteCommandList��ָ���б���ӽ�ָ����к����ִ�иò���
		ThrowIfFailed(gCommandList->Reset(cmdListAlloc.Get(), nullptr));

		gSceneManager->ChangeScene();
		mTimer.Reset();

		//�ر�ָ���б�
		ThrowIfFailed(gCommandList->Close());

		//��ָ���б���ӽ�ָ����У���GPUִ��
		ID3D12CommandList* cmdsLists[] = { gCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// �ȴ�ָ�����
		FlushCommandQueue();
	}

	mShadowMap->Update(mRotatedLightDirections[0]);

	UpdateFrameResource(gt);
}

void D3D12App::Draw(const GameTimer& gt)
{
	// ��ȡ��ǰ��ָ�������
	auto cmdListAlloc = gMainFrameResource->GetCurrCmdListAlloc();

	//����ָ���������������������ڴ�
	//������GPUִ���������ָ���б����ܽ��иò���
	ThrowIfFailed(cmdListAlloc->Reset());

	//����ָ���б��������ڴ�
	//������ʹ��ExecuteCommandList��ָ���б���ӽ�ָ����к����ִ�иò���
	ThrowIfFailed(gCommandList->Reset(cmdListAlloc.Get(), nullptr));

	//
	// ������
	//


	if (mIsWireframe) {
		mWireframe->Draw(mRenderTarget->Rtv(), DepthStencilView());
	} 
	else if (mIsDepthComplexityUseStencil) {
		mDepthComplexityUseStencil->Draw(mRenderTarget->Rtv(), DepthStencilView());
	}
	else if (mIsDepthComplexityUseBlend) {
		mDepthComplexityUseBlend->Draw(mRenderTarget->Rtv(), DepthStencilView());
		mInverseFilter->ExcuteInOut(mRenderTarget->Resource(), mRenderTarget->Resource());
	}
	else {
		// �ر�ƽ��ͷ�޳�
		gSceneManager->GetCurrMainCamera()->mFrustumCullingEnabled = false;

		mShadowMap->DrawSceneToShadowMap();

		mSsao->DrawNormalsAndDepth();
		mSsao->ComputeSsao(3);

		mCubeMap->SetShadow(mShadowMap->GetSrvDescriptorHeapPtr(), mShadowMap->Srv());
		// ����ֱ��ʹ�ø�Ssao��ͼ���������
		// ��Ϊ��Ssao��ͼ�ļ����ǻ����������������cubemap��6�������
		// ��ȷ�������ǻ���cubemap��6������������6����ͬ��Ssao��ͼ
		// ���������ƴ���ֱ��ʹ�ø�Ssao��ͼ
		mCubeMap->SetSsao(mSsao->GetSrvDescriptorHeapPtr(), mSsao->Srv());
		mCubeMap->DrawSceneToCubeMap();

		mMainRender->SetShadow(mShadowMap->GetSrvDescriptorHeapPtr(), mShadowMap->Srv());
		mMainRender->SetCubeMap(mCubeMap->GetSrvDescriptorHeapPtr(), mCubeMap->Srv());
		mMainRender->SetSsao(mSsao->GetSrvDescriptorHeapPtr(), mSsao->Srv());
		mMainRender->Draw(mRenderTarget->Rtv(), DepthStencilView());
	}

	//
	// ����
	//

	if (mIsBlur) {
		mBlurFilter->ExcuteInOut(mRenderTarget->Resource(), mRenderTarget->Resource(), 4);
	}

	if (mIsSobel) {
		mSobelFilter->ExcuteInOut(mRenderTarget->Resource(), mShaderResourceTemp->Resource());
		mInverseFilter->ExcuteInOut(mShaderResourceTemp->Resource(), mShaderResourceTemp->Resource());
		mMultiplyFilter->ExcuteInOut(mRenderTarget->Resource(), mShaderResourceTemp->Resource(), mRenderTarget->Resource());
	}

	//
	// ת���غ󻺳���Ⱦ
	//

	// �ı���Դ״̬
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mDrawQuad->Draw(mRenderTarget->Resource(), CurrentBackBufferView(), DepthStencilView());

	// �ı���Դ״̬
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//�ر�ָ���б�
	ThrowIfFailed(gCommandList->Close());

	//��ָ���б���ӽ�ָ����У���GPUִ��
	ID3D12CommandList* cmdsLists[] = { gCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	//����ǰ�󻺳�
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// ���ָ����fence��
	gMainFrameResource->GetCurrFence() = ++mCurrentFence;

	// ָ��ͬ��
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void D3D12App::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0) {
		SetCapture(mhMainWnd);
	}

	gSceneManager->GetCurrInputManager()->OnMouseDown(btnState, x, y);
}

void D3D12App::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

	gSceneManager->GetCurrInputManager()->OnMouseUp(btnState, x, y);
}

void D3D12App::OnMouseMove(WPARAM btnState, int x, int y)
{
	gSceneManager->GetCurrInputManager()->OnMouseMove(btnState, x, y);
}

void D3D12App::OnKeyDown(WPARAM vkCode)
{
	if (vkCode == '1') {
		mIsWireframe = !mIsWireframe;
		mIsDepthComplexityUseStencil = false;
		mIsDepthComplexityUseBlend = false;
	}

	if (vkCode == '2') {
		mIsDepthComplexityUseStencil = !mIsDepthComplexityUseStencil;
		mIsWireframe = false;
		mIsDepthComplexityUseBlend = false;
	}

	if (vkCode == '3') {
		mIsDepthComplexityUseBlend = !mIsDepthComplexityUseBlend;
		mIsWireframe = false;
		mIsDepthComplexityUseStencil = false;
	}

	if (vkCode == '4') {
		mIsBlur = !mIsBlur;
	}

	if (vkCode == '5') {
		mIsSobel = !mIsSobel;
	}

	if (vkCode == '6') {
		gSceneManager->GetCurrMainCamera()->mFrustumCullingEnabled = !gSceneManager->GetCurrMainCamera()->mFrustumCullingEnabled;
	}

	if (vkCode == 'M') {
		gDrawWireframe = !gDrawWireframe;
	}

	gSceneManager->GetCurrInputManager()->OnKeyDown(vkCode);
}

void D3D12App::OnKeyUp(WPARAM vkCode)
{
	gSceneManager->GetCurrInputManager()->OnKeyUp(vkCode);
}

void D3D12App::UpdateFrameResource(const GameTimer& gt)
{
	PassConstants mainPassCB;

	XMMATRIX view = gSceneManager->GetCurrMainCamera()->GetView();
	XMMATRIX proj = gSceneManager->GetCurrMainCamera()->GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	// ��NDC�ռ�[-1,+1]^2ת��������ռ�[0,1]^2
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

	mainPassCB.EyePosW = gSceneManager->GetCurrMainCamera()->GetPosition3f();
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
	// �ֶ����ó����İ�Χ��
	// ͨ����Ҫ�������еĶ����������Χ��
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